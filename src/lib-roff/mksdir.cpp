/*@ FIXME consider to replace the temporary name creation with homebrew
 * Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 2001 Free Software Foundation, Inc.
 *      Written by Werner Lemberg (wl@gnu.org)
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

/* This file is heavily based on the file mkstemp.c which is part of the
   fileutils package. */

extern int gen_tempname(char *, int = 0); // FIXME

/* Generate a unique temporary directory name from TEMPLATE.
   The last six characters of TEMPLATE must be "XXXXXX";
   they are replaced with a string that makes the filename unique.
   Then open the directory and return a fd. */
int mksdir(char *tmpl)
{
  return gen_tempname(tmpl, 1);
}

// s-it2-mode
