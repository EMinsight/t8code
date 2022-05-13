/*
  This file is part of t8code.
  t8code is a C library to manage a collection (a forest) of multiple
  connected adaptive space-trees of general element classes in parallel.

  Copyright (C) 2015 the developers

  t8code is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  t8code is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with t8code; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <t8.h>

static int          t8_package_id = -1;

int
t8_get_package_id (void)
{
  return t8_package_id;
}

void
t8_logv (int category, int priority, const char *fmt, va_list ap)
{
  sc_logv ("unknown", -1, t8_package_id, category, priority, fmt, ap);
}

void
t8_logf (int category, int priority, const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  sc_logv ("unknown", -1, t8_package_id, category, priority, fmt, ap);
  va_end (ap);
}

void
t8_log_indent_push (void)
{
  sc_log_indent_push_count (t8_get_package_id (), 1);
}

void
t8_log_indent_pop (void)
{
  sc_log_indent_pop_count (t8_get_package_id (), 1);
}

void
t8_global_errorf (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_GLOBAL, SC_LP_ERROR, fmt, ap);
  va_end (ap);
}

void
t8_global_essentialf (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_GLOBAL, SC_LP_ESSENTIAL, fmt, ap);
  va_end (ap);
}

void
t8_global_productionf (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_GLOBAL, SC_LP_PRODUCTION, fmt, ap);
  va_end (ap);
}

void
t8_global_infof (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_GLOBAL, SC_LP_INFO, fmt, ap);
  va_end (ap);
}

void
t8_infof (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_NORMAL, SC_LP_INFO, fmt, ap);
  va_end (ap);
}

void
t8_productionf (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_NORMAL, SC_LP_PRODUCTION, fmt, ap);
  va_end (ap);
}

void
t8_debugf (const char *fmt, ...)
{
#ifdef T8_ENABLE_DEBUG
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_NORMAL, SC_LP_DEBUG, fmt, ap);
  va_end (ap);
#endif
}

void
t8_errorf (const char *fmt, ...)
{
  va_list             ap;

  va_start (ap, fmt);
  t8_logv (SC_LC_NORMAL, SC_LP_ERROR, fmt, ap);
  va_end (ap);
}

void
t8_init (int log_threshold)
{
  int                 w;

  t8_package_id = sc_package_register (NULL, log_threshold,
                                       "t8", "Adaptive discretizations");

  w = 24;
  t8_global_essentialf ("This is %s\n", t8_get_version_string ());
  t8_global_productionf ("%-*s %s\n", w, "CPP", T8_CPP);
  t8_global_productionf ("%-*s %s\n", w, "CPPFLAGS", T8_CPPFLAGS);
  t8_global_productionf ("%-*s %s\n", w, "CC", T8_CC);
  t8_global_productionf ("%-*s %s\n", w, "CFLAGS", T8_CFLAGS);
  t8_global_productionf ("%-*s %s\n", w, "LDFLAGS", T8_LDFLAGS);
  t8_global_productionf ("%-*s %s\n", w, "LIBS", T8_LIBS);
}

void               *
t8_sc_array_index_topidx (sc_array_t *array, t8_topidx_t it)
{
  P4EST_ASSERT (it >= 0 && (size_t) it < array->elem_count);

  return array->array + array->elem_size * (size_t) it;
}

void               *
t8_sc_array_index_locidx (sc_array_t *array, t8_locidx_t it)
{
  P4EST_ASSERT (it >= 0 && (size_t) it < array->elem_count);

  return array->array + array->elem_size * (size_t) it;
}

const char         *
t8_get_version_string ()
{
  return T8_PACKAGE_STRING;
}

const char         *
t8_get_version_point_string ()
{
  return T8_VERSION_POINT;
}

const char         *
t8_get_version_number ()
{
  return T8_VERSION;
}

int
t8_get_version_major ()
{
  return T8_VERSION_MAJOR;
}

int
t8_get_version_minor ()
{
  return T8_VERSION_MINOR;
}

int
t8_get_version_patch ()
{
  const char         *version_point = t8_get_version_point_string ();

  if (version_point == NULL || strlen (version_point) == 0) {
    /* The patch version is invalid. */
    t8_global_errorf ("ERROR: Point version string is NULL or empty.\n");
    /* We abort in debugging mode and continue with value -1 otherwise. */
    T8_ASSERT (version_point != NULL && strlen (version_point) > 1);
    return -1;
  }
  /* We expect the string to look like "X-HASH" or "X", 
   * with X an integer and HASH a string. */
  char               *error_check_string;
  const long          patch_number_long =
    strtol (version_point, &error_check_string, 10);
  /* strotl returns to error_check_string the first invalid character of versiont_point,
   * thus the first character that is not a number. */
  /* The first invalid character must not be the first character of version_point */

  if (*error_check_string != version_point[0]) {
    /* The string does not start with a number */
    t8_global_errorf
      ("ERROR: Point version string does not begin with patch number.\n");
    /* Abort in debugging mode, continue with -1 else. */
    T8_ASSERT (*error_check_string != version_point[0]);
    return -1;
  }

  /* Convert to integer */
  const int           patch_number = patch_number_long;
  /* Double check conversion */
  T8_ASSERT (patch_number == patch_number_long);
  if (patch_number < 0) {
    t8_global_errorf ("ERROR: Patch number %i is not >=0\n", patch_number);
    /* Abort in debugging mode, continue with negative number else. */
    T8_ASSERT (patch_number >= 0);
  }

  /* Return the patch number */
  return patch_number;
}
