/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <mythtypes.h>

class MythProgramInfo;
typedef std::map<std::string, MythProgramInfo> ProgramInfoMap;

class MythProgramInfo
{
public:

  MythProgramInfo();
  MythProgramInfo(const Myth::ProgramPtr& proginfo);

  bool IsNull() const;
  Myth::ProgramPtr GetPtr() const { return m_proginfo; }
  bool operator ==(const MythProgramInfo &other);
  bool operator !=(const MythProgramInfo &other);

  // Program fields
  const std::string& ProgramID() const { return m_proginfo->programId; }
  const std::string& SerieID() const { return m_proginfo->seriesId; }
  const std::string& Title() const { return m_proginfo->title; }
  const std::string& Subtitle() const { return m_proginfo->subTitle; }
  const std::string& HostName() const { return m_proginfo->hostName; }
  const std::string& FileName() const { return m_proginfo->fileName; }
  const std::string& Description() const { return m_proginfo->description; }
  int Duration() const;
  const std::string& Category() const { return m_proginfo->category; }
  time_t StartTime() const { return m_proginfo->startTime; }
  time_t EndTime() const { return m_proginfo->endTime; }
  bool IsWatched() const;
  bool IsDeletePending() const;
  bool HasBookmark() const;
  uint32_t ChannelID() const { return m_proginfo->channel.chanId; }
  const std::string& ChannelName() const { return m_proginfo->channel.channelName; }
  const std::string& Callsign() const { return m_proginfo->channel.callSign; }
  Myth::RS_t Status() const;
  const std::string& RecordingGroup() const { return m_proginfo->recording.recGroup; }
  uint32_t RecordID() const { return m_proginfo->recording.recordId; }
  time_t RecordingStartTime() const { return m_proginfo->recording.startTs; }
  time_t RecordingEndTime() const { return m_proginfo->recording.endTs; }
  int Priority() const { return m_proginfo->recording.priority; }
  const std::string& StorageGroup() const { return m_proginfo->recording.storageGroup; }
  const std::string& Inetref() const { return m_proginfo->inetref; }
  uint16_t Season() const { return m_proginfo->season; }
  uint16_t Episode() const { return m_proginfo->episode; }
  const std::string& Airdate() const { return m_proginfo->airdate; }
  int64_t FileSize() const { return m_proginfo->fileSize; }
  bool IsDamaged(uint32_t schemaVersion) const;

  /// Reset cache
  void ResetCache() {  m_cache.reset(new Cache()); }
  /// Copy cache from other
  void CopyCache(const MythProgramInfo &other) { m_cache = other.m_cache; }
  /// Reset cached flags
  void ResetFlags() { m_cache->ResetFlags(); }

  // About cache
  bool IsVisible() const;
  bool IsDeleted() const;
  bool IsLiveTV() const;
  bool HasCoverart() const;
  bool HasFanart() const;
  const std::string& UID() const { return m_cache->GetUID(*this); }
  const std::string& GroupingTitle() const { return m_cache->GetGroupingTitle(*this); }
  float VideoFrameRate() const { return m_cache->GetVideoFrameRate(); }
  float VideoAspec() const { return m_cache->GetVideoAspec(); }
  bool Serie() const { return m_cache->GetSerie(); }
  int Bookmark() const { return m_cache->GetBookmark(); }

  void SetVideoFrameRate(float fps) { m_cache->SetVideoFrameRate(fps); }
  void SetVideoAspec(float aspec) { m_cache->SetVideoAspec(aspec); }
  void SetSerie(bool flag) { m_cache->SetSerie(flag); }
  void SetBookmark(int seconds) { m_cache->SetBookmark(seconds); }

private:
  Myth::ProgramPtr m_proginfo;

  friend class Cache;

  class Cache
  {
  public:
    Cache()
    : m_flags(0)
    , m_UID()
    , m_groupingTitle()
    , m_videoFrameRate(0)
    , m_videoAspec(0)
    , m_serie(false)
    , m_bookmark(0)
    {}
    ~Cache() {}
    void ResetFlags() { m_flags = 0; }
    int32_t GetFlags(const MythProgramInfo& prog) const { return (m_flags == 0 ? get_flags(prog) : m_flags); }
    const std::string& GetUID(const MythProgramInfo& prog) const { return (m_UID.empty() ? get_uid(prog) : m_UID); }
    const std::string& GetGroupingTitle(const MythProgramInfo& prog) const { return (m_groupingTitle.empty() ? get_grouping_title(prog) : m_groupingTitle); }

    float GetVideoFrameRate() const { return m_videoFrameRate; }
    float GetVideoAspec() const { return m_videoAspec; }
    bool GetSerie() const { return m_serie; }
    int GetBookmark() const { return m_bookmark; }
    void SetVideoFrameRate(float fps) { m_videoFrameRate = fps; }
    void SetVideoAspec(float aspec) { m_videoAspec = aspec; }
    void SetSerie(bool flag) { m_serie = flag; }
    void SetBookmark(int seconds) { m_bookmark = seconds; }

  private:
    mutable int32_t m_flags;
    mutable std::string m_UID;
    mutable std::string m_groupingTitle;

    int32_t get_flags(const MythProgramInfo& prog) const;
    const std::string& get_uid(const MythProgramInfo& prog) const;
    const std::string& get_grouping_title(const MythProgramInfo& prog) const;

    float m_videoFrameRate;
    float m_videoAspec;
    bool m_serie;               ///< true if program is serie else false
    int m_bookmark;             ///< last played position in seconds
  };

  MYTH_SHARED_PTR<Cache> m_cache;
};
