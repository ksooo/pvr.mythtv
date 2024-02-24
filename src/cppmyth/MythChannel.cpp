/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "MythChannel.h"

#include <cctype>
#include <cstring>
#include <cstdlib>

#define CHANNUM_STR_SIZE  32

MythChannel::MythChannel()
: m_channel()
, m_numMajor(0)
, m_numMinor(0)
{
}

MythChannel::MythChannel(const Myth::ChannelPtr& channel)
: m_channel(channel)
, m_numMajor(0)
, m_numMinor(0)
{
  if (m_channel && m_channel->chanNum.size() < CHANNUM_STR_SIZE)
  {
    BreakNumber(m_channel->chanNum.c_str(), &m_numMajor, &m_numMinor);
  }
}

void MythChannel::BreakNumber(const char *numstr, unsigned *major, unsigned *minor)
{
  // ATSC channel number use "_" or "." for major and minor separator
  const char *pn = numstr;
  char str[CHANNUM_STR_SIZE];
  char *ps = str;
  while (isspace(*pn))
    ++pn;
  while (isdigit(*pn))
    *ps++ = *pn++;
  *ps = '\0';
  *major = (unsigned)atoi(str);
  ps = str;
  ++pn;
  while (isdigit(*pn))
    *ps++ = *pn++;
  *ps = '\0';
  *minor = (unsigned)atoi(str);
}

bool MythChannel::IsNull() const
{
  if (!m_channel)
    return true;
  return m_channel.get() == NULL;
}

bool MythChannel::IsRadio() const
{
  // Check for keyword to move channel to "Radio" group in the channel's callsign
  // If "[RADIO]" is found then return true else return false
  return (m_channel ? ((m_channel->callSign.find("[RADIO]") != std::string::npos) ? true : false) : false);
}
