/*
 *  Copyright (C) 2018-2021 Jean-Luc Barrière
 *  Copyright (C) 2018-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "MythScheduleHelper85.h"

// News in 29.0

class MythScheduleHelper91 : public MythScheduleHelper85
{
public:
  MythScheduleHelper91(MythScheduleManager *manager, Myth::Control *control)
  : MythScheduleHelper85(manager, control) { }

  virtual MythRecordingRule NewFromTimer(const MythTimerEntry& entry, bool withTemplate);
};
