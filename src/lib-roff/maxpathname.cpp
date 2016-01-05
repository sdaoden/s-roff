/*@ path_name_max(dir) does the same as pathconf(dir, _PC_PATH_MAX).
 *
 * Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 2005 Free Software Foundation, Inc.
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

#include "config.h"
#include "lib.h"

#include <sys/types.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef _POSIX_VERSION
size_t path_name_max()
{
  return pathconf("/", _PC_PATH_MAX) < 1 ? 1024 : pathconf("/",_PC_PATH_MAX);
}

#else
# include <stdlib.h>

# ifdef HAVE_DIRENT_H
#  include <dirent.h>
# else
#  ifdef HAVE_SYS_DIR_H
#   include <sys/dir.h>
#  endif
# endif

# ifndef PATH_MAX
#  ifdef MAXPATHLEN
#   define PATH_MAX MAXPATHLEN
#  else
#   ifdef MAX_PATH
#    define PATH_MAX MAX_PATH
#   else
#    ifdef _MAX_PATH
#     define PATH_MAX _MAX_PATH
#    else
#     define PATH_MAX 255
#    endif
#   endif
#  endif
# endif

size_t path_name_max()
{
  return PATH_MAX;
}
#endif /* _POSIX_VERSION */

// s-it2-mode
