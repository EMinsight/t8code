/*
  This file is part of t8code.
  t8code is a C library to manage a collection (a forest) of multiple
  connected adaptive space-trees of general element classes in parallel.

  Copyright (C) 2010 The University of Texas System
  Written by Carsten Burstedde, Lucas C. Wilcox, and Tobin Isaac

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

#include <t8_element.h>

struct t8_element
{
  int                 t8_element_dummy;
};

void
t8_scheme_destroy (t8_scheme_t * s)
{
  int                 t;

  T8_ASSERT (s != NULL);

  for (t = 0; t < T8_ECLASS_LAST; ++t) {
    if (s->eclass_schemes[t] != NULL) {
      t8_eclass_scheme_destroy (s->eclass_schemes[t]);
    }
  }
  T8_FREE (s);
}

void
t8_eclass_scheme_destroy (t8_eclass_scheme_t * ts)
{
  T8_ASSERT (ts != NULL);

  if (ts->ts_destroy != NULL) {
    ts->ts_destroy (ts);
  }
  T8_FREE (ts);
}

void
t8_eclass_boundary_alloc (t8_scheme_t * scheme, t8_eclass_t theclass,
                          int min_dim, int length, t8_element_t ** boundary)
{
  int                 t, offset, per;
#ifdef T8_ENABLE_DEBUG
  int                 per_eclass[T8_ECLASS_LAST];
#endif

  T8_ASSERT (length ==
             t8_eclass_count_boundary (theclass, min_dim, per_eclass));

  for (offset = t = 0; t < T8_ECLASS_LAST; ++t) {
    if (t8_eclass_to_dimension[t] >= min_dim) {
      per = t8_eclass_boundary_count[theclass][t];
      if (per > 0) {
        t8_element_new (scheme->eclass_schemes[t], per, boundary + offset);
        offset += per;
      }
    }
  }
  T8_ASSERT (offset == length);
}

void
t8_eclass_boundary_destroy (t8_scheme_t * scheme, t8_eclass_t theclass,
                            int min_dim, int length, t8_element_t ** boundary)
{
  int                 t, offset, per;
#ifdef T8_ENABLE_DEBUG
  int                 per_eclass[T8_ECLASS_LAST];
#endif

  T8_ASSERT (length ==
             t8_eclass_count_boundary (theclass, min_dim, per_eclass));

  for (offset = t = 0; t < T8_ECLASS_LAST; ++t) {
    if (t8_eclass_to_dimension[t] >= min_dim) {
      per = t8_eclass_boundary_count[theclass][t];
      if (per > 0) {
        t8_element_destroy (scheme->eclass_schemes[t], per,
                            boundary + offset);
        offset += per;
      }
    }
  }
  T8_ASSERT (offset == length);
}

void
t8_element_maxlevel (t8_eclass_scheme_t * ts)
{
  T8_ASSERT (ts != NULL && ts->elem_maxlevel != NULL);
  ts->elem_maxlevel ();
}

size_t
t8_element_size (t8_eclass_scheme_t * ts)
{
  T8_ASSERT (ts != NULL && ts->elem_size != NULL);
  return ts->elem_size ();
}

void
t8_element_parent (t8_eclass_scheme_t * ts,
                   const t8_element_t * elem, t8_element_t * parent)
{
  T8_ASSERT (ts != NULL && ts->elem_parent != NULL);
  ts->elem_parent (elem, parent);
}

void
t8_element_sibling (t8_eclass_scheme_t * ts,
                    const t8_element_t * elem, int sibid,
                    t8_element_t * sibling)
{
  T8_ASSERT (ts != NULL && ts->elem_sibling != NULL);
  ts->elem_sibling (elem, sibid, sibling);
}

void
t8_element_child (t8_eclass_scheme_t * ts, const t8_element_t * elem,
                  int childid, t8_element_t * child)
{
  T8_ASSERT (ts != NULL && ts->elem_child != NULL);
  ts->elem_child (elem, childid, child);
}

void
t8_element_nca (t8_eclass_scheme_t * ts, const t8_element_t * elem1,
                const t8_element_t * elem2, t8_element_t * nca)
{
  T8_ASSERT (ts != NULL && ts->elem_nca != NULL);
  ts->elem_nca (elem1, elem2, nca);
}

void
t8_element_boundary (t8_eclass_scheme_t * ts,
                     const t8_element_t * elem,
                     int min_dim, int length, t8_element_t ** boundary)
{
  T8_ASSERT (ts != NULL && ts->elem_boundary != NULL);
  ts->elem_boundary (elem, min_dim, length, boundary);
}

void
t8_element_new (t8_eclass_scheme_t * ts, int length, t8_element_t ** elems)
{
  T8_ASSERT (ts != NULL && ts->elem_new != NULL);
  ts->elem_new (ts->ts_context, length, elems);
}

void
t8_element_destroy (t8_eclass_scheme_t * ts, int length,
                    t8_element_t ** elems)
{
  T8_ASSERT (ts != NULL && ts->elem_destroy != NULL);
  ts->elem_destroy (ts->ts_context, length, elems);
}