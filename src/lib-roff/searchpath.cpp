/*@
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Copyright (C) 1989, 1990, 1991, 1992, 2000, 2001, 2003, 2005
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

#include "su/strsup.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "file_case.h"
#include "nonposix.h"
#include "searchpath.h"

#ifdef _WIN32
# include "relocate.h"
#else
# define relocate(path) su_strdup(path)
#endif

static file_case *  _try_iter(char const *dirs, char const *name,
                      uint32_t flags);

static file_case *
_try_iter(char const *dirs, char const *name, uint32_t flags)
{
  file_case *fcp;
  bool delname = ((flags & fcp->fc_take_path) != 0);
  flags = (flags & ~(fcp->fc_const_path)) | fcp->fc_take_path;
  unsigned namelen = strlen(name);
  char const *p = dirs;

  for (;;) {
    char *end = strchr(p, PATH_SEP_CHAR);
    if (end == NULL)
      end = strchr(p, '\0');
    int need_slash = (end > p && strchr(DIR_SEPS, end[-1]) == NULL);
    char *origpath = su_talloc(char, (end - p) + need_slash + namelen + 1);
    memcpy(origpath, p, end - p);
    if (need_slash)
      origpath[end - p] = '/';
    strcpy(origpath + (end - p) + need_slash, name);
#if 0
    fprintf(stderr, "origpath `%s'\n", origpath);
#endif
    char *path = relocate(origpath);
    su_free(origpath);
#if 0
    fprintf(stderr, "trying `%s'\n", path);
#endif
    if ((fcp = file_case::muxer(path, flags)) != NULL)
      goto jleave;
    if (errno != ENOENT)
      goto jleave;
    if (*end == '\0')
      break;
    p = end + 1;
  }
  errno = ENOENT;
jleave:
  if (delname)
    su_free(name);
  return fcp;
}

search_path::search_path(const char *envvar, const char *standard,
			 int add_home, int add_current)
{
  char *home = 0;
  if (add_home)
    home = getenv("HOME");
  char *e = 0;
  if (envvar)
    e = getenv(envvar);
  dirs = su_talloc(char, ((e && *e) ? strlen(e) + 1 : 0)
        + (add_current ? 1 + 1 : 0)
        + ((home && *home) ? strlen(home) + 1 : 0)
        + ((standard && *standard) ? strlen(standard) : 0)
        + 1);
  *dirs = '\0';
  if (e && *e) {
    su_stpcpy(su_stpcpy(dirs, e), PATH_SEP);
  }
  if (add_current) {
    su_stpcpy(su_stpcpy(dirs, "."), PATH_SEP);
  }
  if (home && *home) {
    su_stpcpy(su_stpcpy(dirs, home), PATH_SEP);
  }
  if (standard && *standard)
    strcat(dirs, standard);
  init_len = strlen(dirs);
}

search_path::~search_path()
{
  // dirs is always allocated
  su_free(dirs);
}

void search_path::command_line_dir(const char *s)
{
  char *old = dirs;
  unsigned old_len = strlen(old);
  unsigned slen = strlen(s);
  dirs = su_talloc(char, old_len + 1 + slen + 1);
  memcpy(dirs, old, old_len - init_len);
  char *p = dirs;
  p += old_len - init_len;
  if (init_len == 0)
    *p++ = PATH_SEP_CHAR;
  memcpy(p, s, slen);
  p += slen;
  if (init_len > 0) {
    *p++ = PATH_SEP_CHAR;
    memcpy(p, old + old_len - init_len, init_len);
    p += init_len;
  }
  *p++ = '\0';
  su_free(old);
}

file_case *search_path::open_file(char const *name, uint32_t flags)
{
  assert(name != NULL);

  file_case *fcp;
  if (IS_ABSOLUTE(name) || *dirs == '\0')
    fcp = file_case::muxer(name, flags);
  else
    fcp = _try_iter(dirs, name, flags);
  return fcp;
}

file_case *search_path::open_file_cautious(char const *name, uint32_t flags)
{
  file_case *fcp;
  if (name == NULL || strcmp(name, "-") == 0) {
    name = NULL;
    goto jmuxer;
  }
  if (IS_ABSOLUTE(name) || *dirs == '\0')
jmuxer:
    fcp = file_case::muxer(name, flags);
  else
    fcp = _try_iter(dirs, name, flags);
  return fcp;
}

// s-it2-mode
