/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include "settings.h"

#include <kodi/AddonBase.h>

class PVRClientMythTV;
class PVRClientLauncher;

class ATTR_DLL_LOCAL CPVRMythTVAddon : public kodi::addon::CAddonBase
{
public:
  CPVRMythTVAddon() = default;

  ADDON_STATUS SetSetting(const std::string& settingName,
                          const kodi::addon::CSettingValue& settingValue) override;
  ADDON_STATUS CreateInstance(const kodi::addon::IInstanceInfo& instance,
                              KODI_ADDON_INSTANCE_HDL& hdl) override;
  void DestroyInstance(const kodi::addon::IInstanceInfo& instance,
                       const KODI_ADDON_INSTANCE_HDL hdl) override;

private:
  CMythSettings m_settings;
  PVRClientMythTV* m_client = nullptr;
  PVRClientLauncher* m_launcher = nullptr;
};

#endif /* CLIENT_H */
