/*
 * Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992 Free Software Foundation, Inc.
 *      Written by James Clark (jjc@jclark.com)
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with groff; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin St - Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _CMAP_H
#define _CMAP_H

#include "config.h"

#include <limits.h>

enum cmap_builtin { CMAP_BUILTIN };

class cmap
{
  friend class cmap_init;

  unsigned char v[UCHAR_MAX +1];

public:
  cmap();
  cmap(cmap_builtin);
  int operator()(unsigned char) const;
  unsigned char &operator[](unsigned char);
};

inline int cmap::operator()(unsigned char c) const
{
  return v[c];
}

inline unsigned char &cmap::operator[](unsigned char c)
{
  return v[c];
}

extern cmap cmlower;
extern cmap cmupper;

static class cmap_init { /* FIXME oh my god.. injection initializers!! */
  static int initialised;
public:
  cmap_init();
} _cmap_init;

#endif // _CMAP_H
// s-it2-mode
