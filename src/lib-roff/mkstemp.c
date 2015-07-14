/*@ FIXME Take that thing from S-nail instead!
 *
 * Copyright (C) 2001, 2003, 2004 Free Software Foundation, Inc.
 *       Written by Werner Lemberg (wl@gnu.org)
 *
 * Copyright (c) 2014 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "nonposix.h"
#include "posix.h"

#ifndef TMP_MAX
# define TMP_MAX 238328 /* FIXME lib.h/config.h stuff! */
#endif

int
rf_mkstemp(char *tmpl, su_boole wantdir){
   static char const a_letters[62] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
   static su_ui64 a_value;

   char *XXXXXX;
   su_uiz len, i;
   int rv;
   su_NYD_IN;

   rv = -1;

   if((len = strlen(tmpl)) < 6 || strcmp(&tmpl[len - 6], "XXXXXX"))
      goto jleave;

   /* This is where the Xs start.   */
   XXXXXX = &tmpl[len - 6];

   /* Get some more or less random data.  */
   i = getpid();
#ifdef HAVE_CLOCK_GETTIME
   {
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   value += (((su_ui64)tv.tv_nsec << 16) ^ tv.tv_sec) ^ i;
   }
#elif defined HAVE_GETTIMEOFDAY
   {
   timeval tv;
   gettimeofday(&tv, NULL);
   value += (((su_ui64)tv.tv_usec << 16) ^ tv.tv_sec) ^ i;
   }
#else
   value += (su_ui64)time(NULL) ^ i;
#endif

   for(i = 0; i < TMP_MAX; a_value *= 33, ++i){
      su_ui64 v;

      /* Fill in the random bits.  */
#undef _X
#define _X(I) \
   XXXXXX[I] = a_letters[v % su_NELEM(a_letters)];\
   v /= su_NELEM(a_letters)

      v = a_value;
      _X(0);
      _X(1);
      _X(2);
      _X(3);
      _X(4);
      _X(5);
#undef _X

      rv = wantdir ? mkdir(tmpl, S_IRUSR | S_IWUSR | S_IXUSR)
            : open(tmpl, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
      if(rv >= 0 || errno != EEXIST)
         break;
   }
jleave:
   su_NYD_OU;
   return rv;
}

/* s-it-mode */
