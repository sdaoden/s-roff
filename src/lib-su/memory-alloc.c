/*@ Implementation of memory.h: allocation functions.
 *@ TODO Yes.  And why not provide LOFI memory?
 *
 * Copyright (c) 2012 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#define su_FILE "su__memory_alloc"

#include "su/primary.h"

#include <stdlib.h>
#include <string.h>

#include "su/memory.h"
#include "su/code-in.h"

#ifdef su_HAVE_MEMORY_DEBUG
CTA(sizeof(char) == sizeof(ui8), "But POSIX says a byte is 8 bit");

# define a_MEMORY_HOPE_SIZE (2 * 8 * sizeof(char))
# define a_MEMORY_HOPE_INC(P) (P) += 8
# define a_MEMORY_HOPE_DEC(P) (P) -= 8

/* We use address-induced canary values, inspiration (but he didn't invent)
 * and primes from maxv@netbsd.org, src/sys/kern/subr_kmem.c */
# define a_MEMORY_HOPE_LOWER(S,P) \
do{\
   ui64 __h__ = (uip)(P);\
   __h__ *= ((ui64)0x9E37FFFFu << 32) | 0xFFFC0000u;\
   __h__ >>= 56;\
   (S) = (ui8)__h__;\
}while(0)

# define a_MEMORY_HOPE_UPPER(S,P) \
do{\
   ui32 __i__;\
   ui64 __x__, __h__ = (uip)(P);\
   __h__ *= ((ui64)0x9E37FFFFu << 32) | 0xFFFC0000u;\
   for(__i__ = 56; __i__ != 0; __i__ -= 8)\
      if((__x__ = (__h__ >> __i__)) != 0){\
         (S) = (ui8_t)__x__;\
         break;\
      }\
   if(__i__ == 0)\
      (S) = 0xAAu;\
}while(0)

# define a_MEMORY_HOPE_SET(T,C) \
do{\
   union a_memory_ptr __xp;\
   struct a_memory_chunk *__xc;\
   __xp.p_vp = (C).p_vp;\
   __xc = (struct a_memory_chunk*)(__xp.T - 1);\
   a_MEMORY_HOPE_INC((C).p_cp);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[0], &__xp.p_ui8p[0]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[1], &__xp.p_ui8p[1]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[2], &__xp.p_ui8p[2]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[3], &__xp.p_ui8p[3]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[4], &__xp.p_ui8p[4]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[5], &__xp.p_ui8p[5]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[6], &__xp.p_ui8p[6]);\
   a_MEMORY_HOPE_LOWER(__xp.p_ui8p[7], &__xp.p_ui8p[7]);\
   a_MEMORY_HOPE_INC(__xp.p_ui8p) + __xc->mc_user_size;\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[0], &__xp.p_ui8p[0]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[1], &__xp.p_ui8p[1]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[2], &__xp.p_ui8p[2]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[3], &__xp.p_ui8p[3]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[4], &__xp.p_ui8p[4]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[5], &__xp.p_ui8p[5]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[6], &__xp.p_ui8p[6]);\
   a_MEMORY_HOPE_UPPER(__xp.p_ui8p[7], &__xp.p_ui8p[7]);\
}while(0)

# define a_MEMORY_HOPE_GET_TRACE(T,C,BAD) \
do{\
   a_MEMORY_HOPE_INC((C).p_cp);\
   a_MEMORY_HOPE_GET(T, C, BAD);\
   a_MEMORY_HOPE_INC((C).p_cp);\
}while(0)

# define a_MEMORY_HOPE_GET(T,C,BAD) \
do{\
   union a_memory_ptr __xp;\
   struct a_memory_chunk *__xc;\
   ui32 __i;\
   ui8 __m;\
   __xp.p_vp = (C).p_vp;\
   a_MEMORY_HOPE_DEC(__xp.p_cp);\
   (C).p_cp = __xp.p_cp;\
   __xc = (struct a_memory_chunk*)(__xp.T - 1);\
   (BAD) = FAL0;\
   __i = 0;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[0]);\
      if(__xp.p_ui8p[0] != __m) __i |= 1<<0;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[1]);\
      if(__xp.p_ui8p[1] != __m) __i |= 1<<1;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[2]);\
      if(__xp.p_ui8p[2] != __m) __i |= 1<<2;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[3]);\
      if(__xp.p_ui8p[3] != __m) __i |= 1<<3;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[4]);\
      if(__xp.p_ui8p[4] != __m) __i |= 1<<4;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[5]);\
      if(__xp.p_ui8p[5] != __m) __i |= 1<<5;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[6]);\
      if(__xp.p_ui8p[6] != __m) __i |= 1<<6;\
   a_MEMORY_HOPE_LOWER(__m, &__xp.p_ui8p[7]);\
      if(__xp.p_ui8p[7] != __m) __i |= 1<<7;\
   if(__i != 0){\
      (BAD) = TRU1;\
      a_MEMORY_HOPE_INC((C).p_cp);\
      su_alert("%p: corrupt lower canary: 0x%02X: %s, line %d",\
         (C).p_cp, __i, mdbg_file, mdbg_line);\
      a_MEMORY_HOPE_DEC((C).p_cp);\
   }\
   a_MEMORY_HOPE_INC(__xp.p_ui8p) + __xc->mc_user_size;\
   __i = 0;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[0]);\
      if(__xp.p_ui8p[0] != __m) __i |= 1<<0;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[1]);\
      if(__xp.p_ui8p[1] != __m) __i |= 1<<1;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[2]);\
      if(__xp.p_ui8p[2] != __m) __i |= 1<<2;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[3]);\
      if(__xp.p_ui8p[3] != __m) __i |= 1<<3;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[4]);\
      if(__xp.p_ui8p[4] != __m) __i |= 1<<4;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[5]);\
      if(__xp.p_ui8p[5] != __m) __i |= 1<<5;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[6]);\
      if(__xp.p_ui8p[6] != __m) __i |= 1<<6;\
   a_MEMORY_HOPE_UPPER(__m, &__xp.p_ui8p[7]);\
      if(__xp.p_ui8p[7] != __m) __i |= 1<<7;\
   if(__i != 0){\
      (BAD) = TRU1;\
      a_MEMORY_HOPE_INC((C).p_cp);\
      su_alert("%p: corrupt upper canary: 0x%02X: %s, line %d",\
         (C).p_cp, __i, mdbg_file, mdbg_line);\
      a_MEMORY_HOPE_DEC((C).p_cp);\
   }\
   if(BAD)\
      su_alert("   ..canary last seen: %s, line %u",\
         __xc->mc_file, __xc->mc_line);\
}while(0)
#endif /* su_HAVE_MEMORY_DEBUG */

#ifdef su_HAVE_MEMORY_DEBUG
struct a_memory_chunk{
   char const *mc_file;
   ui32 mc_line;
   ui8 mc_isfree;
   ui8 mc__dummy[3];
   ui32 mc_user_size;
   ui32 mc_size;
};

/* The heap memory su_free() may become delayed to detect double frees.
 * It is primitive, but ok: speed and memory usage don't matter here, and SMP
 * is no issue for us (SMP: thread-specific data or atomic lock, then) */
struct a_memory_heap_chunk{
   struct a_memory_chunk mhc_super;
   struct a_memory_heap_chunk *mhc_prev;
   struct a_memory_heap_chunk *mhc_next;
};
#endif /* su_HAVE_MEMORY_DEBUG */

union a_memory_ptr{
   void *p_vp;
   char *p_cp;
   ui8 *p_ui8p;
#ifdef su_HAVE_MEMORY_DEBUG
   struct a_memory_chunk *p_c;
   struct a_memory_heap_chunk *p_hc;
#endif
};

static char const a_memory_outof[] = N_("Out of memory\n");

#ifdef su_HAVE_MEMORY_DEBUG
static uiz a_memory_heap_aall, a_memory_heap_acur, a_memory_heap_amax,
      a_memory_heap_mall, a_memory_heap_mcur, a_memory_heap_mmax;
static struct a_memory_heap_chunk *a_memory_heap_list, *a_memory_heap_free;
#endif

#ifndef su_HAVE_MEMORY_DEBUG
void *
su_alloc(size_t size){
   void *rv;
   NYD2_IN;

   if(size == 0)
      size = 1;
   if((rv = malloc(size)) == NIL)
      su_panic(V_(a_memory_outof));
   NYD2_OU;
   return rv;
}

void *
su_realloc(void *vp, uiz size){
   void *rv;
   NYD2_IN;

   if(vp == NIL)
      rv = su_alloc(s);
   else{
      if(size == 0)
         size = 1;
      if((rv = realloc(vp, size)) == NIL)
         su_panic(V_(a_memory_outof));
   }
   NYD2_OU;
   return rv;
}

void *
su_calloc(uiz nmemb, uiz size){ /* TODO overflow non-debug */
   void *rv;
   NYD2_IN;

   if(size == 0)
      size = 1;
   if((rv = calloc(nmemb, size)) == NIL)
      su_panic(V_(a_memory_outof));
   NYD2_OU;
   return rv;
}

void
su_free(void *vp){ /* TODO log on NIL, we do not do that */
   NYD2_IN;
   free(vp);
   NYD2_OU;
}

#else /* !su_HAVE_MEMORY_DEBUG */
void *
(su_alloc)(uiz size  su_MEMORY_DEBUG_ARGS){
   union a_memory_ptr p;
   ui32 user_s;
   NYD2_IN;

   if(size > UI32_MAX - sizeof(struct a_memory_heap_chunk) -
         a_MEMORY_HOPE_SIZE)
      su_panic("su_alloc(): allocation too large: %s, line %d",
         mdbg_file, mdbg_line);
   if((user_size = (ui32)size) == 0)
      size = 1;
   size += sizeof(struct a_memory_heap_chunk) + a_MEMORY_HOPE_SIZE;

   if((p.p_vp = malloc(size)) == NIL)
      su_panic(V_(a_memory_outof));

   p.p_hc->mhc_prev = NIL;
   if((p.p_hc->mhc_next = a_memory_heap_list) != NIL)
      a_memory_heap_list->mhc_prev = p.p_hc;

   p.p_c->mc_file = mdbg_file;
   p.p_c->mc_line = (ui16)mdbg_line;
   p.p_c->mc_isfree = FAL0;
   p.p_c->mc_user_size = user_size;
   p.p_c->mc_size = (ui32)size;

   a_memory_heap_list = p.p_hc++;
   a_MEMORY_HOPE_SET(p_hc, p);

   ++a_memory_heap_aall;
   ++a_memory_heap_acur;
   a_memory_heap_amax = su_MAX(a_memory_heap_amax, a_memory_heap_acur);
   a_memory_heap_mall += user_size;
   a_memory_heap_mcur += user_size;
   a_memory_heap_mmax = su_MAX(a_memory_heap_mmax, a_memory_heap_mcur);
   NYD2_OU;
   return p.p_vp;
}

void *
(su_realloc)(void *vp, uiz size  su_MEMORY_DEBUG_ARGS){
   union a_memory_ptr p;
   ui32 user_size;
   boole isbad;
   NYD2_IN;

   if((p.p_vp = vp) == NIL){
jforce:
      p.p_vp = (su_alloc)(size, mdbg_file, mdbg_line);
      goto jleave;
   }

   a_MEMORY_HOPE_GET(p_hc, p, isbad);
   --p.p_hc;

   if(p.p_c->mc_isfree){
      su_alert("su_realloc(): region freed!  At %s, line %d\n"
         "\tLast seen: %s, line %" PRIu16 "\n",
         mdbg_file, mdbg_line, p.p_c->mc_file, p.p_c->mc_line);
      goto jforce;
   }

   if(p.p_hc == a_memory_heap_list)
      a_memory_heap_list = p.p_hc->mhc_next;
   else
      p.p_hc->mhc_prev->mhc_next = p.p_hc->mhc_next;
   if (p.p_hc->mhc_next != NIL)
      p.p_hc->mhc_next->mhc_prev = p.p_hc->mhc_prev;

   --a_memory_heap_acur;
   a_memory_heap_mcur -= p.p_c->mc_user_size;

   if(size > UI32_MAX - sizeof(struct a_memory_heap_chunk) -
         a_MEMORY_HOPE_SIZE)
      su_panic("su_realloc(): allocation too large: %s, line %d",
         mdbg_file, mdbg_line);
   if((user_size = (ui32)size) == 0)
      size = 1;
   size += sizeof(struct a_memory_heap_chunk) + a_MEMORY_HOPE_SIZE;

   if((p.p_vp = realloc(p.p_c, size)) == NIL)
      su_panic(V_(a_memory_outof));
   p.p_hc->mhc_prev = NIL;
   if((p.p_hc->mhc_next = a_memory_heap_list) != NIL)
      a_memory_heap_list->mhc_prev = p.p_hc;

   p.p_c->mc_file = mdbg_file;
   p.p_c->mc_line = (ui16)mdbg_line;
   p.p_c->mc_isfree = FAL0;
   p.p_c->mc_user_size = user_size;
   p.p_c->mc_size = (ui32)size;

   a_memory_heap_list = p.p_hc++;
   a_MEMORY_HOPE_SET(p_hc, p);

   ++a_memory_heap_aall;
   ++a_memory_heap_acur;
   a_memory_heap_amax = su_MAX(a_memory_heap_amax, a_memory_heap_acur);
   a_memory_heap_mall += user_size;
   a_memory_heap_mcur += user_size;
   a_memory_heap_mmax = su_MAX(a_memory_heap_mmax, a_memory_heap_mcur);
jleave:
   NYD2_OU;
   return p.p_vp;
}

void *
(su_calloc)(uiz nmemb, uiz size  su_MEMORY_DEBUG_ARGS){
   union a_memory_ptr p;
   ui32 user_size;
   NYD2_IN;

   if(nmemb == 0)
      nmemb = 1;
   if(size > UI32_MAX - sizeof(struct a_memory_heap_chunk) -
         a_MEMORY_HOPE_SIZE)
      su_panic("su_calloc(): allocation size too large: %s, line %d",
         mdbg_file, mdbg_line);
   if((user_size = (ui32)size) == 0)
      size = 1;
   if((UI32_MAX - sizeof(struct a_memory_heap_chunk) - a_MEMORY_HOPE_SIZE) /
         nmemb < size)
      su_panic("su_calloc(): allocation count too large: %s, line %d",
         mdbg_file, mdbg_line);

   size *= nmemb;
   size += sizeof(struct a_memory_heap_chunk) + a_MEMORY_HOPE_SIZE;

   if((p.p_vp = malloc(size)) == NIL)
      su_panic(V_(a_memory_outof));
   su_memset(p.p_vp, 0, size);

   p.p_hc->mhc_prev = NIL;
   if((p.p_hc->mhc_next = a_memory_heap_list) != NIL)
      a_memory_heap_list->mhc_prev = p.p_hc;

   p.p_c->mc_file = mdbg_file;
   p.p_c->mc_line = (ui16)mdbg_line;
   p.p_c->mc_isfree = FAL0;
   p.p_c->mc_user_size = (user_size > 0) ? (user_size *= nmemb) : 0;
   p.p_c->mc_size = (ui32)size;

   a_memory_heap_list = p.p_hc++;
   a_MEMORY_HOPE_SET(p_hc, p);

   ++a_memory_heap_aall;
   ++a_memory_heap_acur;
   a_memory_heap_amax = su_MAX(a_memory_heap_amax, a_memory_heap_acur);
   a_memory_heap_mall += user_size;
   a_memory_heap_mcur += user_size;
   a_memory_heap_mmax = su_MAX(a_memory_heap_mmax, a_memory_heap_mcur);
   NYD2_OU;
   return p.p_vp;
}

void
(su_free)(void *vp  su_MEMORY_DEBUG_ARGS){
   union a_memory_ptr p;
   boole isbad;
   NYD2_IN;

   if((p.p_vp = vp) == NIL){
      su_err("su_free(NIL) from %s, line %d\n", mdbg_file, mdbg_line);
      goto jleave;
   }

   a_MEMORY_HOPE_GET(p_hc, p, isbad);
   --p.p_hc;

   if(p.p_c->mc_isfree){
      su_alert("su_free(): double-free avoided at %s, line %d\n"
         "\tLast seen: %s, line %" PRIu16 "\n",
         mdbg_file, mdbg_line, p.p_c->mc_file, p.p_c->mc_line);
      goto jleave;
   }

   if(p.p_hc == a_memory_heap_list){
      if((a_memory_heap_list = p.p_hc->mhc_next) != NIL)
         a_memory_heap_list->mhc_prev = NIL;
   }else
      p.p_hc->mhc_prev->mhc_next = p.p_hc->mhc_next;
   if(p.p_hc->mhc_next != NIL)
      p.p_hc->mhc_next->mhc_prev = p.p_hc->mhc_prev;

   p.p_c->mc_isfree = TRU1;
   /* Trash contents */
   su_memset(vp, 0377, p.p_c->mc_user_size);

   --a_memory_heap_acur;
   a_memory_heap_mcur -= p.p_c->mc_user_size;

   if(su_state & (su_SF_DEBUG | su_SF_MEMORY_DEBUG)){
      p.p_hc->mhc_next = a_memory_heap_free;
      a_memory_heap_free = p.p_hc;
   }else
      free(p.p_vp);
jleave:
   NYD2_OU;
}
#endif /* su_HAVE_MEMORY_DEBUG */

void
su_memory_reset(void){
#ifdef su_HAVE_MEMORY_DEBUG
   union a_memory_ptr p;
   uiz c, s;
#endif
   NYD_IN;

   su_memory_check();

   /* Now we are ready to deal with heap */
#ifdef su_HAVE_MEMORY_DEBUG
   c = s = 0;

   for(p.p_hc = a_memory_heap_free; p.p_hc != NIL;){
      void *vp;

      vp = p.p_hc;
      ++c;
      s += p.p_c->mc_size;
      p.p_hc = p.p_hc->mhc_next;
      free(vp);
   }
   a_memory_heap_free = NIL;

   su_debug("su_memory_reset(): freed %lu" PRIuZ " chunks/%" PRIuZ " bytes\n",
      c, s);
#endif
   NYD_OU;
}

#ifdef su_HAVE_MEMORY_DEBUG
void
su_memory_trace(void){
   /* For a_MEMORY_HOPE_GET() */
   char const * const mdbg_file = "su_memory_trace()";
   int const mdbg_line = -1;

   boole isbad;
   union a_memory_ptr p, xp;
   uiz lines;
   ui32 ostate;
   NYD2_IN;

   ostate = su_state;
   su_state |= su_SF_DEBUG;

   lines = 0;

   su_debug(
      "\nHeap memory buffers:\n"
      "  Allocation cur/peek/all: %7" PRIuZ "/%7" PRIuZ "/%10" PRIuZ "\n"
      "       Bytes cur/peek/all: %7" PRIuZ "/%7" PRIuZ "/%10" PRIuZ "\n\n",
      a_memory_heap_acur, a_memory_heap_amax, a_memory_heap_aall,
      a_memory_heap_mcur, a_memory_heap_mmax, a_memory_heap_mall);
   lines += 6;

   for(p.p_hc = a_memory_heap_list; p.p_hc != NIL;
         ++lines, p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMORY_HOPE_GET_TRACE(p_hc, xp, isbad);
      (isbad ? su_alert : su_debug)("  %s%p (%u bytes): %s, line %u\n",
         (isbad ? "! CANARY ERROR (heap): " : ""), xp.p_vp,
         p.p_c->mc_user_size, p.p_c->mc_file, p.p_c->mc_line);
   }

   su_debug("Freed heap buffers lingering for release:\n");
   ++lines;

   for(p.p_hc = a_memory_heap_free; p.p_hc != NIL;
         ++lines, p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMORY_HOPE_GET_TRACE(p_hc, xp, isbad);
      (isbad ? su_alert : su_debug)("  %s%p (%u bytes): %s, line %u\n",
         (isbad ? "! CANARY ERROR (free): " : ""), xp.p_vp,
         p.p_c->mc_user_size, p.p_c->mc_file, p.p_c->mc_line);
   }

   su_state = ostate;
   NYD2_OU;
}

void
su_memory_check(char const *mdbg_file, int mdbg_line){
   union a_memory_ptr p, xp;
   boole anybad, isbad;
   NYD2_IN;

   anybad = FAL0;

   for(p.p_hc = a_memory_heap_list; p.p_hc != NIL; p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMORY_HOPE_GET_TRACE(p_hc, xp, isbad);
      if(isbad){
         anybad = TRU1;
         su_alert("! CANARY ERROR (heap): %p (%u bytes): %s, line %u\n",
            xp.p_vp, p.p_c->mc_user_size, p.p_c->mc_file, p.p_c->mc_line);
      }
   }

   for(p.p_hc = a_memory_heap_free; p.p_hc != NIL; p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMORY_HOPE_GET_TRACE(p_hc, xp, isbad);
      if(isbad){
         anybad = TRU1;
         su_alert("! CANARY ERROR (free): %p (%u bytes): %s, line %u\n",
            xp.p_vp, p.p_c->mc_user_size, p.p_c->mc_file, p.p_c->mc_line);
      }
   }

   if(anybad)
      su_panic("Memory errors encountered");
   NYD2_OU;
}
#endif /* su_HAVE_MEMORY_DEBUG */

#include "su/code-ou.h"
/* s-it-mode */
