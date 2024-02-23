/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "MythScheduleHelperNoHelper.h"

// MythTV 0.26

class MythScheduleHelper75 : public MythScheduleHelperNoHelper
{
public:
  explicit MythScheduleHelper75(MythScheduleManager *manager)
  : MythScheduleHelperNoHelper(manager) { }

  virtual MythTimerTypeList GetTimerTypes() const;
  virtual bool SameTimeslot(const MythRecordingRule& first, const MythRecordingRule& second) const;
  virtual bool FillTimerEntryWithRule(MythTimerEntry& entry, const MythRecordingRuleNode& node) const;
  virtual bool FillTimerEntryWithUpcoming(MythTimerEntry& entry, const MythProgramInfo& recording) const;
  virtual MythRecordingRule NewFromTemplate(const MythEPGInfo& epgInfo);
  virtual MythRecordingRule NewFromTimer(const MythTimerEntry& entry, bool withTemplate);
  virtual MythRecordingRule MakeDontRecord(const MythRecordingRule& rule, const MythProgramInfo& recording);
  virtual MythRecordingRule MakeOverride(const MythRecordingRule& rule, const MythProgramInfo& recording);

  // Members return default id for setting
  virtual int GetRuleDupMethodDefaultId() const { return Myth::DM_CheckSubtitleThenDescription; }
  virtual int GetRuleExpirationDefaultId() const { return EXPIRATION_ALLOW_EXPIRE_ID; }

protected:
  // Following members aren't thread safe: SHOULD be called by a locked method
  virtual const MythTimerType::AttributeList& GetRulePriorityList() const;
  virtual const MythTimerType::AttributeList& GetRuleDupMethodList() const;
  virtual const RuleExpirationMap& GetRuleExpirationMap() const;
  virtual const MythTimerType::AttributeList& GetRuleRecordingGroupList() const;
};
