/*
 *  Copyright (C) 2018-2021 Jean-Luc Barrière
 *  Copyright (C) 2018 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/AddonBase.h>

#include <queue>
#include <vector>

class Task
{
public:
  virtual ~Task() { }
  virtual void Execute() = 0;
};

class TaskHandlerPrivate;

class ATTRIBUTE_HIDDEN TaskHandler
{
public:
  TaskHandler();
  ~TaskHandler();

  void ScheduleTask(Task *task, unsigned delayMs = 0);
  void Clear();
  void Suspend();
  bool Resume();

private:
  TaskHandlerPrivate *m_p;
};
