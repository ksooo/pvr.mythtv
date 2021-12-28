/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2012 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/AddonBase.h>

#include <string>
#include <map>

typedef std::multimap<int, std::string> CategoryByIdMap;
typedef std::map<std::string, int> CategoryByNameMap;

class ATTR_DLL_LOCAL Categories
{
public:
  Categories();

  std::string Category(int category) const;
  int Category(const std::string& category) const;

private:
  void LoadEITCategories(const char *filePath);

  CategoryByIdMap   m_categoriesById;
  CategoryByNameMap m_categoriesByName;
};
