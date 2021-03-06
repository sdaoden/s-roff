/*@
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2000 - 2002, 2004
 *    Free Software Foundation, Inc.
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
#ifndef _TROFF_H
#define _TROFF_H

#include "config.h"
#include "troff-config.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "cset.h"
#include "cmap.h"
#include "color.h"
#include "device.h"
#include "errarg.h"
#include "error.h"
#include "lib.h"
#include "searchpath.h"

typedef int units;

extern units scale(units n, units x, units y); // scale n by x/y

extern units units_per_inch;

extern int ascii_output_flag;
extern int suppress_output_flag;
extern int color_flag;
extern int is_html;

extern int tcommand_flag;
extern int vresolution;
extern int hresolution;
extern int sizescale;

extern search_path *mac_path;

enum warning_type {
  WARN_CHAR = 01,
  WARN_NUMBER = 02,
  WARN_BREAK = 04,
  WARN_DELIM = 010,
  WARN_EL = 020,
  WARN_SCALE = 040,
  WARN_RANGE = 0100,
  WARN_SYNTAX = 0200,
  WARN_DI = 0400,
  WARN_MAC = 01000,
  WARN_REG = 02000,
  WARN_TAB = 04000,
  WARN_RIGHT_BRACE = 010000,
  WARN_MISSING = 020000,
  WARN_INPUT = 040000,
  WARN_ESCAPE = 0100000,
  WARN_SPACE = 0200000,
  WARN_FONT = 0400000,
  WARN_IG =  01000000,
  WARN_COLOR = 02000000,
  WARN_FILE = 1<<20,
  __WARN_MAX = WARN_FILE
};
enum {
  WARN_TOTAL = (__WARN_MAX << 1) - 1
}

int warning(warning_type, const char *,
	    const errarg & = empty_errarg,
	    const errarg & = empty_errarg,
	    const errarg & = empty_errarg);
int output_warning(warning_type, const char *,
		   const errarg & = empty_errarg,
		   const errarg & = empty_errarg,
		   const errarg & = empty_errarg);

#endif // _TROFF_H
// s-it2-mode
