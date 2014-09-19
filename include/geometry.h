/*@
 * Copyright (c) 2014 Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
 *
 * Copyright (C) 2001 Free Software Foundation, Inc.
 *      Written by Gaius Mulley <gaius@glam.ac.uk>
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
#ifndef _GEOMETRY_H
#define _GEOMETRY_H

#include "config.h"

int adjust_arc_center(const int *, double *);
void check_output_arc_limits(int x, int y,
			     int xv1, int yv1,
			     int xv2, int yv2,
			     double c0, double c1,
			     int *minx, int *maxx,
			     int *miny, int *maxy);

#endif // _GEOMETRY_H
// s-it2-mode
