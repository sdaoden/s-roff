/*@ Provide relocation for macro and font files. FIXME please
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 *  Copyright (C) 2005 - 2006 Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Library General Public License as published
 *  by the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _RELOCATE_H
#define _RELOCATE_H

#include "config.h"

#ifdef __cplusplus
extern char *curr_prefix;
extern size_t curr_prefix_len;

void set_current_prefix ();
char *xdirname (char *s);
char *searchpath (const char *name, const char *pathp);
#endif

/* This function has C linkage.  */
extern
#ifdef __cplusplus
"C"
#endif
char *relocatep (const char *path);

#ifdef __cplusplus
char *relocate (const char *path);
#endif

#endif // _RELOCATE_H
// s-it2-mode
