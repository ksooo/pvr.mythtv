/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2020-2021 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "addon.h"
#include "pvrclient-launcher.h"
#include "pvrclient-mythtv.h"

ADDON_STATUS CPVRMythTVAddon::SetSetting(const std::string& settingName,
                                              const kodi::addon::CSettingValue& settingValue)
{
  if (m_client)
    return m_settings.SetSetting(*m_client, settingName, settingValue);
  return ADDON_STATUS_UNKNOWN;
}

ADDON_STATUS CPVRMythTVAddon::CreateInstance(const kodi::addon::IInstanceInfo& instance,
                                             KODI_ADDON_INSTANCE_HDL& hdl)
{
  if (instance.IsType(ADDON_INSTANCE_PVR))
  {
    kodi::Log(ADDON_LOG_INFO, "Creating MythTV PVR-Client");
    kodi::Log(ADDON_LOG_INFO, "Addon compiled with PVR API version %s", STR(ADDON_INSTANCE_VERSION_PVR));

    if (m_client)
    {
      kodi::Log(ADDON_LOG_ERROR, "Addon instance creation called where already present");
      return ADDON_STATUS_PERMANENT_FAILURE;
    }

    m_settings.Load();

    m_client = new PVRClientMythTV(instance);
    m_launcher = new PVRClientLauncher(m_client);

    if (m_launcher->Start())
    {
      hdl = m_client;
      kodi::Log(ADDON_LOG_INFO, "Addon started successfully");
      return ADDON_STATUS_OK;
    }

    kodi::Log(ADDON_LOG_ERROR, "Addon failed to start");

    delete m_client;
    m_client = nullptr;
    delete m_launcher;
    m_launcher = nullptr;

    return ADDON_STATUS_PERMANENT_FAILURE;
  }

  return ADDON_STATUS_UNKNOWN;
}

void CPVRMythTVAddon::DestroyInstance(const kodi::addon::IInstanceInfo& instance,
                                      const KODI_ADDON_INSTANCE_HDL hdl)
{
  if (instance.IsType(ADDON_INSTANCE_PVR))
  {
    delete m_launcher;
    m_launcher = nullptr;
    m_client = nullptr; // NOTE: delete of client done in background of instance handling
    kodi::Log(ADDON_LOG_INFO, "Addon destroyed.");
  }
}

ADDONCREATOR(CPVRMythTVAddon)
