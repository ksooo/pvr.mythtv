/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "MythScheduleHelper75.h"

// News in 0.27

class MythScheduleHelper76 : public MythScheduleHelper75
{
public:
  MythScheduleHelper76(MythScheduleManager *manager, Myth::Control *control)
  : MythScheduleHelper75(manager, control) { }

  virtual bool FillTimerEntryWithRule(MythTimerEntry& entry, const MythRecordingRuleNode& node) const;
  //virtual bool FillTimerEntry(MythTimerEntry& entry, const MythProgramInfo& recording) const;
  virtual MythRecordingRule NewFromTimer(const MythTimerEntry& entry, bool withTemplate);
};
