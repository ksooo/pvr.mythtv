/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2013 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "demuxer/tsDemuxer.h"

#include <mythstream.h>
#include <kodi/AddonBase.h>

#include <set>
#include <vector>

#define AV_BUFFER_SIZE          131072

class PVRClientMythTV;

class ATTRIBUTE_HIDDEN AVInfo : public TSDemux::TSDemuxer
{
public:
  AVInfo(PVRClientMythTV& client, Myth::Stream *file);
  ~AVInfo();

  const unsigned char* ReadAV(uint64_t pos, size_t n);

  typedef struct
  {
    uint16_t pid;
    TSDemux::STREAM_TYPE stream_type;
    TSDemux::STREAM_INFO stream_info;
  } STREAM_AVINFO;

  bool GetMainStream(STREAM_AVINFO *info) const;
  std::vector<STREAM_AVINFO> GetStreams() const;

private:
  PVRClientMythTV& m_client;
  Myth::Stream *m_file;
  uint16_t m_channel;

  void Process();

  bool get_stream_data(TSDemux::STREAM_PKT* pkt);
  void populate_pvr_streams();
  bool update_pvr_stream(uint16_t pid);

  // AV raw buffer
  size_t m_av_buf_size;         ///< size of av buffer
  uint64_t m_av_pos;            ///< absolute position in av
  unsigned char* m_av_buf;      ///< buffer
  unsigned char* m_av_rbs;      ///< raw data start in buffer
  unsigned char* m_av_rbe;      ///< raw data end in buffer

  // Playback context
  TSDemux::AVContext* m_AVContext;
  uint16_t m_mainStreamPID;     ///< PID of main stream
  uint64_t m_DTS;               ///< absolute decode time of main stream
  uint64_t m_PTS;               ///< absolute presentation time of main stream

  std::set<uint16_t> m_nosetup;
  int m_AVStatus;
};
