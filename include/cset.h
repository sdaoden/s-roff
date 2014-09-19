/*@
 * Copyright (c) 2014 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
 *
 * Copyright (C) 1989 - 1992 Free Software Foundation, Inc.
 *   Written by James Clark (jjc@jclark.com)
 *
 * groff is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * groff is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with groff; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin St - Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _CSET_H
#define _CSET_H

#include "config.h"

#include <limits.h>

enum cset_builtin { CSET_BUILTIN };

class cset
{
  friend class cset_init;

  char v[UCHAR_MAX +1];
  void clear();

public:
  cset();
  cset(cset_builtin);
  cset(const char *);
  cset(const unsigned char *);
  int operator()(unsigned char) const;

  cset &operator|=(const cset &);
  cset &operator|=(unsigned char);
};

inline int cset::operator()(unsigned char c) const
{
  return v[c];
}

inline cset &cset::operator|=(unsigned char c)
{
  v[c] = 1;
  return *this;
}

extern cset csalpha;
extern cset csupper;
extern cset cslower;
extern cset csdigit;
extern cset csxdigit;
extern cset csspace;
extern cset cspunct;
extern cset csalnum;
extern cset csprint;
extern cset csgraph;
extern cset cscntrl;

static class cset_init { /* FIXME oh my god.. injection initializers!! */
  static int initialised;
public:
  cset_init();
} _cset_init;

#endif // _CSET_H
// s-it2-mode
