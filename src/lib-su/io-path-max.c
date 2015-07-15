/*@ su_path_name_max(dname): pathconf(dname, _PC_PATH_MAX).
 *
 * Copyright (c) 2017 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#define su_FILE "su__io_path_max"

#include "su/primary.h"

#include <unistd.h>

#include "su/io.h"
#include "su/code-in.h"

/* POSIX 2008/Cor 1-2013 defines for
 * - _POSIX_PATH_MAX a minimum of 256
 * - _XOPEN_PATH_MAX a minimum of 1024
 * NFS RFC 1094 from March 1989 defines a MAXPATHLEN of 1024, so we really
 * should avoid anything smaller than that! */
#ifndef PATH_MAX
# ifdef MAXPATHLEN
#  define PATH_MAX MAXPATHLEN
# else
#  define PATH_MAX 1024
# endif
#endif

#if PATH_MAX + 0 < 1024
# undef PATH_MAX
# define PATH_MAX 1024
#endif

uiz
su_path_name_max(char const *dname_or_nil){
   uiz rv;
#ifdef HAVE_PATHCONF
   long rv;
#endif
   NYD_IN;
   UNUSED(dname_or_nil);

#ifdef HAVE_PATHCONF
   if(dname_or_nil == NIL)
      dname_or_nil = "/"; /* TODO dirsep configurable */

   if((sr = pathconf(dname_or_nil, _PC_PATH_MAX)) != -1)
      rv = S(uiz,sr);
   else
#endif
      rv = PATH_MAX;
   NYD_OU;
   return rv;
}

#include "su/code-ou.h"
/* s-it-mode */
