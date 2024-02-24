/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <mythtypes.h>

class MythEPGInfo
{
public:
  MythEPGInfo();
  MythEPGInfo(const Myth::ProgramPtr& epginfo);
  MythEPGInfo(unsigned chanid, time_t starttime, time_t endtime);

  bool IsNull() const;
  Myth::ProgramPtr GetPtr() const { return m_epginfo; }

  uint32_t ChannelID() const { return m_epginfo->channel.chanId; }
  const std::string& ChannelName() const { return m_epginfo->channel.channelName; }
  const std::string& Callsign() const { return m_epginfo->channel.callSign; }
  uint32_t SourceID() const { return m_epginfo->channel.sourceId; }
  const std::string& Title() const { return m_epginfo->title; }
  const std::string& Subtitle() const { return m_epginfo->subTitle; }
  const std::string& Description() const { return m_epginfo->description; }
  time_t StartTime() const { return m_epginfo->startTime; }
  time_t EndTime() const { return m_epginfo->endTime; }
  const std::string& ProgramID() const { return m_epginfo->programId; }
  const std::string& SeriesID() const { return m_epginfo->seriesId; }
  const std::string& Category() const { return m_epginfo->category; }
  const std::string& CategoryType() const { return m_epginfo->catType; }
  const std::string& ChannelNumber() const { return m_epginfo->channel.chanNum; }

  /**
   *
   * \brief Handle broadcast UID for MythTV program
   */
  static int MakeBroadcastID(unsigned int chanid, time_t starttime);
  static void BreakBroadcastID(int broadcastid, unsigned int *chanid, time_t *starttime);

private:
  Myth::ProgramPtr m_epginfo;
};
