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
  MythChannel(const Myth::ChannelPtr& channel);

  bool IsNull() const;
  Myth::ChannelPtr GetPtr() const { return m_channel; }

  uint32_t ID() const { return m_channel->chanId; }
  const std::string& Name() const { return m_channel->channelName; }
  const std::string& Number() const { return m_channel->chanNum; }
  const std::string& Callsign() const { return m_channel->callSign; }
  const std::string& Icon() const { return m_channel->iconURL; }
  bool Visible() const { return m_channel->visible; }
  uint32_t SourceID() const { return m_channel->sourceId; }
  uint32_t MultiplexID() const { return m_channel->mplexId; };
  unsigned NumberMajor() const { return m_numMajor; }
  unsigned NumberMinor() const { return m_numMinor; }
  bool IsRadio() const;

private:
  Myth::ChannelPtr m_channel;
  unsigned m_numMajor;
  unsigned m_numMinor;

  static void BreakNumber(const char *numstr, unsigned *major, unsigned *minor);
};
