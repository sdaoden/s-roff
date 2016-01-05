/*@
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989 - 1992, 2000, 2001, 2004
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

#include "config.h"
#include "lib.h"

#include <stdlib.h>

#ifdef HAVE_MMAP /* FIXME */
# include <sys/types.h>
# include <sys/mman.h>
#else
# include <errno.h>
#endif

#include "bib.h"

#ifdef HAVE_MMAP /* FIXME */
  /* The Net-2 man pages says that a MAP_FILE flag is required. */
# ifndef MAP_FILE
#  define MAP_FILE 0
# endif

char *mapread(int fd, int nbytes)
{
  char *p = (char *)mmap((void *)0, (size_t)nbytes, PROT_READ,
			 MAP_FILE|MAP_PRIVATE, fd, (off_t)0);
  if (p == (char *)-1)
    return 0;
  /* mmap() shouldn't return 0 since MAP_FIXED wasn't specified. */
  if (p == 0)
    abort();
  return p;
}

int unmap(char *p, int len)
{
  return munmap((void *)p, len);
}

#else // HAVE_MMAP
char *mapread(int fd, int nbytes)
{
  errno = ENODEV;
  return 0;
}

int unmap(char *p, int len)
{
  errno = EINVAL;
  return -1;
}
#endif // HAVE_MMAP

// s-it2-mode
