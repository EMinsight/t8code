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

#include "t8_default_common_cxx.hxx"
#include "t8_default_prism_cxx.hxx"
#include "t8_dprism_bits.h"
#include "t8_dprism.h"

typedef t8_dprism_t t8_default_prism_t;

int
t8_default_scheme_prism_c::t8_element_maxlevel (void)
{
  return T8_DPRISM_MAXLEVEL;
}

int
t8_default_scheme_prism_c::t8_element_level (const t8_element_t * elem)
{
  return t8_dprism_get_level ((const t8_dprism_t *) elem);
}

void
t8_default_scheme_prism_c::t8_element_copy (const t8_element_t * source,
                                            t8_element_t * dest)
{
  t8_dprism_copy ((const t8_dprism_t *) source, (t8_dprism_t *) dest);
}

int
t8_default_scheme_prism_c::t8_element_num_children (const t8_element_t * elem)
{
    return T8_DPRISM_CHILDREN;
}

void
t8_default_scheme_prism_c::t8_element_set_linear_id (t8_element_t * elem,
                                                     int level, uint64_t id)
{
  T8_ASSERT (0 <= level && level <= T8_DPRISM_MAXLEVEL);
  T8_ASSERT (0 <= id && id < ((u_int64_t) 1) << 3 * level);

  t8_dprism_init_linear_id ((t8_default_prism_t *) elem, level, id);
}

void
t8_default_scheme_prism_c::t8_element_successor (const t8_element_t * t,
                                                 t8_element_t * s, int level)
{
  T8_ASSERT (1 <= level && level <= T8_DPRISM_MAXLEVEL);

  t8_dprism_successor ((const t8_default_prism_t *) t,
                       (t8_default_prism_t *) s, level);
}

void
t8_default_scheme_prism_c::t8_element_first_descendant (const t8_element_t *
                                                        elem,
                                                        t8_element_t * desc)
{
  t8_dprism_first_descendant ((const t8_default_prism_t *) elem,
                              (t8_default_prism_t *) desc,
                              T8_DPRISM_MAXLEVEL);
}

void
t8_default_scheme_prism_c::t8_element_last_descendant (const t8_element_t *
                                                       elem,
                                                       t8_element_t * desc)
{
  t8_dprism_last_descendant ((const t8_default_prism_t *) elem,
                             (t8_default_prism_t *) desc, T8_DPRISM_MAXLEVEL);
}

int
t8_default_scheme_prism_c::t8_element_root_len (const t8_element_t * elem)
{
  return T8_DPRISM_ROOT_LEN;
}

void
t8_default_scheme_prism_c::t8_element_vertex_coords (const t8_element_t * t,
                                                     int vertex, int coords[])
{
  t8_dprism_vertex_coords ((const t8_dprism_t *) t, vertex, coords);
}

u_int64_t
  t8_default_scheme_prism_c::t8_element_get_linear_id (const t8_element_t *
                                                       elem, int level)
{
  return t8_dprism_linear_id ((const t8_dprism_t *) elem, level);
}

/* Constructor */
t8_default_scheme_prism_c::t8_default_scheme_prism_c (void)
{
  eclass = T8_ECLASS_PRISM;
  element_size = sizeof (t8_default_prism_t);
  ts_context = sc_mempool_new (sizeof (element_size));
}

t8_default_scheme_prism_c::~t8_default_scheme_prism_c ()
{
  /* This destructor is empty since the destructor of the
   * default_common scheme is called automatically and it
   * suffices to destroy the quad_scheme.
   * However we need to provide an implementation of the destructor
   * and hence this empty function. */
}
