/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MythRecordingRule.h"

MythRecordingRule::MythRecordingRule()
: m_recordSchedule(new Myth::RecordSchedule())
{
}

MythRecordingRule::MythRecordingRule(Myth::RecordSchedulePtr recordschedule)
: m_recordSchedule(recordschedule)
{
}

MythRecordingRule MythRecordingRule::DuplicateRecordingRule() const
{
  Myth::RecordSchedulePtr copy(new Myth::RecordSchedule());
  *copy = *m_recordSchedule;
  return MythRecordingRule(copy);
}

bool MythRecordingRule::UserJob(int jobnumber) const
{
  switch (jobnumber)
  {
  case 1:
    return m_recordSchedule->autoUserJob1;
  case 2:
    return m_recordSchedule->autoUserJob2;
  case 3:
    return m_recordSchedule->autoUserJob3;
  case 4:
    return m_recordSchedule->autoUserJob4;
  default:
    break;
  }
  return false;
}

void MythRecordingRule::SetUserJob(int jobnumber, bool enable)
{
  switch (jobnumber)
  {
  case 1:
    m_recordSchedule->autoUserJob1 = enable;
    break;
  case 2:
    m_recordSchedule->autoUserJob2 = enable;
    break;
  case 3:
    m_recordSchedule->autoUserJob3 = enable;
    break;
  case 4:
    m_recordSchedule->autoUserJob4 = enable;
    break;
  default:
    break;
  }
}
