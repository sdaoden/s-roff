/*@
 * Copyright (c) 2014 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
 *
 * Copyright (C) 2002, 2003, 2006
 *    Free Software Foundation, Inc.
 *      Written by Werner Lemberg <wl@gnu.org>
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
#ifndef _UNICODE_H
#define _UNICODE_H

#include "config.h"

// Convert a groff glyph name to a string containing an underscore-separated
// list of Unicode code points.  For example,
//
//   `-'   ->  `2010'
//   `,c'  ->  `00E7'
//   `fl'  ->  `0066_006C'
//
// Return NULL if there is no equivalent.
const char *glyph_name_to_unicode(const char *);

// Convert a string containing an underscore-separated list of Unicode code
// points to a groff glyph name.  For example,
//
//   `2010'       ->  `hy'
//   `0066_006C'  ->  `fl'
//
// Return NULL if there is no equivalent.
const char *unicode_to_glyph_name(const char *);

// Convert a string containing a precomposed Unicode character to a string
// containing an underscore-separated list of Unicode code points,
// representing its canonical decomposition.  Also perform compatibility
// equivalent replacement.  For example,
//
//   `1F3A' -> `0399_0313_0300'
//   `FA6A' -> `983B'
//
// Return NULL if there is no equivalent.
const char *decompose_unicode(const char *);

// Test whether the given string denotes a Unicode character.  It must
// be of the form `uNNNN', obeying the following rules.
//
//   - `NNNN' must consist of at least 4 hexadecimal digits in upper case.
//   - If there are more than 4 hexadecimal digits, the leading one must not
//     be zero,
//   - `NNNN' must denote a valid Unicode code point (U+0000..U+10FFFF,
//     excluding surrogate code points.
//
// Return a pointer to `NNNN' (skipping the leading `u' character) in case
// of success, NULL otherwise.
const char *check_unicode_name(const char *);

#endif // _UNICODE_H
// s-it2-mode
