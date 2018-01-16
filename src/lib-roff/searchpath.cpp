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

#include "su/cs.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#include "file_case.h"
#include "nonposix.h"
#include "searchpath.h"

#ifdef _WIN32
# include "relocate.h"
#else
# define relocate(path) su_cs_dup(path)
#endif

static file_case *  _try_iter(char const *dirs, char const *name,
                      uint32_t flags);

static file_case *
_try_iter(char const *dirs, char const *name, uint32_t flags)
{
  file_case *fcp;
  bool delname = ((flags & fcp->fc_take_path) != 0);
  flags = (flags & ~(fcp->fc_const_path)) | fcp->fc_take_path;
  unsigned namelen = su_cs_len(name);
  char const *p = dirs;

  for (;;) {
    char *end = su_cs_find_c(p, PATH_SEP_CHAR);
    if (end == NULL)
      end = su_cs_find_c(p, '\0');
    int need_slash = (end > p && su_cs_find_c(DIR_SEPS, end[-1]) == NULL);
    char *origpath = su_TALLOC(char, (end - p) + need_slash + namelen +1);
    su_mem_copy(origpath, p, end - p);
    if (need_slash)
      origpath[end - p] = '/';
    su_mem_copy(origpath + (end - p) + need_slash, name, namelen);
#if 0
    fprintf(stderr, "origpath `%s'\n", origpath);
#endif
    char *path = relocate(origpath);
    su_FREE(origpath);
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
    su_FREE(name);
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
  dirs = su_TALLOC(char, ((e && *e) ? su_cs_len(e) + 1 : 0)
        + (add_current ? 1 + 1 : 0)
        + ((home && *home) ? su_cs_len(home) + 1 : 0)
        + ((standard && *standard) ? su_cs_len(standard) : 0)
        + 1);
  *dirs = '\0';
  char *cp = dirs;

  if (e && *e) {
    cp = su_cs_pcopy(su_cs_pcopy(cp, e), PATH_SEP);
  }
  if (add_current) {
    cp = su_cs_pcopy(su_cs_pcopy(cp, "."), PATH_SEP);
  }
  if (home && *home) {
    cp = su_cs_pcopy(su_cs_pcopy(cp, home), PATH_SEP);
  }
  if (standard && *standard)
    cp = su_cs_pcopy(cp, standard);
  init_len = PTR2UZ(cp - dirst);
}

search_path::~search_path()
{
  // dirs is always allocated
  su_FREE(dirs);
}

void search_path::command_line_dir(const char *s)
{
  char *old = dirs;
  unsigned old_len = su_cs_len(old);
  unsigned slen = su_cs_len(s);
  dirs = su_TALLOC(char, old_len + 1 + slen + 1);
  su_mem_copy(dirs, old, old_len - init_len);
  char *p = dirs;
  p += old_len - init_len;
  if (init_len == 0)
    *p++ = PATH_SEP_CHAR;
  su_mem_copy(p, s, slen);
  p += slen;
  if (init_len > 0) {
    *p++ = PATH_SEP_CHAR;
    su_mem_copy(p, old + old_len - init_len, init_len);
    p += init_len;
  }
  *p++ = '\0';
  su_FREE(old);
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
  if (name == NULL || !su_cs_cmp(name, "-")) {
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
