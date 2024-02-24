/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <mythtypes.h>

class MythRecordingRule
{

public:

  MythRecordingRule();
  MythRecordingRule(Myth::RecordSchedulePtr recordschedule);

  Myth::RecordSchedulePtr GetPtr() { return m_recordSchedule; }
  MythRecordingRule DuplicateRecordingRule() const;

  uint32_t RecordID() const { return m_recordSchedule->recordId; }
  void SetRecordID(uint32_t recordid) { m_recordSchedule->recordId = recordid; }

  uint32_t ChannelID() const { return m_recordSchedule->chanId; }
  void SetChannelID(uint32_t channelid) { m_recordSchedule->chanId = channelid; }

  const std::string& Callsign() const { return m_recordSchedule->callSign; }
  void SetCallsign(const std::string& callsign) { m_recordSchedule->callSign = callsign; }

  time_t StartTime() const { return m_recordSchedule->startTime; }
  void SetStartTime(time_t starttime) { m_recordSchedule->startTime = starttime; }

  time_t EndTime() const { return m_recordSchedule->endTime; }
  void SetEndTime(time_t endtime) { m_recordSchedule->endTime = endtime; }

  const std::string& Title() const { return m_recordSchedule->title; }
  void SetTitle(const std::string& title) { m_recordSchedule->title = title; }

  const std::string& Subtitle() const { return m_recordSchedule->subtitle; }
  void SetSubtitle(const std::string& subtitle) { m_recordSchedule->subtitle = subtitle; }

  const std::string& Description() const { return m_recordSchedule->description; }
  void SetDescription(const std::string& description) { m_recordSchedule->description = description; }

  Myth::RT_t Type() const { return m_recordSchedule->type_t; }
  void SetType(Myth::RT_t type) { m_recordSchedule->type_t = type; }

  const std::string& Category() const { return m_recordSchedule->category; }
  void SetCategory(const std::string& category) { m_recordSchedule->category = category; }

  uint8_t StartOffset() const { return m_recordSchedule->startOffset; }
  void SetStartOffset(uint8_t startoffset) { m_recordSchedule->startOffset = startoffset; }

  uint8_t EndOffset() const { return m_recordSchedule->endOffset; }
  void SetEndOffset(uint8_t endoffset) { m_recordSchedule->endOffset = endoffset; }

  int8_t Priority() const { return m_recordSchedule->recPriority; }
  void SetPriority(int8_t priority) { m_recordSchedule->recPriority = priority; }

  bool Inactive() const { return m_recordSchedule->inactive; }
  void SetInactive(bool inactive) { m_recordSchedule->inactive = inactive; }

  Myth::ST_t SearchType() const { return m_recordSchedule->searchType_t; }
  void SetSearchType(Myth::ST_t searchtype)
  {
    m_recordSchedule->searchType_t = searchtype;
    m_recordSchedule->searchType.clear();
  }

  Myth::DM_t DuplicateControlMethod() const { return m_recordSchedule->dupMethod_t; }
  void SetDuplicateControlMethod(Myth::DM_t method)
  {
    m_recordSchedule->dupMethod_t = method;
    m_recordSchedule->dupMethod.clear();
  }

  Myth::DI_t CheckDuplicatesInType() const { return m_recordSchedule->dupIn_t; }
  void SetCheckDuplicatesInType(Myth::DI_t in)
  {
    m_recordSchedule->dupIn_t = in;
    m_recordSchedule->dupIn.clear();
  }

  const std::string& RecordingGroup() const { return m_recordSchedule->recGroup; }
  void SetRecordingGroup(const std::string& group) { m_recordSchedule->recGroup = group; }

  const std::string& StorageGroup() const { return m_recordSchedule->storageGroup; }
  void SetStorageGroup(const std::string& group) { m_recordSchedule->storageGroup = group; }

  const std::string& PlaybackGroup() const { return m_recordSchedule->playGroup; }
  void SetPlaybackGroup(const std::string& group) { m_recordSchedule->playGroup = group; }

  bool AutoTranscode() const { return m_recordSchedule->autoTranscode; }
  void SetAutoTranscode(bool enable) { m_recordSchedule->autoTranscode = enable; }

  bool UserJob(int jobnumber) const;
  void SetUserJob(int jobnumber, bool enable);

  bool AutoMetadata() const { return m_recordSchedule->autoMetaLookup; }
  void SetAutoMetadata(bool enable) { m_recordSchedule->autoMetaLookup = enable; }

  bool AutoCommFlag() const { return m_recordSchedule->autoCommflag; }
  void SetAutoCommFlag(bool enable) { m_recordSchedule->autoCommflag = enable; }

  bool AutoExpire() const { return m_recordSchedule->autoExpire; }
  void SetAutoExpire(bool enable) { m_recordSchedule->autoExpire = enable; }

  uint32_t MaxEpisodes() const { return m_recordSchedule->maxEpisodes; }
  void SetMaxEpisodes(uint32_t max) { m_recordSchedule->maxEpisodes = max; }

  bool NewExpiresOldRecord() const { return m_recordSchedule->maxNewest; }
  void SetNewExpiresOldRecord(bool enable) { m_recordSchedule->maxNewest = enable; }

  uint32_t Transcoder() const { return m_recordSchedule->transcoder; }
  void SetTranscoder(uint32_t transcoder) { m_recordSchedule->transcoder = transcoder; }

  uint32_t ParentID() const { return m_recordSchedule->parentId; }
  void SetParentID(uint32_t parentid) { m_recordSchedule->parentId = parentid; }

  uint32_t Filter() const { return m_recordSchedule->filter; }
  void SetFilter(uint32_t filter) { m_recordSchedule->filter = filter; }

  const std::string& ProgramID() const { return m_recordSchedule->programId; }
  void SetProgramID(const std::string& programid) { m_recordSchedule->programId = programid; }

  const std::string& SeriesID() const { return m_recordSchedule->seriesId; }
  void SetSeriesID(const std::string& seriesid) { m_recordSchedule->seriesId = seriesid; }

  const std::string& RecordingProfile() const { return m_recordSchedule->recProfile; }
  void SetRecordingProfile(const std::string& profile) { m_recordSchedule->recProfile = profile; }

  const std::string& InetRef() const { return m_recordSchedule->inetref; }
  void SetInerRef(const std::string& inetref) { m_recordSchedule->inetref = inetref; }

  uint16_t Season() const { return m_recordSchedule->season; }
  void SetSeason(uint16_t season) { m_recordSchedule->season = season; }

  uint16_t Episode() const { return m_recordSchedule->episode; }
  void SetEpisode(uint16_t episode) { m_recordSchedule->episode = episode; }

  time_t NextRecording() const { return m_recordSchedule->nextRecording; }
  time_t LastRecorded() const { return m_recordSchedule->lastRecorded; }
  time_t LastDeleted() const { return m_recordSchedule->lastDeleted; }
  uint32_t AverageDelay() const { return m_recordSchedule->averageDelay; }

private:
  Myth::RecordSchedulePtr m_recordSchedule;
};
