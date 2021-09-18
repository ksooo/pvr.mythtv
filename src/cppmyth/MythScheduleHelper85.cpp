/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

///////////////////////////////////////////////////////////////////////////////
////
//// Version helper for backend version 85 (0.28-pre)
////

#include "MythScheduleHelper85.h"
#include "../tools.h"
#include "../settings.h"

#include <cstdio>
#include <cassert>

bool MythScheduleHelper85::FillTimerEntryWithUpcoming(MythTimerEntry& entry, const MythProgramInfo& recording) const
{
  //Only include timers which have an inactive status if the user has requested it (flag m_showNotRecording)
  switch (recording.Status())
  {
    //Upcoming recordings which are disabled due to being lower priority duplicates or already recorded
    case Myth::RS_EARLIER_RECORDING:  //will record earlier
    case Myth::RS_LATER_SHOWING:      //will record later
    case Myth::RS_CURRENT_RECORDING:  //Already in the current library
    case Myth::RS_PREVIOUS_RECORDING: //Previoulsy recorded but no longer in the library
      if (!m_manager->ShowNotRecording())
      {
        if (CMythSettings::GetExtraDebug())
          kodi::Log(ADDON_LOG_DEBUG, "85::%s: Skipping %s:%s on %s because status %d", __FUNCTION__,
                  recording.Title().c_str(), recording.Subtitle().c_str(), recording.ChannelName().c_str(),
                  recording.Status());
        return false;
      }
    default:
      break;
  }

  MythRecordingRuleNodePtr node = m_manager->FindRuleById(recording.RecordID());
  if (node)
  {
    MythRecordingRule rule = node->GetRule();
    // Relate the main rule as parent
    entry.parentIndex = MythScheduleManager::MakeIndex(node->GetMainRule());
    switch (rule.Type())
    {
      case Myth::RT_SingleRecord:
        return false; // Discard upcoming. We show only main rule.
      case Myth::RT_DontRecord:
        entry.recordingStatus = recording.Status();
        entry.timerType = TIMER_TYPE_DONT_RECORD;
        entry.isInactive = rule.Inactive();
        break;
      case Myth::RT_OverrideRecord:
        entry.recordingStatus = recording.Status();
        entry.timerType = TIMER_TYPE_OVERRIDE;
        entry.isInactive = rule.Inactive();
        break;
      default:
        entry.recordingStatus = recording.Status();
        if (node->GetMainRule().SearchType() == Myth::ST_ManualSearch)
          entry.timerType = TIMER_TYPE_UPCOMING_MANUAL;
        else
          {
            switch (recording.Status())
            {
              case Myth::RS_EARLIER_RECORDING:  //will record earlier
              case Myth::RS_LATER_SHOWING:      //will record later
                entry.timerType = TIMER_TYPE_UPCOMING_ALTERNATE;
                break;
              case Myth::RS_CURRENT_RECORDING:  //Already in the current library
                entry.timerType = TIMER_TYPE_UPCOMING_RECORDED;
                break;
              case Myth::RS_PREVIOUS_RECORDING: //Previoulsy recorded but no longer in the library
                entry.timerType = TIMER_TYPE_UPCOMING_EXPIRED;
                break;
              case Myth::RS_INACTIVE:           //Parent rule is disabled
                entry.timerType = TIMER_TYPE_RULE_INACTIVE;
                break;
              default:
                entry.timerType = TIMER_TYPE_UPCOMING;
                break;
            }
          }
    }
    entry.startOffset = rule.StartOffset();
    entry.endOffset = rule.EndOffset();
    entry.priority = rule.Priority();
    entry.expiration = GetRuleExpirationId(RuleExpiration(rule.AutoExpire(), 0, false));
  }
  else
    entry.timerType = TIMER_TYPE_ZOMBIE;

  switch (entry.timerType)
  {
    case TIMER_TYPE_UPCOMING:
    case TIMER_TYPE_RULE_INACTIVE:
    case TIMER_TYPE_UPCOMING_ALTERNATE:
    case TIMER_TYPE_UPCOMING_RECORDED:
    case TIMER_TYPE_UPCOMING_EXPIRED:
    case TIMER_TYPE_OVERRIDE:
    case TIMER_TYPE_UPCOMING_MANUAL:
      entry.epgCheck = true;
      break;
    default:
      entry.epgCheck = false;
  }

  entry.epgInfo = MythEPGInfo(recording.ChannelID(), recording.StartTime(), recording.EndTime());
  entry.description = "";
  entry.chanid = recording.ChannelID();
  entry.callsign = recording.Callsign();
  entry.startTime = recording.StartTime();
  entry.endTime = recording.EndTime();
  entry.title.assign(recording.Title());
  if (!recording.Subtitle().empty())
    entry.title.append(" (").append(recording.Subtitle()).append(")");
  if (recording.Season() || recording.Episode())
    entry.title.append(" - ").append(Myth::IntToString(recording.Season())).append(".").append(Myth::IntToString(recording.Episode()));
  entry.recordingGroup = GetRuleRecordingGroupId(recording.RecordingGroup());
  entry.entryIndex = MythScheduleManager::MakeIndex(recording); // upcoming index
  return true;
}
