/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2018-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "cppmyth/MythChannel.h"
#include "cppmyth/MythProgramInfo.h"

#include <kodi/AddonBase.h>
#include <mythwsapi.h>

#include <string>

class PVRClientMythTV;

class ATTRIBUTE_HIDDEN ArtworkManager
{
public:
  enum ArtworksType
  {
    AWTypeThumbnail,
    AWTypeCoverart,
    AWTypeFanart,
    AWTypeChannelIcon,
    AWTypeBanner,
    AWTypeScreenshot,
    AWTypePoster,
    AWTypeBackcover,
    AWTypeInsidecover,
    AWTypeCDImage
  };

  static std::vector<ArtworksType> GetArtworksTypes()
  {
    std::vector<ArtworksType> ret;
    ret.push_back(AWTypeChannelIcon);
    ret.push_back(AWTypeThumbnail);
    ret.push_back(AWTypeCoverart);
    ret.push_back(AWTypeFanart);
    ret.push_back(AWTypeBanner);
    ret.push_back(AWTypeScreenshot);
    ret.push_back(AWTypePoster);
    ret.push_back(AWTypeBackcover);
    ret.push_back(AWTypeInsidecover);
    ret.push_back(AWTypeCDImage);
    return ret;
  }

  static const char *GetTypeNameByArtworksType(ArtworksType type)
  {
    switch(type)
    {
    case AWTypeChannelIcon: return "channelIcon";
    case AWTypeThumbnail: return "thumbnail";
    case AWTypeCoverart: return "coverart";
    case AWTypeFanart: return "fanart";
    case AWTypeBanner: return "banner";
    case AWTypeScreenshot: return "screenshot";
    case AWTypePoster: return "poster";
    case AWTypeBackcover: return "backcover";
    case AWTypeInsidecover: return "insidecover";
    case AWTypeCDImage: return "cdimage";
    default: return "";
    }
  }

  ArtworkManager(const std::string& server, unsigned wsapiport, const std::string& wsapiSecurityPin);
  virtual ~ArtworkManager();

  std::string GetChannelIconPath(const MythChannel& channel);
  std::string GetPreviewIconPath(const MythProgramInfo& recording);
  std::string GetArtworkPath(const MythProgramInfo& recording, ArtworksType type);

  Myth::WSAPI *m_wsapi;
};
