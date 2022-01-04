/*
 *  Copyright (C) 2017-2021 Jean-Luc Barrière
 *  Copyright (C) 2017 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "pvrclient-mythtv.h"

#define PVRCLIENT_LAUNCHER_RETRY    30  /* Wait for 30 seconds before retry */

class PVRClientLauncherPrivate;

class ATTR_DLL_LOCAL PVRClientLauncher
{
public:
  PVRClientLauncher(PVRClientMythTV* client);
  ~PVRClientLauncher();

  bool Start();
  bool WaitForCompletion(unsigned timeout);

private:
  PVRClientLauncherPrivate *m_p;
};
