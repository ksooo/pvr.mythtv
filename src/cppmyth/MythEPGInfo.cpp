/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MythEPGInfo.h"
#include "../tools.h"

MythEPGInfo::MythEPGInfo()
: m_epginfo()
{
}

MythEPGInfo::MythEPGInfo(const Myth::ProgramPtr& epginfo)
: m_epginfo(epginfo)
{
}

MythEPGInfo::MythEPGInfo(unsigned chanid, time_t starttime, time_t endtime)
: m_epginfo()
{
  m_epginfo.reset(new Myth::Program());
  m_epginfo->channel.chanId = chanid;
  m_epginfo->startTime = starttime;
  m_epginfo->endTime = endtime;
}

bool MythEPGInfo::IsNull() const
{
  if (!m_epginfo)
    return true;
  return m_epginfo.get() == NULL;
}

// Broacast ID is 32 bits integer and allows to identify a EPG item.
// MythTV backend doesn't provide one. So we make it encoding time and channel
// as below:
// 31. . . . . . . . . . . . . . . 15. . . . . . . . . . . . . . 0
// [   timecode (self-relative)   ][         channel Id          ]
// Timecode is the count of minutes since epoch modulo 0xFFFF. Now therefore it
// is usable for a period of +/- 32767 minutes (+/-22 days) around itself.

int MythEPGInfo::MakeBroadcastID(unsigned int chanid, time_t starttime)
{
  int timecode = (int)(difftime(starttime, 0) / INTERVAL_MINUTE) & 0xFFFF;
  return (int)((timecode << 16) | (chanid & 0xFFFF));
}

void MythEPGInfo::BreakBroadcastID(int broadcastid, unsigned int *chanid, time_t *attime)
{
  time_t now;
  int ntc, ptc, distance;
  struct tm epgtm;

  now = time(NULL);
  ntc = (int)(difftime(now, 0) / INTERVAL_MINUTE) & 0xFFFF;
  ptc = (broadcastid >> 16) & 0xFFFF; // removes arithmetic bits
  if (ptc > ntc)
    distance = (ptc - ntc) < 0x8000 ? ptc - ntc : ptc - ntc - 0xFFFF;
  else
    distance = (ntc - ptc) < 0x8000 ? ptc - ntc : ptc - ntc + 0xFFFF;
  localtime_r(&now, &epgtm);
  epgtm.tm_min += distance;
  // Time precision is minute, so we are looking for program started before next minute.
  epgtm.tm_sec = INTERVAL_MINUTE - 1;

  *attime = mktime(&epgtm);
  *chanid = (unsigned int)broadcastid & 0xFFFF;
}
