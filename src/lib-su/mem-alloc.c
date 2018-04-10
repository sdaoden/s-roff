/*@ Implementation of mem.h: allocation functions.
 *@ TODO Yes.  Introduce su_HAVE_MEM_CYCLES and thus optionally reintroduce
 *@ TODO _autorec_ and _lofi_ allocations.  Later: port the C++ cache, use it.
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
#define su_FILE "su__mem_alloc"
#define su_SOURCE
#define su_SOURCE_MEM_ALLOC

#include "su/code.h"

#include <stdlib.h> /* FIXME port C++ memcache! */

#include "su/mem.h"
#include "su/code-in.h"

#ifndef su_HAVE_MEM_DEBUG
# define a_MEMA_DBG(X)
# define a_MEMA_HOPE_SIZE_ADD 0
#else
CTA(sizeof(char) == sizeof(u8), "But POSIX says a byte is 8 bit");

# define a_MEMA_DBG(X) X
# define a_MEMA_HOPE_SIZE (2 * 8 * sizeof(char))
# define a_MEMA_HOPE_INC(P) (P) += 8
# define a_MEMA_HOPE_DEC(P) (P) -= 8
# define a_MEMA_HOPE_SIZE_ADD \
   (a_MEMA_HOPE_SIZE + sizeof(struct a_mema_heap_chunk))

   /* We use address-induced canary values, inspiration (but he didn't invent)
    * and primes from maxv@netbsd.org, src/sys/kern/subr_kmem.c */
# define a_MEMA_HOPE_LOWER(S,P) \
do{\
   u64 __h__ = S(up,P);\
   __h__ *= (S(u64,0x9E37FFFFu) << 32) | 0xFFFC0000u;\
   __h__ >>= 56;\
   (S) = S(u8,__h__);\
}while(0)

# define a_MEMA_HOPE_UPPER(S,P) \
do{\
   u32 __i__;\
   u64 __x__, __h__ = S(up,P);\
   __h__ *= (S(u64,0x9E37FFFFu) << 32) | 0xFFFC0000u;\
   for(__i__ = 56; __i__ != 0; __i__ -= 8)\
      if((__x__ = (__h__ >> __i__)) != 0){\
         (S) = S(u8,__x__);\
         break;\
      }\
   if(__i__ == 0)\
      (S) = 0xAAu;\
}while(0)

# define a_MEMA_HOPE_SET(T,C) \
do{\
   union a_mema_ptr __xp;\
   struct a_mema_chunk *__xc;\
   __xp.p_vp = (C).p_vp;\
   __xc = R(struct a_mema_chunk*,__xp.T - 1);\
   a_MEMA_HOPE_INC((C).p_cp);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[0], &__xp.p_u8p[0]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[1], &__xp.p_u8p[1]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[2], &__xp.p_u8p[2]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[3], &__xp.p_u8p[3]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[4], &__xp.p_u8p[4]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[5], &__xp.p_u8p[5]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[6], &__xp.p_u8p[6]);\
   a_MEMA_HOPE_LOWER(__xp.p_u8p[7], &__xp.p_u8p[7]);\
   a_MEMA_HOPE_INC(__xp.p_u8p) + __xc->mac_size - __xc->mac_user_off;\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[0], &__xp.p_u8p[0]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[1], &__xp.p_u8p[1]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[2], &__xp.p_u8p[2]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[3], &__xp.p_u8p[3]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[4], &__xp.p_u8p[4]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[5], &__xp.p_u8p[5]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[6], &__xp.p_u8p[6]);\
   a_MEMA_HOPE_UPPER(__xp.p_u8p[7], &__xp.p_u8p[7]);\
}while(0)

# define a_MEMA_HOPE_GET_TRACE(T,C,BAD) \
do{\
   a_MEMA_HOPE_INC((C).p_cp);\
   a_MEMA_HOPE_GET(T, C, BAD);\
   a_MEMA_HOPE_INC((C).p_cp);\
}while(0)

# define a_MEMA_HOPE_GET(T,C,BAD) \
do{\
   union a_mema_ptr __xp;\
   struct a_mema_chunk *__xc;\
   u32 __i;\
   u8 __m;\
   __xp.p_vp = (C).p_vp;\
   a_MEMA_HOPE_DEC(__xp.p_cp);\
   (C).p_cp = __xp.p_cp;\
   __xc = R(struct a_mema_chunk*,__xp.T - 1);\
   (BAD) = FAL0;\
   __i = 0;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[0]);\
      if(__xp.p_u8p[0] != __m) __i |= 1<<0;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[1]);\
      if(__xp.p_u8p[1] != __m) __i |= 1<<1;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[2]);\
      if(__xp.p_u8p[2] != __m) __i |= 1<<2;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[3]);\
      if(__xp.p_u8p[3] != __m) __i |= 1<<3;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[4]);\
      if(__xp.p_u8p[4] != __m) __i |= 1<<4;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[5]);\
      if(__xp.p_u8p[5] != __m) __i |= 1<<5;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[6]);\
      if(__xp.p_u8p[6] != __m) __i |= 1<<6;\
   a_MEMA_HOPE_LOWER(__m, &__xp.p_u8p[7]);\
      if(__xp.p_u8p[7] != __m) __i |= 1<<7;\
   if(__i != 0){\
      (BAD) = TRU1;\
      a_MEMA_HOPE_INC((C).p_cp);\
      su_log(su_LOG_ALERT,\
         "! SU memory: %p: corrupt lower canary: 0x%02X: %s, line %d",\
         (C).p_cp, __i, mdbg_file, mdbg_line);\
      a_MEMA_HOPE_DEC((C).p_cp);\
   }\
   a_MEMA_HOPE_INC(__xp.p_u8p) + __xc->mac_size - __xc->mac_user_off;\
   __i = 0;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[0]);\
      if(__xp.p_u8p[0] != __m) __i |= 1<<0;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[1]);\
      if(__xp.p_u8p[1] != __m) __i |= 1<<1;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[2]);\
      if(__xp.p_u8p[2] != __m) __i |= 1<<2;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[3]);\
      if(__xp.p_u8p[3] != __m) __i |= 1<<3;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[4]);\
      if(__xp.p_u8p[4] != __m) __i |= 1<<4;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[5]);\
      if(__xp.p_u8p[5] != __m) __i |= 1<<5;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[6]);\
      if(__xp.p_u8p[6] != __m) __i |= 1<<6;\
   a_MEMA_HOPE_UPPER(__m, &__xp.p_u8p[7]);\
      if(__xp.p_u8p[7] != __m) __i |= 1<<7;\
   if(__i != 0){\
      (BAD) = TRU1;\
      a_MEMA_HOPE_INC((C).p_cp);\
      su_log(su_LOG_ALERT,\
         "! SU memory: %p: corrupt upper canary: 0x%02X: %s, line %d",\
         (C).p_cp, __i, mdbg_file, mdbg_line);\
      a_MEMA_HOPE_DEC((C).p_cp);\
   }\
   if(BAD)\
      su_log(su_LOG_ALERT,\
         "! SU memory:   ..canary last seen: %s, line %u",\
         __xc->mac_file, __xc->mac_line);\
}while(0)
#endif /* su_HAVE_MEM_DEBUG */

#ifdef su_HAVE_MEM_DEBUG
struct a_mema_chunk{
   char const *mac_file;
   u32 mac_line;
# if su_ENDIAN == su_ENDIAN_BIG
   u32 mac_isfree : 1;
# endif
   u32 mac_user_off : 31;  /* .mac_size-.mac_user_off: user size */
# if su_ENDIAN == su_ENDIAN_LITTLE
   u32 mac_isfree : 1;
# endif
   uz mac_size;
};

/* The heap memory mem_free() may become delayed to detect double frees.
 * It is primitive, but ok: speed and memory usage don't matter here, and SMP
 * is no issue for us (SMP: thread-specific data or atomic lock, then) */
struct a_mema_heap_chunk{
   struct a_mema_chunk mhc_super;
   struct a_mema_heap_chunk *mhc_prev;
   struct a_mema_heap_chunk *mhc_next;
};
#endif /* su_HAVE_MEM_DEBUG */

union a_mema_ptr{
   void *p_vp;
   char *p_cp;
   u8 *p_u8p;
#ifdef su_HAVE_MEM_DEBUG
   struct a_mema_chunk *p_c;
   struct a_mema_heap_chunk *p_hc;
#endif
};

#ifdef su_HAVE_MEM_DEBUG
static uz a_mema_heap_aall, a_mema_heap_acur, a_mema_heap_amax,
      a_mema_heap_mall, a_mema_heap_mcur, a_mema_heap_mmax;
static struct a_mema_heap_chunk *a_mema_heap_list, *a_mema_heap_free;
#endif

void *
su_mem_allocate(uz sz, uz no, enum su_mem_alloc_flags maf
      su_MEM_DEBUG_ARGS_DECL){
#ifdef su_MEM_HAVE_DEBUG
   union a_mema_ptr p;
   uz user_sz = sz;
#endif
   boole domul;
   void *rv;
   NYD_IN;

   rv = NIL;

   if(UNLIKELY(sz == 0))
      sz = 1;
   domul = (no > 1);

   if(a_MEMA_DBG( UZ_MAX - a_MEMA_HOPE_SIZE_ADD > sz && )
         LIKELY(!domul || UZ_MAX / no > sz + a_MEMA_HOPE_SIZE_ADD)){
      if(domul)
         sz *= no;
      a_MEMA_DBG( sz += a_MEMA_HOPE_SIZE_ADD; )

      if(LIKELY((rv = malloc(sz)) != NIL)){
         /* XXX Of course this may run on odd ranges, but once upon a time
          * XXX i will port my C++ cache and then we're fine again */
         if(maf & su_MEM_ALLOC_CLEAR)
            su_mem_set(rv, 0, sz);
#ifdef su_HAVE_MEM_DEBUG
         p.p_vp = rv;

         p.p_hc->mhc_prev = NIL;
         if((p.p_hc->mhc_next = a_mema_heap_list) != NIL)
            a_mema_heap_list->mhc_prev = p.p_hc;
         p.p_c->mac_file = mdbg_file;
         p.p_c->mac_line = mdbg_line;
         p.p_c->mac_isfree = FAL0;
         ASSERT(sz - user_sz <= S32_MAX);
         p.p_c->mac_user_off = S(u32,sz - user_sz);
         p.p_c->mac_size = sz;
         a_mema_heap_list = p.p_hc++;
         a_MEMA_HOPE_SET(p_hc, p);

         rv = p.p_vp;

         ++a_mema_heap_aall;
         ++a_mema_heap_acur;
         a_mema_heap_amax = su_MAX(a_mema_heap_amax, a_mema_heap_acur);
         a_mema_heap_mall += user_size;
         a_mema_heap_mcur += user_size;
         a_mema_heap_mmax = su_MAX(a_mema_heap_mmax, a_mema_heap_mcur);
#endif /* su_HAVE_MEM_DEBUG */
      }else
         su_state_err((su_STATE_ERR_NOMEM |
               (maf & su_MEM_ALLOC_NOMEM_OK ? su_STATE_ERR_PASS : 0) |
               (maf & su_MEM_ALLOC_MUSTFAIL ? su_STATE_ERR_NOPASS : 0)),
            _("SU memory: allocation request"));
   }else
      su_state_err((su_STATE_ERR_OVERFLOW |
            (maf & su_MEM_ALLOC_OVERFLOW_OK ? su_STATE_ERR_PASS : 0) |
            (maf & su_MEM_ALLOC_MUSTFAIL ? su_STATE_ERR_NOPASS : 0)),
         _("SU memory: allocation request"));
   NYD_OU;
   return rv;
}

void *
su_mem_reallocate(void *ovp, uz sz, uz no, enum su_mem_alloc_flags maf
      su_MEM_DEBUG_ARGS_DECL){
#ifdef su_MEM_HAVE_DEBUG
   union a_mema_ptr p;
   uz user_sz = sz;
#endif
   boole domul;
   void *rv;
   NYD_IN;

   rv = NIL;

   if(UNLIKELY(sz == 0))
      sz = 1;
   domul = (no > 1);

#ifdef su_HAVE_MEM_DEBUG
   if((p.p_vp = ovp) != NIL){
      a_MEMA_HOPE_GET(p_hc, p, isbad);
      --p.p_hc;

      if(!p.p_c->mac_isfree){
         if(p.p_hc == a_mema_heap_list)
            a_mema_heap_list = p.p_hc->mhc_next;
         else
            p.p_hc->mhc_prev->mhc_next = p.p_hc->mhc_next;
         if (p.p_hc->mhc_next != NIL)
            p.p_hc->mhc_next->mhc_prev = p.p_hc->mhc_prev;

         --a_mema_heap_acur;
         a_mema_heap_mcur -= p.p_c->mac_size - p.p_c->mac_user_off;

         ovp = p.p_vp;
      }else{
         su_log(su_LOG_ALERT,
            "SU memory: reallocation: region freed!  At %s, line %d\n"
            "\tLast seen: %s, line %" PRIu32 "\n",
            mdbg_file, mdbg_line, p.p_c->mac_file, p.p_c->mac_line);
         su_state_err((su_STATE_ERR_NOMEM |
               (maf & su_MEM_ALLOC_NOMEM_OK ? su_STATE_ERR_PASS : 0) |
               (maf & su_MEM_ALLOC_MUSTFAIL ? su_STATE_ERR_NOPASS : 0)),
            _("SU memory: reallocation on freed pointer"));
         ovp = NIL;
         goto jleave;
      }
   }
#endif /* su_HAVE_MEM_DEBUG */

   if(a_MEMA_DBG( UZ_MAX - a_MEMA_HOPE_SIZE_ADD > sz && )
         LIKELY(!domul || UZ_MAX / no > sz + a_MEMA_HOPE_SIZE_ADD)){
      if(domul)
         sz *= no;
      a_MEMA_DBG( sz += a_MEMA_HOPE_SIZE_ADD; )

      if(UNLIKELY((rv = realloc(ovp, sz)) == NIL))
         su_state_err((su_STATE_ERR_NOMEM |
               (maf & su_MEM_ALLOC_NOMEM_OK ? su_STATE_ERR_PASS : 0) |
               (maf & su_MEM_ALLOC_MUSTFAIL ? su_STATE_ERR_NOPASS : 0)),
            _("SU memory: reallocation request"));
#ifdef su_HAVE_MEM_DEBUG
      else{
         p.p_vp = rv;

         p.p_hc->mhc_prev = NIL;
         if((p.p_hc->mhc_next = a_mema_heap_list) != NIL)
            a_mema_heap_list->mhc_prev = p.p_hc;
         p.p_c->mac_file = mdbg_file;
         p.p_c->mac_line = mdbg_line;
         p.p_c->mac_isfree = FAL0;
         ASSERT(sz - user_sz <= S32_MAX);
         p.p_c->mac_user_off = S(u32,sz - user_sz);
         p.p_c->mac_size = sz;
         a_mema_heap_list = p.p_hc++;
         a_MEMA_HOPE_SET(p_hc, p);

         rv = p.p_vp;

         ++a_mema_heap_aall;
         ++a_mema_heap_acur;
         a_mema_heap_amax = su_MAX(a_mema_heap_amax, a_mema_heap_acur);
         a_mema_heap_mall += user_sz;
         a_mema_heap_mcur += user_sz;
         a_mema_heap_mmax = su_MAX(a_mema_heap_mmax, a_mema_heap_mcur);
      }
jleave:;
#endif /* su_HAVE_MEM_DEBUG */
   }else
      su_state_err((su_STATE_ERR_OVERFLOW |
            (maf & su_MEM_ALLOC_OVERFLOW_OK ? su_STATE_ERR_PASS : 0) |
            (maf & su_MEM_ALLOC_MUSTFAIL ? su_STATE_ERR_NOPASS : 0)),
         _("SU memory: reallocation request"));
   NYD_OU;
   return rv;
}

void *
su_mem_free(void *ovp  su_MEM_DEBUG_ARGS_DECL){
   NYD_IN;
   if(LIKELY(ovp != NIL)){
#ifdef su_HAVE_MEM_DEBUG
      union a_mema_ptr p;
      boole isbad;

      p.p_vp = ovp;
      a_MEMA_HOPE_GET(p_hc, p, isbad);
      --p.p_hc;

      if(p.p_c->mac_isfree){
         su_log(su_LOG_ALERT,
            "SU memory: double-free avoided at %s, line %d\n"
            "\tLast seen: %s, line %" PRIu32 "\n",
            mdbg_file, mdbg_line, p.p_c->mac_file, p.p_c->mac_line);
         goto jleave;
      }

      p.p_c->mac_file = mdbg_file;
      p.p_c->mac_line = mdbg_line;
      p.p_c->mac_isfree = TRU1;
      if(p.p_hc == a_mema_heap_list){
         if((a_mema_heap_list = p.p_hc->mhc_next) != NIL)
            a_mema_heap_list->mhc_prev = NIL;
      }else
         p.p_hc->mhc_prev->mhc_next = p.p_hc->mhc_next;
      if(p.p_hc->mhc_next != NIL)
         p.p_hc->mhc_next->mhc_prev = p.p_hc->mhc_prev;

      su_mem_set(ovp, 0377, p.p_c->mac_size - p.p_c->mac_user_off); /* Trash */

      --a_mema_heap_acur;
      a_mema_heap_mcur -= p.p_c->mac_size - p.p_c->mac_user_off;

      ovp = p.p_vp; /* (..shall we free()) */
      if(su_state_has(su_STATE_DEBUG | su_STATE_MEMORY_DEBUG)){
         p.p_hc->mhc_next = a_mema_heap_free;
         a_mema_heap_free = p.p_hc;
      }else
#endif /* su_HAVE_MEM_DEBUG */
         free(ovp);
   }
#ifdef su_HAVE_MEM_DEBUG
   else
      su_log(su_LOG_INFO, "SU memory: free NIL from %s, line %d\n",
         mdbg_file, mdbg_line);
jleave:;
#endif
   NYD_OU;
}

void
su_mem_reset(void){
#ifdef su_HAVE_MEM_DEBUG
   union a_mema_ptr p;
   uz c, s;
#endif
   NYD_IN;

   su_mem_check();

   /* TODO bring back autorec and lofi */

   /* Now we are ready to deal with heap */
#ifdef su_HAVE_MEM_DEBUG
   c = s = 0;

   for(p.p_hc = a_mema_heap_free; p.p_hc != NIL;){
      void *vp;

      vp = p.p_hc;
      ++c;
      s += p.p_c->mac_size;
      p.p_hc = p.p_hc->mhc_next;
      free(vp);
   }
   a_mema_heap_free = NIL;

   su_log(su_LOG_DEBUG,
      "su_mem_reset(): freed %lu" PRIuZ " chunks/%" PRIuZ " bytes\n",
      c, s);
#endif /* su_HAVE_MEM_DEBUG */
   NYD_OU;
}

#ifdef su_HAVE_MEM_DEBUG
void
su_mem_trace(void){
   /* For a_MEMA_HOPE_GET() */
   char const * const mdbg_file = "su_mem_trace()";
   int const mdbg_line = -1;

   boole isbad;
   union a_mema_ptr p, xp;
   uz lines;
   NYD2_IN;

   lines = 0;

   su_log(su_LOG_DEBUG,
      "\nHeap memory buffers:\n"
      "  Allocation cur/peek/all: %7" PRIuZ "/%7" PRIuZ "/%10" PRIuZ "\n"
      "       Bytes cur/peek/all: %7" PRIuZ "/%7" PRIuZ "/%10" PRIuZ "\n\n",
      a_mema_heap_acur, a_mema_heap_amax, a_mema_heap_aall,
      a_mema_heap_mcur, a_mema_heap_mmax, a_mema_heap_mall);
   lines += 6;

   for(p.p_hc = a_mema_heap_list; p.p_hc != NIL;
         ++lines, p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMA_HOPE_GET_TRACE(p_hc, xp, isbad);
      su_log((isbad ? su_LOG_ALERT : su_LOG_DEBUG),
         "  %s%p (%u bytes): %s, line %u\n",
         (isbad ? "! SU memory: CANARY ERROR (heap): " : ""), xp.p_vp,
         p.p_c->mac_size - p.p_c->mac_user_off,
         p.p_c->mac_file, p.p_c->mac_line);
   }

   su_log(su_LOG_DEBUG, "Freed heap buffers lingering for release:\n");
   ++lines;

   for(p.p_hc = a_mema_heap_free; p.p_hc != NIL;
         ++lines, p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMA_HOPE_GET_TRACE(p_hc, xp, isbad);
      su_log((isbad ? su_LOG_ALERT : su_LOG_DEBUG),
         "  %s%p (%u bytes): %s, line %u\n",
         (isbad ? "! SU memory: CANARY ERROR (free): " : ""), xp.p_vp,
         p.p_c->mac_size - p.p_c->mac_user_off,
         p.p_c->mac_file, p.p_c->mac_line);
   }
   NYD2_OU;
}

void
su_mem_check(char const *mdbg_file, int mdbg_line){
   union a_mema_ptr p, xp;
   boole anybad, isbad;
   NYD2_IN;

   anybad = FAL0;

   for(p.p_hc = a_mema_heap_list; p.p_hc != NIL; p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMA_HOPE_GET_TRACE(p_hc, xp, isbad);
      if(isbad){
         anybad = TRU1;
         su_log(su_LOG_ALERT,
            "! SU memory: CANARY ERROR (heap): %p (%u bytes): %s, line %u\n",
            xp.p_vp, (p.p_c->mac_size - p.p_c->mac_user_off),
            p.p_c->mac_file, p.p_c->mac_line);
      }
   }

   for(p.p_hc = a_mema_heap_free; p.p_hc != NIL; p.p_hc = p.p_hc->mhc_next){
      xp = p;
      ++xp.p_hc;
      a_MEMA_HOPE_GET_TRACE(p_hc, xp, isbad);
      if(isbad){
         anybad = TRU1;
         su_log(su_LOG_ALERT,
            "! SU memory: CANARY ERROR (free): %p (%u bytes): %s, line %u\n",
            xp.p_vp, (p.p_c->mac_size - p.p_c->mac_user_off),
            p.p_c->mac_file, p.p_c->mac_line);
      }
   }

   if(anybad)
      su_log(su_LOG_EMERG, "SU memory: errors encountered");
   NYD2_OU;
}
#endif /* su_HAVE_MEM_DEBUG */

#include "su/code-ou.h"
/* s-it-mode */
