/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <mythtypes.h>

class MythChannel
{
public:
  MythChannel();
  MythChannel(Myth::ChannelPtr channel);

  bool IsNull() const;
  Myth::ChannelPtr GetPtr() const;

  uint32_t ID() const;
  std::string Name() const;
  std::string Number() const;
  std::string Callsign() const;
  std::string Icon() const;
  bool Visible() const;
  bool IsRadio() const;
  uint32_t SourceID() const;
  uint32_t MultiplexID() const;
  unsigned NumberMajor() const { return m_numMajor; }
  unsigned NumberMinor() const { return m_numMinor; }

private:
  Myth::ChannelPtr m_channel;
  unsigned m_numMajor;
  unsigned m_numMinor;

  static void BreakNumber(const char *numstr, unsigned *major, unsigned *minor);
};
