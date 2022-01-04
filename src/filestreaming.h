/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2015 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/Filesystem.h>

#include <mythstream.h>

class ATTR_DLL_LOCAL FileStreaming : public Myth::Stream
{
public:
  FileStreaming(const std::string& filePath);
  virtual ~FileStreaming();

  bool IsValid() { return m_valid; }

  virtual int Read(void* buffer, unsigned n);
  virtual int64_t Seek(int64_t offset, Myth::WHENCE_t whence);
  virtual int64_t GetPosition() const { return m_pos; }
  virtual int64_t GetSize() const { return m_flen; }

private:
  bool m_valid;
  kodi::vfs::CFile m_file;
  int64_t m_flen;
  int64_t m_pos;

  bool _init(const char* filePath);
};
