/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "filestreaming.h"

#define MAX_READ_SIZE  131072

FileStreaming::FileStreaming(const std::string& filePath)
: m_valid(false)
, m_flen(0)
, m_pos(0)
{
  m_valid = _init(filePath.c_str());
}

FileStreaming::~FileStreaming()
{
}

int FileStreaming::Read(void* buffer, unsigned n)
{
  if (!m_valid)
    return -1;

  char* b = (char*)buffer;
  bool eof = false;
  n = (n > MAX_READ_SIZE ? MAX_READ_SIZE : n);
  unsigned r = n;
  do
  {
    size_t s = m_file.Read(b, r);
    if (s > 0)
    {
      r -= s;
      b += s;
      m_pos += s;
      eof = false;
    }
    else
    {
      if (eof)
        break;
      eof = true;
      m_file.Seek(0, 0);
    }
  } while (r > 0 || eof);
  if (eof)
    kodi::Log(ADDON_LOG_DEBUG, "%s: EOF", __FUNCTION__);
  return (int)(n -r);
}

int64_t FileStreaming::Seek(int64_t offset, Myth::WHENCE_t whence)
{
  switch (whence)
  {
  case Myth::WHENCE_SET:
    if (offset <= GetSize() && offset >= 0)
      return (m_pos = m_file.Seek(offset, 0));
    break;
  case Myth::WHENCE_CUR:
    if ((m_pos + offset) <= GetSize() && m_pos + offset >= 0)
      return (m_pos = m_file.Seek(m_pos + offset, 0));
    break;
  case Myth::WHENCE_END:
    if (offset >= 0 && (GetSize() - offset) >= 0)
      return (m_pos = m_file.Seek(GetSize() - offset, 0));
    break;
  default:
    break;
  }
  return -1;
}

bool FileStreaming::_init(const char* filePath)
{
  if (m_file.OpenFile(filePath, 0))
  {
    m_flen = m_file.GetLength();
    return true;
  }
  kodi::Log(ADDON_LOG_DEBUG, "%s: cannot open file '%s'", __FUNCTION__, filePath);
  return false;
}
