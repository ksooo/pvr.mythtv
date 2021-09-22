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
  Myth::ProgramPtr GetPtr() const;

  uint32_t ChannelID() const;
  std::string ChannelName() const;
  std::string Callsign() const;
  uint32_t SourceID() const;
  std::string Title() const;
  std::string Subtitle() const;
  std::string Description() const;
  time_t StartTime() const;
  time_t EndTime() const;
  std::string ProgramID() const;
  std::string SeriesID() const;
  std::string Category() const;
  std::string CategoryType() const;
  std::string ChannelNumber() const;

  /**
   *
   * \brief Handle broadcast UID for MythTV program
   */
  static int MakeBroadcastID(unsigned int chanid, time_t starttime);
  static void BreakBroadcastID(int broadcastid, unsigned int *chanid, time_t *starttime);

private:
  Myth::ProgramPtr m_epginfo;
};
