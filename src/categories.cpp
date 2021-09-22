/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2012 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "categories.h"
#include "private/os/os.h"

#include <kodi/Filesystem.h>
#include <regex>

#define CATEGORIES_FILENAME       "eit_categories.txt"

Categories::Categories()
: m_categoriesById()
{
  std::string filePath;
  filePath = kodi::GetAddonPath(std::string("resources") + PATH_SEPARATOR_STRING + CATEGORIES_FILENAME);
  LoadEITCategories(filePath.c_str());
  filePath = kodi::GetBaseUserPath(CATEGORIES_FILENAME);
  LoadEITCategories(filePath.c_str());
  // Copy over
  CategoryByIdMap::const_iterator it;
  for (it = m_categoriesById.begin(); it != m_categoriesById.end(); ++it)
  {
    m_categoriesByName[it->second] = it->first;
  }
}

std::string Categories::Category(int category) const
{
  CategoryByIdMap::const_iterator it = m_categoriesById.find(category);
  if (it != m_categoriesById.end())
    return it->second;
  return "";
}

int Categories::Category(const std::string& category) const
{
  CategoryByNameMap::const_iterator it = m_categoriesByName.find(category);
  if (it != m_categoriesByName.end())
    return it->second;
  return 0;
}

void Categories::LoadEITCategories(const char *filePath)
{
  if (kodi::vfs::FileExists(filePath, false))
  {
    kodi::Log(ADDON_LOG_DEBUG, "%s: Loading EIT categories from file '%s'", __FUNCTION__, filePath);

    kodi::vfs::CFile file;
    if (!file.OpenFile(filePath, 0))
    {
      kodi::Log(ADDON_LOG_ERROR, "%s: File '%s' failed to open", __FUNCTION__, filePath);
      return;
    }

    std::string line;
    std::regex rgx("^ *(0x.*)*; *\"(.*)\"");
    while (file.ReadLine(line))
    {
      std::smatch matches;
      if (std::regex_search(line, matches, rgx) && matches.size() == 3)
      {
        int catId = std::stoi(matches[1].str(), nullptr, 16);
        std::string name = matches[2].str();

        m_categoriesById.insert(std::pair<int, std::string>(catId, name));
        kodi::Log(ADDON_LOG_DEBUG, "%s: Add name [%s] for category %.2X", __FUNCTION__, name.c_str(), catId);
      }
    }
  }
  else
  {
    kodi::Log(ADDON_LOG_INFO, "%s: File '%s' not found", __FUNCTION__, filePath);
  }
}
