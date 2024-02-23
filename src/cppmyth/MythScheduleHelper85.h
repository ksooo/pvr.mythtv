/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "MythScheduleHelper76.h"

// News in 0.28

class MythScheduleHelper85 : public MythScheduleHelper76
{
public:
  explicit MythScheduleHelper85(MythScheduleManager *manager)
  : MythScheduleHelper76(manager) { }

  virtual bool FillTimerEntryWithUpcoming(MythTimerEntry& entry, const MythProgramInfo& recording) const;
};
