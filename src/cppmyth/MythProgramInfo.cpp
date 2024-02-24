/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MythProgramInfo.h"

#include <cstdio> // for sprintf
#include <time.h> // for difftime

enum
{
  FLAGS_INITIALIZED   = 0x80000000,
  FLAGS_HAS_COVERART  = 0x00000001,
  FLAGS_HAS_FANART    = 0x00000002,
  FLAGS_HAS_BANNER    = 0x00000004,
  FLAGS_IS_VISIBLE    = 0x00000008,
  FLAGS_IS_LIVETV     = 0x00000010,
  FLAGS_IS_DELETED    = 0x00000020,
};

MythProgramInfo::MythProgramInfo()
: m_proginfo()
, m_props()
, m_cache()
{
}

MythProgramInfo::MythProgramInfo(const Myth::ProgramPtr& proginfo)
: m_proginfo(proginfo)
, m_props(new Props())
, m_cache(new Cache())
{
}

bool MythProgramInfo::IsNull() const
{
  if (!m_proginfo)
    return true;
  return m_proginfo.get() == NULL;
}

bool MythProgramInfo::operator ==(const MythProgramInfo &other)
{
  if (!this->IsNull() && !other.IsNull())
  {
    if (this->m_proginfo->channel.chanId == other.m_proginfo->channel.chanId &&
            this->m_proginfo->recording.startTs == other.m_proginfo->recording.startTs)
      return true;
  }
  return false;
}

bool MythProgramInfo::operator !=(const MythProgramInfo &other)
{
  return !(*this == other);
}

int MythProgramInfo::Duration() const
{
  return (int)difftime(m_proginfo->recording.endTs, m_proginfo->recording.startTs);
}

bool MythProgramInfo::IsWatched() const
{
  return ((m_proginfo->programFlags & 0x00000200) ? true : false);
}

bool MythProgramInfo::IsDeletePending() const
{
  return ((m_proginfo->programFlags & 0x00000080) ? true : false);
}

bool MythProgramInfo::HasBookmark() const
{
  return ((m_proginfo->programFlags & 0x00000010) ? true : false);
}

Myth::RS_t MythProgramInfo::Status() const
{
  return (Myth::RS_t)m_proginfo->recording.status;
}

bool MythProgramInfo::IsDamaged(uint32_t schemaVersion) const
{
  if (schemaVersion >= 1362)
    return ((m_proginfo && (m_proginfo->videoProps & 0x0400)) ? true : false);
  else
    return ((m_proginfo && (m_proginfo->videoProps & 0x0020)) ? true : false);
}

bool MythProgramInfo::IsVisible() const
{
  if ((m_cache->GetFlags(*this) & FLAGS_IS_VISIBLE))
    return true;
  return false;
}

bool MythProgramInfo::IsDeleted() const
{
  if ((m_cache->GetFlags(*this) & FLAGS_IS_DELETED))
    return true;
  return false;
}

bool MythProgramInfo::IsLiveTV() const
{
  if ((m_cache->GetFlags(*this) & FLAGS_IS_LIVETV))
    return true;
  return false;
}

bool MythProgramInfo::HasCoverart() const
{
  if ((m_cache->GetFlags(*this) & FLAGS_HAS_COVERART))
    return true;
  return false;
}

bool MythProgramInfo::HasFanart() const
{
  if ((m_cache->GetFlags(*this) & FLAGS_HAS_FANART))
    return true;
  return false;
}

void MythProgramInfo::SetPropsVideoFrameRate(float fps)
{
  m_props->videoFrameRate = fps;
}

float MythProgramInfo::GetPropsVideoFrameRate() const
{
  return m_props->videoFrameRate;
}

void MythProgramInfo::SetPropsVideoAspec(float aspec)
{
  m_props->videoAspec = aspec;
}

float MythProgramInfo::GetPropsVideoAspec() const
{
  return m_props->videoAspec;
}

void MythProgramInfo::SetPropsSerie(bool flag)
{
  m_props->serie = flag;
}

bool MythProgramInfo::GetPropsSerie() const
{
  return m_props->serie;
}

void MythProgramInfo::SetPropsBookmark(int seconds)
{
  m_props->bookmark = seconds;
}

int MythProgramInfo::GetPropsBookmark() const
{
  return m_props->bookmark;
}

int32_t MythProgramInfo::Cache::get_flags(const MythProgramInfo& prog) const
{
  m_flags |= FLAGS_INITIALIZED;

  // Has Artworks ?
  for (std::vector<Myth::Artwork>::const_iterator it = prog.m_proginfo->artwork.begin(); it != prog.m_proginfo->artwork.end(); ++it)
  {
    if (it->type == "coverart")
      m_flags |= FLAGS_HAS_COVERART;
    else if (it->type == "fanart")
      m_flags |= FLAGS_HAS_FANART;
    else if (it->type == "banner")
      m_flags |= FLAGS_HAS_BANNER;
  }

  // Is Visible ?
  // Filter out recording of special storage group Deleted
  // Filter out recording with duration less than 5 seconds
  // When  deleting a recording, it might not be deleted immediately but marked as 'pending delete'.
  // Depending on the protocol version the recording is moved to the group Deleted or
  // the 'delete pending' flag is set
  if (prog.Duration() >= 5)
  {
    if (prog.RecordingGroup() == "Deleted" || prog.IsDeletePending())
      m_flags |= FLAGS_IS_DELETED;
    else
      m_flags |= FLAGS_IS_VISIBLE;
  }

  // Is LiveTV ?
  if (prog.RecordingGroup() == "LiveTV")
    m_flags |= FLAGS_IS_LIVETV;

  return m_flags;
}

const std::string& MythProgramInfo::Cache::get_uid(const MythProgramInfo& prog) const
{
  char buf[48];
  snprintf(buf, sizeof(buf), "%u_%ld_%.3x",
          (unsigned)prog.m_proginfo->channel.chanId,
          (long)prog.m_proginfo->recording.startTs,
          (unsigned)prog.m_proginfo->recording.recordedId & 0xfff);
  m_UID.assign(buf);
  return m_UID;
}

const std::string& MythProgramInfo::Cache::get_grouping_title(const MythProgramInfo& prog) const
{
  // UTF-8 is safe when interpreting 7-bit ASCII characters. Therefore, the following
  // treatments do not require special care about locale

  const std::string& sin = prog.m_proginfo->title;
  std::string buf;
  buf.reserve(sin.size());
  // clean and truncate title at the first left parenthesis
  // i.e: "Ad Vitam (1/6)" => "Ad Vitam"
  bool brk = true;
  int trim = 0;
  for (std::string::const_iterator it = sin.begin(); it != sin.end(); ++it)
  {
    if (brk && *it == 0x28)
    {
      if (it != sin.begin())
        break;
      brk = false; // truncate disabled
    }
    switch (*it)
    {
    case 0x20:
    case 0x2f: // slash
    case 0x5c: // back-slash
    case 0x5b: // [
    case 0x5d: // ]
      buf.push_back(0x20);
      ++trim;
      break;
    default:
      buf.push_back(*it);
      trim = 0;
    }    
  }
  m_groupingTitle.assign(buf.c_str(), buf.size() - trim);
  return m_groupingTitle;
}
