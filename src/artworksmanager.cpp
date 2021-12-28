/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2018-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "artworksmanager.h"

#include "pvrclient-mythtv.h"
#include "private/os/os.h"

#define FILEOPS_CHANNEL_DUMMY_ICON    "channel.png"
#define FILEOPS_RECORDING_DUMMY_ICON  "recording.png"
#define CHANNEL_ICON_WIDTH            300

ArtworkManager::ArtworkManager(const std::string& server, unsigned wsapiport, const std::string& wsapiSecurityPin)
: m_wsapi(NULL)
{
  m_wsapi = new Myth::WSAPI(server, wsapiport, wsapiSecurityPin);
}

ArtworkManager::~ArtworkManager()
{
  delete m_wsapi;
}

std::string ArtworkManager::GetChannelIconPath(const MythChannel& channel)
{
  if (channel.IsNull() || channel.Icon().empty())
    return "";
  if (!CMythSettings::GetChannelIcons())
    return kodi::addon::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_CHANNEL_DUMMY_ICON;

  return m_wsapi->GetChannelIconUrl(channel.ID(), CHANNEL_ICON_WIDTH);
}

std::string ArtworkManager::GetPreviewIconPath(const MythProgramInfo& recording)
{
  if (recording.IsNull())
    return "";
  if (!CMythSettings::GetRecordingIcons())
    return kodi::addon::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_RECORDING_DUMMY_ICON;

  return m_wsapi->GetPreviewImageUrl(recording.ChannelID(), recording.RecordingStartTime());
}

std::string ArtworkManager::GetArtworkPath(const MythProgramInfo& recording, ArtworksType type)
{
  if (recording.IsNull())
    return "";
  if (!CMythSettings::GetRecordingIcons())
    switch (type)
    {
    case AWTypeCoverart:
      return kodi::addon::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_RECORDING_DUMMY_ICON;
    default:
      return "";
    }

  return m_wsapi->GetRecordingArtworkUrl(GetTypeNameByArtworksType(type), recording.Inetref(), recording.Season());
}
