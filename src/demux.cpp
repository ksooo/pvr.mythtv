/*
 *      Copyright (C) 2013-2016 Team Kodi
 *      http://www.kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include <p8-platform/os.h>
#include <libXBMC_pvr.h>
#include <xbmc_codec_types.h>

#include "demux.h"
#include "demuxer/debug.h"

#include <climits>

#define LOGTAG                  "[DEMUX] "
#define POSMAP_PTS_INTERVAL     (PTS_TIME_BASE * 2)       // 2 secs
#define READAV_TIMEOUT          10000                     // 10 secs

using namespace ADDON;
using namespace P8PLATFORM;

void DemuxLog(int level, char *msg)
{
  if (msg && level != DEMUX_DBG_NONE)
  {
    bool doLog = g_bExtraDebug;
    addon_log_t loglevel = LOG_DEBUG;
    switch (level)
    {
    case DEMUX_DBG_ERROR:
      loglevel = LOG_ERROR;
      doLog = true;
      break;
    case DEMUX_DBG_WARN:
    case DEMUX_DBG_INFO:
      loglevel = LOG_INFO;
      break;
    case DEMUX_DBG_DEBUG:
    case DEMUX_DBG_PARSE:
    case DEMUX_DBG_ALL:
      loglevel = LOG_DEBUG;
      break;
    }
    if (XBMC && doLog)
      XBMC->Log(loglevel, LOGTAG "%s", msg);
  }
}

Demux::Demux(Myth::Stream *file)
  : CThread()
  , m_file(file)
  , m_channel(1)
  , m_demuxPacketBuffer(100)
  , m_av_buf_size(AV_BUFFER_SIZE)
  , m_av_pos(0)
  , m_av_buf(NULL)
  , m_av_rbs(NULL)
  , m_av_rbe(NULL)
  , m_AVContext(NULL)
  , m_mainStreamPID(0xffff)
  , m_DTS(PTS_UNSET)
  , m_PTS(PTS_UNSET)
  , m_dts(PTS_UNSET)
  , m_pts(PTS_UNSET)
  , m_startpts(PTS_UNSET)
  , m_pinTime(0)
  , m_curTime(0)
  , m_endTime(0)
  , m_isChangePlaced(false)
{
  memset(&m_streams, 0, sizeof(PVR_STREAM_PROPERTIES));
  m_av_buf = (unsigned char*)malloc(sizeof(*m_av_buf) * (m_av_buf_size + 1));
  if (m_av_buf)
  {
    m_av_rbs = m_av_buf;
    m_av_rbe = m_av_buf;

    if (g_bExtraDebug)
      TSDemux::DBGLevel(DEMUX_DBG_DEBUG);
    else
      TSDemux::DBGLevel(DEMUX_DBG_ERROR);
    TSDemux::SetDBGMsgCallback(DemuxLog);

    m_AVContext = new TSDemux::AVContext(this, m_av_pos, m_channel);

    CreateThread(true);
  }
  else
  {
    XBMC->Log(LOG_ERROR, LOGTAG "alloc AV buffer failed");
  }
}

Demux::~Demux()
{
  Abort();

  // Free AV context
  if (m_AVContext)
    SAFE_DELETE(m_AVContext);
  // Free AV buffer
  if (m_av_buf)
  {
    if (g_bExtraDebug)
      XBMC->Log(LOG_DEBUG, LOGTAG "free AV buffer: allocated size was %zu", m_av_buf_size);
    free(m_av_buf);
    m_av_buf = NULL;
  }
}

/*
 * Implement our AV reader
 */
const unsigned char* Demux::ReadAV(uint64_t pos, size_t n)
{
  // out of range
  if (n > m_av_buf_size)
    return NULL;

  // Already read ?
  size_t sz = m_av_rbe - m_av_buf;
  if (pos < m_av_pos || pos > (m_av_pos + sz))
  {
    // seek and reset buffer
    int64_t newpos = m_file->Seek((int64_t)pos, Myth::WHENCE_SET);
    if (newpos < 0)
      return NULL;
    m_av_pos = pos = (uint64_t)newpos;
    m_av_rbs = m_av_rbe = m_av_buf;
  }
  else
  {
    // move to the desired pos in buffer
    m_av_rbs = m_av_buf + (size_t)(pos - m_av_pos);
  }

  size_t dataread = m_av_rbe - m_av_rbs;
  if (dataread >= n)
    return m_av_rbs;
  // flush old data to free up space at the end
  memmove(m_av_buf, m_av_rbs, dataread);
  m_av_rbs = m_av_buf;
  m_av_rbe = m_av_rbs + dataread;
  m_av_pos = pos;
  // fill new data
  unsigned int len = (unsigned int)(m_av_buf_size - dataread);
  CTimeout timeout;
  while (!IsStopped())
  {
    int ret = m_file->Read(m_av_rbe, len);
    if (ret > 0)
    {
      m_av_rbe += ret;
      dataread += ret;
      len -= ret;
    }
    if (dataread >= n || ret < 0)
      break;
    if (!(timeout.IsSet()))
      timeout.Init(READAV_TIMEOUT);
    else if (!timeout.TimeLeft())
      break;
    usleep(100000);
  }
  return dataread >= n ? m_av_rbs : NULL;
}

void* Demux::Process()
{
  if (!m_AVContext)
  {
    XBMC->Log(LOG_ERROR, LOGTAG "%s: no AVContext", __FUNCTION__);
    return NULL;
  }

  int ret = 0;

  while (!IsStopped())
  {
    {
      CLockObject lock(m_mutex);
      ret = m_AVContext->TSResync();
    }
    if (ret != TSDemux::AVCONTEXT_CONTINUE)
      break;

    ret = m_AVContext->ProcessTSPacket();

    if (m_AVContext->HasPIDStreamData())
    {
      TSDemux::STREAM_PKT pkt;
      while (get_stream_data(&pkt))
      {
        if (pkt.streamChange)
        {
          // Update stream properties. Change will be pushed once setup is completed for all streams.
          if (update_pvr_stream(pkt.pid) && m_nosetup.empty())
            push_stream_change();
        }
        DemuxPacket* dxp = stream_pvr_data(&pkt);
        if (dxp)
          push_stream_data(dxp);
      }
    }
    if (m_AVContext->HasPIDPayload())
    {
      ret = m_AVContext->ProcessTSPayload();
      if (ret == TSDemux::AVCONTEXT_PROGRAM_CHANGE)
      {
        populate_pvr_streams();
        push_stream_change();
      }
    }

    if (ret < 0)
      XBMC->Log(LOG_NOTICE, LOGTAG "%s: error %d", __FUNCTION__, ret);

    if (ret == TSDemux::AVCONTEXT_TS_ERROR)
      m_AVContext->Shift();
    else
      m_AVContext->GoNext();
  }

  XBMC->Log(LOG_DEBUG, LOGTAG "%s: stopped with status %d", __FUNCTION__, ret);
  return NULL;
}

bool Demux::GetStreamProperties(PVR_STREAM_PROPERTIES* props)
{
  if (!m_nosetup.empty())
    XBMC->Log(LOG_NOTICE, LOGTAG "%s: incomplete setup", __FUNCTION__);

  CLockObject lock(m_mutex);
  m_isChangePlaced = false;
  for (unsigned i = 0; i < m_streams.iStreamCount; i++)
    memcpy(&props->stream[i], &m_streams.stream[i], sizeof(PVR_STREAM_PROPERTIES::PVR_STREAM));

  props->iStreamCount = m_streams.iStreamCount;
  return true;
}

void Demux::Flush(void)
{
  CLockObject lock(m_mutex);
  DemuxPacket* pkt(NULL);
  while (m_demuxPacketBuffer.Pop(pkt))
    PVR->FreeDemuxPacket(pkt);
}

void Demux::Abort()
{
  StopThread(0);
  Flush();
  m_streams.iStreamCount = 0;
}

DemuxPacket* Demux::Read()
{
  DemuxPacket* packet(NULL);
  if (IsStopped())
    return packet;
  if (m_demuxPacketBuffer.Pop(packet, 100))
    return packet;
  return PVR->AllocateDemuxPacket(0);
}

bool Demux::SeekTime(double time, bool backwards, double* startpts)
{
  // Current PTS must be valid to estimate offset
  if (m_startpts == PTS_UNSET)
    return false;
  // time is in MSEC not PTS_TIME_BASE. Rescale time to PTS (90Khz)
  int64_t pts = (int64_t)(time * PTS_TIME_BASE / 1000);
  // Compute desired time position
  int64_t desired = pts - m_startpts;

  XBMC->Log(LOG_DEBUG, LOGTAG "%s: bw:%d desired:%+6.3f buffered:%+6.3f", __FUNCTION__, backwards, (double)desired / PTS_TIME_BASE, (double)m_curTime / PTS_TIME_BASE);

  CLockObject lock(m_mutex);
  std::map<int64_t, AV_POSMAP_ITEM>::const_iterator it;
  it = m_posmap.upper_bound(desired);
  if (backwards && it != m_posmap.begin())
    --it;

  if (it != m_posmap.end())
  {
    int64_t new_time = it->first;
    uint64_t new_pos = it->second.av_pos;
    uint64_t new_pts = it->second.av_pts;
    XBMC->Log(LOG_DEBUG, LOGTAG "seek to %+6.3f pts=%" PRIu64, (double)new_time / PTS_TIME_BASE, new_pts);

    Flush();
    m_AVContext->GoPosition(new_pos);
    m_AVContext->ResetPackets();
    m_curTime = m_pinTime = new_time;
    m_DTS = m_PTS += new_pts - m_pts;
    m_dts = m_pts = new_pts;
  }

  *startpts = (double)m_startpts * DVD_TIME_BASE / PTS_TIME_BASE;
  return true;
}

int Demux::GetPlayingTime()
{
  double time_ms = (double)m_curTime * 1000 / PTS_TIME_BASE;
  if (time_ms > INT_MAX)
    return INT_MAX;
  return (int)time_ms;
}

bool Demux::get_stream_data(TSDemux::STREAM_PKT* pkt)
{
  TSDemux::ElementaryStream* es = m_AVContext->GetPIDStream();
  if (!es)
    return false;

  if (!es->GetStreamPacket(pkt))
    return false;

  uint64_t DTS = pkt->dts;
  uint64_t PTS = pkt->pts;
  if (m_startpts != PTS_UNSET)
  {
    pkt->dts = (DTS != PTS_UNSET ? m_dts + DTS - m_DTS : PTS_UNSET); // rebase dts
    pkt->pts = (PTS != PTS_UNSET ? m_pts + PTS - m_PTS : PTS_UNSET); // rebase pts
  }
  else if (DTS != PTS_UNSET && PTS != PTS_UNSET)
  {
    m_startpts = 0x80000000LL;
    m_dts = pkt->dts = m_startpts; // rebase dts
    m_pts = pkt->pts = m_startpts + PTS - DTS; // rebase pts
    m_DTS = DTS;
    m_PTS = PTS;
  }
  else
    return false;

  if (pkt->duration > PTS_TIME_BASE * 2)
  {
    pkt->duration = 0;
  }
  else if (pkt->pid == m_mainStreamPID)
  {
    // Fill duration map for main stream
    m_curTime += pkt->duration;
    if (m_curTime >= m_pinTime)
    {
      m_pinTime += POSMAP_PTS_INTERVAL;
      if (m_curTime > m_endTime)
      {
        AV_POSMAP_ITEM item;
        item.av_pts = pkt->pts;
        item.av_pos = m_AVContext->GetPosition();
        m_posmap.insert(std::make_pair(m_curTime, item));
        m_endTime = m_curTime;
      }
    }
    // Sync main DTS & PTS
    m_DTS = DTS;
    m_PTS = PTS;
    m_dts = pkt->dts;
    m_pts = pkt->pts;
  }
  return true;
}

void Demux::reset_posmap()
{
  if (m_posmap.empty())
    return;

  {
    CLockObject lock(m_mutex);
    m_posmap.clear();
    m_pinTime = m_curTime = m_endTime = 0;
  }
}

static inline int stream_identifier(int composition_id, int ancillary_id)
{
  return (composition_id & 0xffff) | ((ancillary_id & 0xffff) << 16);
}

static void recode_language(const char* muxLanguage, char* strLanguage)
{
  /*
   * While XBMC does'nt support them.
   * Fix unsupported language codes (EN 300 468 Annex F & J)
   * 'qaa'        : Original audio
   * 'qad','NAR'  : Audio Description
   */
  if (strncmp(muxLanguage, "qaa", 3) == 0 ||
      strncmp(muxLanguage, "qad", 3) == 0 ||
      strncmp(muxLanguage, "NAR", 3) == 0)
  {
    strLanguage[0] = 0;
    strLanguage[1] = 0;
    strLanguage[2] = 0;
    strLanguage[3] = 0;
  }
  else
  {
    strLanguage[0] = muxLanguage[0];
    strLanguage[1] = muxLanguage[1];
    strLanguage[2] = muxLanguage[2];
    strLanguage[3] = 0;
  }
}

void Demux::populate_pvr_streams()
{
  CLockObject Lock(m_mutex);

  uint16_t mainPid = 0xffff;
  int mainType = XBMC_CODEC_TYPE_UNKNOWN;
  const std::vector<TSDemux::ElementaryStream*> es_streams = m_AVContext->GetStreams();
  int count = 0;
  for (std::vector<TSDemux::ElementaryStream*>::const_iterator it = es_streams.begin(); it != es_streams.end(); ++it)
  {
    const char* codec_name = (*it)->GetStreamCodecName();
    xbmc_codec_t codec = CODEC->GetCodecByName(codec_name);
    if (codec.codec_type != XBMC_CODEC_TYPE_UNKNOWN)
    {
      memset(&m_streams.stream[count], 0, sizeof(PVR_STREAM_PROPERTIES::PVR_STREAM));

      // Find the main stream:
      // The best candidate would be the first video. Else the first audio
      switch (mainType)
      {
      case XBMC_CODEC_TYPE_VIDEO:
        break;
      case XBMC_CODEC_TYPE_AUDIO:
        if (codec.codec_type != XBMC_CODEC_TYPE_VIDEO)
          break;
      default:
        mainPid = (*it)->pid;
        mainType = codec.codec_type;
      }

      m_streams.stream[count].iPID           = (*it)->pid;
      m_streams.stream[count].iCodecId       = codec.codec_id;
      m_streams.stream[count].iCodecType     = codec.codec_type;
      recode_language((*it)->stream_info.language, m_streams.stream[count].strLanguage);
      m_streams.stream[count].iSubtitleInfo  = stream_identifier((*it)->stream_info.composition_id, (*it)->stream_info.ancillary_id);
      m_streams.stream[count].iFPSScale      = (*it)->stream_info.fps_scale;
      m_streams.stream[count].iFPSRate       = (*it)->stream_info.fps_rate;
      m_streams.stream[count].iHeight        = (*it)->stream_info.height;
      m_streams.stream[count].iWidth         = (*it)->stream_info.width;
      m_streams.stream[count].fAspect        = (*it)->stream_info.aspect;
      m_streams.stream[count].iChannels      = (*it)->stream_info.channels;
      m_streams.stream[count].iSampleRate    = (*it)->stream_info.sample_rate;
      m_streams.stream[count].iBlockAlign    = (*it)->stream_info.block_align;
      m_streams.stream[count].iBitRate       = (*it)->stream_info.bit_rate;
      m_streams.stream[count].iBitsPerSample = (*it)->stream_info.bits_per_sample;

      count++;
      m_AVContext->StartStreaming((*it)->pid);

      // Add stream to no setup set
      if (!(*it)->has_stream_info)
        m_nosetup.insert((*it)->pid);

      if (g_bExtraDebug)
        XBMC->Log(LOG_DEBUG, LOGTAG "%s: register PES %.4x %s", __FUNCTION__, (*it)->pid, codec_name);
    }
  }
  m_streams.iStreamCount = count;
  // Renew main stream
  m_mainStreamPID = mainPid;
}

bool Demux::update_pvr_stream(uint16_t pid)
{
  TSDemux::ElementaryStream* es = m_AVContext->GetStream(pid);
  if (!es)
    return false;

  const char* codec_name = es->GetStreamCodecName();
  xbmc_codec_t codec = CODEC->GetCodecByName(codec_name);
  if (g_bExtraDebug)
    XBMC->Log(LOG_DEBUG, LOGTAG "%s: update info PES %.4x %s", __FUNCTION__, es->pid, codec_name);

  CLockObject Lock(m_mutex);

  // find stream index for pid
  for (unsigned i = 0; i < m_streams.iStreamCount; i++)
  {
    if (m_streams.stream[i].iPID == es->pid)
    {
      m_streams.stream[i].iCodecId       = codec.codec_id;
      m_streams.stream[i].iCodecType     = codec.codec_type;
      recode_language(es->stream_info.language, m_streams.stream[i].strLanguage);
      m_streams.stream[i].iSubtitleInfo  = stream_identifier(es->stream_info.composition_id, es->stream_info.ancillary_id);
      m_streams.stream[i].iFPSScale      = es->stream_info.fps_scale;
      m_streams.stream[i].iFPSRate       = es->stream_info.fps_rate;
      m_streams.stream[i].iHeight        = es->stream_info.height;
      m_streams.stream[i].iWidth         = es->stream_info.width;
      m_streams.stream[i].fAspect        = es->stream_info.aspect;
      m_streams.stream[i].iChannels      = es->stream_info.channels;
      m_streams.stream[i].iSampleRate    = es->stream_info.sample_rate;
      m_streams.stream[i].iBlockAlign    = es->stream_info.block_align;
      m_streams.stream[i].iBitRate       = es->stream_info.bit_rate;
      m_streams.stream[i].iBitsPerSample = es->stream_info.bits_per_sample;

      if (es->has_stream_info)
      {
        // Now stream is setup. Remove it from no setup set
        std::set<uint16_t>::iterator it = m_nosetup.find(es->pid);
        if (it != m_nosetup.end())
        {
          m_nosetup.erase(it);
          if (m_nosetup.empty())
            XBMC->Log(LOG_DEBUG, LOGTAG "%s: setup is completed", __FUNCTION__);
        }
      }
      return true;
    }
  }
  return false;
}

void Demux::push_stream_change()
{
  if (!m_isChangePlaced)
  {
    bool ret = false;
    DemuxPacket* dxp  = PVR->AllocateDemuxPacket(0);
    dxp->iStreamId    = DMX_SPECIALID_STREAMCHANGE;

    while (!IsStopped() && !(ret = m_demuxPacketBuffer.Push(dxp)))
        usleep(100000);
    if (!ret)
      PVR->FreeDemuxPacket(dxp);
    else
    {
      m_isChangePlaced = true;
      XBMC->Log(LOG_DEBUG, LOGTAG "%s: done", __FUNCTION__);
    }
  }
}

DemuxPacket* Demux::stream_pvr_data(TSDemux::STREAM_PKT* pkt)
{
  if (!pkt)
    return NULL;

  DemuxPacket* dxp = PVR->AllocateDemuxPacket(pkt->size);
  if (dxp)
  {
    if (pkt->size > 0 && pkt->data)
      memcpy(dxp->pData, pkt->data, pkt->size);

    dxp->iStreamId = (int)(pkt->pid);
    dxp->iSize = pkt->size;
    dxp->duration = (double)pkt->duration * DVD_TIME_BASE / PTS_TIME_BASE;
    if (pkt->dts != PTS_UNSET)
      dxp->dts = (double)pkt->dts * DVD_TIME_BASE / PTS_TIME_BASE;
    else
      dxp->dts = DVD_NOPTS_VALUE;
    if (pkt->pts != PTS_UNSET)
      dxp->pts = (double)pkt->pts * DVD_TIME_BASE / PTS_TIME_BASE;
    else
      dxp->pts = DVD_NOPTS_VALUE;
  }
  return dxp;
}

void Demux::push_stream_data(DemuxPacket* dxp)
{
  if (dxp)
  {
    bool ret = false;
    while (!IsStopped() && !(ret = m_demuxPacketBuffer.Push(dxp)))
        usleep(100000);
    if (!ret)
      PVR->FreeDemuxPacket(dxp);
  }
}
