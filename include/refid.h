/*@
 * Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
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
#ifndef _REFID_H
#define _REFID_H

#include "config.h"

class reference_id
{
  int filename_id;
  int pos;

public:
  reference_id() : filename_id(-1) { }
  reference_id(int fid, int off) : filename_id(fid), pos(off) { }
  unsigned hash() const { return (filename_id << 4) + pos; }
  int is_null() const { return filename_id < 0; }
  friend inline int operator==(const reference_id &, const reference_id &);
};

inline int operator==(const reference_id &r1, const reference_id &r2)
{
  return r1.filename_id == r2.filename_id && r1.pos == r2.pos;
}

#endif // _REFID_H
// s-it2-mode
