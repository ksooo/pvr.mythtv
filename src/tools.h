/*
 *  Copyright (C) 2013-2021 Jean-Luc Barrière
 *  Copyright (C) 2005-2014 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "private/os/os.h"

#include <math.h>

#ifdef __WINDOWS__
#include <time.h>
static inline struct tm *localtime_r(const time_t  *clock, struct tm *result)
{
  struct tm *data;
  if (!clock || !result)
    return NULL;
  data = localtime(clock);
  if (!data)
    return NULL;
  memcpy(result, data, sizeof (*result));
  return result;
}
#else
#include <time.h>
#endif

#define INTERVAL_MINUTE    60       ///< number of seconds in minute
#define INTERVAL_HOUR      3600     ///< number of seconds in hour
#define INTERVAL_DAY       86400    ///< number of seconds in day

static inline int daytime(time_t *time)
{
  struct tm dtm;
  localtime_r(time, &dtm);
  int retval = dtm.tm_sec + dtm.tm_min * INTERVAL_MINUTE + dtm.tm_hour * INTERVAL_HOUR;
  return retval;
}

static inline int weekday(time_t *time)
{
  struct tm dtm;
  localtime_r(time, &dtm);
  int retval = dtm.tm_wday;
  return retval;
}

static inline void timeadd(time_t *time, double diffsec)
{
  double dh = trunc(diffsec / INTERVAL_HOUR);
  struct tm newtm;
  localtime_r(time, &newtm);
  newtm.tm_hour += (int)dh;
  newtm.tm_sec += (int)(diffsec - dh * INTERVAL_HOUR);
  *time = mktime(&newtm);
}
