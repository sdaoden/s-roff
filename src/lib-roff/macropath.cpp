/*@
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2000 Free Software Foundation, Inc.
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

#include "config.h"
#include "lib.h"

#include "defs.h"
#include "macropath.h"
#include "searchpath.h"

#define MACROPATH_ENVVAR U_ROFF_TMAC_PATH /* FIXME */

search_path macro_path(MACROPATH_ENVVAR, MACROPATH, 1, 1);
search_path safer_macro_path(MACROPATH_ENVVAR, MACROPATH, 1, 0);
search_path config_macro_path(MACROPATH_ENVVAR, MACROPATH, 0, 0);

// s-it2-mode
