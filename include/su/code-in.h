/*@ Internal: opposite of code-ou.h.
 *
 * Copyright (c) 2003 - 2018 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
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
#ifndef su_CODE_H
# error su/code-in.h must be included after su/code.h
#endif
#ifdef su_CODE_IN_H
# error su/code-ou.h must be included before including su/code-in.h again
#endif
#define su_CODE_IN_H

/* LANG */

#undef C_LANG
#undef C_DECL_BEGIN
#undef C_DECL_END
#define C_LANG su_C_LANG
#define C_DECL_BEGIN su_C_DECL_BEGIN
#define C_DECL_END su_C_DECL_END
#define NSPC_BEGIN su_NSPC_BEGIN
#define NSPC_END su_NSPC_END
#define NSPC_USE su_NSPC_USE
#defien NSPC su_NSPC

#define CLASS_NO_COPY su_CLASS_NO_COPY

#define PUB su_PUB
#define PRO su_PRO
#define PRI su_PRI
#define STA su_STA
#define VIR su_VIR
#define OVW su_OVW
#define OVR su_OVR

#define S su_S
#define R su_R
#define C su_C

#define NIL su_NIL

#define SHADOW su_SHADOW

#ifdef su_HEADER
# ifdef su_SOURCE
#  define EXPORT su_EXPORT
#  define EXPORT_DATA su_EXPORT_DATA
#  define IMPORT su_IMPORT
#  define IMPORT_DATA su_IMPORT_DATA
# endif
#elif defined rf_HEADER
# ifdef rf_SOURCE
#  define EXPORT su_EXPORT
#  define EXPORT_DATA su_EXPORT_DATA
#  define IMPORT su_IMPORT
#  define IMPORT_DATA su_IMPORT_DATA
# endif
#endif
#ifndef EXPORT
# define EXPORT su_IMPORT
# define EXPORT_DATA su_IMPORT_DATA
# define IMPORT su_IMPORT
# define IMPORT_DATA su_IMPORT_DATA
#endif

#define CTA su_CTA
#define LCTA su_LCTA
#define CTAV su_CTAV
#define LCTAV su_LCTAV
#define MCTA su_MCTA

/* CC */

#define PACKED su_PACKED

#define INLINE su_INLINE

#define LIKELY su_LIKELY
#define UNLIKELY su_UNLIKELY

/* SUPPORT MACROS+ */

#undef ABS
#undef CLIP
#undef MAX
#undef MIN
#undef IS_POW2
#define ABS su_ABS
#define CLIP su_CLIP
#define MAX su_MAX
#define MIN su_MIN
#define IS_POW2 su_IS_POW2

#define ALIGN su_ALIGN
#define ALIGN_SMALL su_ALIGN_SMALL

/* ASSERT series */
#if defined NDEBUG || !defined su_HAVE_DEBUG
# define ASSERT(X) do{}while(0)
# define ASSERT_EXEC(X,S) do{}while(0)
# define ASSERT_JUMP(X,L) do{}while(0)
# define ASSERT_RET(X,Y) do{}while(0)
# define ASSERT_NYD_RET(X,Y) do{}while(0)
#else
# define ASSERT(X) \
do if(!(X))\
   su_assert(STRINGIFY(X), __FILE__, __LINE__, su_FUN, TRU1);\
while(0)
# define ASSERT_EXEC(X,S) \
do if(!(X)){\
   su_assert(STRINGIFY(X), __FILE__, __LINE__, su_FUN, FAL0);\
   S;\
}while(0)
# define ASSERT_JUMP(X,L) \
do if(!(X)){\
   su_assert(STRINGIFY(X), __FILE__, __LINE__, su_FUN, FAL0);\
   goto L;\
}while(0)
# define ASSERT_RET(X,Y) \
do if(!(X)){\
   su_assert(STRINGIFY(X), __FILE__, __LINE__, su_FUN, FAL0);\
   return Y;\
}while(0)
# define ASSERT_NYD_RET(X,Y) \
do if(!(X)){\
   su_assert(STRINGIFY(X), __FILE__, __LINE__, su_FUN, FAL0);\
   Y; goto j__su_nydou;\
}while(0)
#endif /* defined NDEBUG || !defined su_HAVE_DEBUG */

#define BITENUM_MASK su_BITENUM_MASK

#define EMPTY_FILE su_EMPTY_FILE

#define DBG su_DBG
#define NDGB su_NDBG
#define DBGOR su_DBGOR

#define FIELD_INITN su_FIELD_INITN
#define FIELD_INITI su_FIELD_INITI

#define NELEM su_NELEM

/* Not-Yet-Dead macros TODO stubs */
#if defined NDEBUG || (!defined su_HAVE_DEBUG && !defined su_HAVE_DEVEL)
# undef NYD
# undef NYD2
#elif defined su_HAVE_DEVEL
# define NYD_IN if(1){
# define NYD_OU goto j__su_nydou;j__su_nydou:;}else{}
# define NYD if(0){}else{}
# ifdef NYD2
#  undef NYD2
#  define NYD2_IN if(1){
#  define NYD2_OU goto j__su_nydou;j__su_nydou:;}else{}
#  define NYD2 if(0){}else{}
# endif
#else
# define NYD_IN do{
# define NYD_OU goto j__su_nydou;j__su_nydou:;}while(0)
# define NYD do{}while(0)
# ifdef NYD2
#  undef NYD2
#  define NYD2_IN do{
#  define NYD2_OU goto j__su_nydou;j__su_nydou:;}while(0)
#  define NYD2 do{}while(0)
# endif
#endif
/**/
#ifndef NYD
# define NYD_IN do{
# define NYD_OU goto j__su_nydou;j__su_nydou:;}while(0)
# define NYD do{}while(0)
#endif
#ifndef NYD2
# define NYD2_IN do{
# define NYD2_OU goto j__su_nydou;j__su_nydou:;}while(0)
# define NYD2 do{}while(0)
#endif

#define PTR2UZ su_PTR2UZ

#define PTRCMP su_PTRCMP

/* Translation: may NOT set errno! */
#undef _
#undef N_
#undef V_
#define _(S) S
#define N_(S) S
#define V_(S) S

#define UCMP su_UCMP

#define UNCONST su_UNCONST
#define UNVOLATILE su_UNVOLATILE
#define UNALIGN su_UNALIGN

#define UNINIT su_UNINIT

#define UNUSED su_UNUSED

#define VFIELD_SIZE su_VFIELD_SIZE
#define VSTRUCT_SIZEOF su_VSTRUCT_SIZEOF

/* POD TYPE SUPPORT (only if !C++) */
#if C_LANG

# define ul su_ul
# define ui su_ui
# define us su_us
# define uc su_uc

# define sl su_sl
# define si su_si
# define ss su_ss
# define sc su_sc

# define U8_MAX su_U8_MAX
# define S8_MIN su_S8_MIN
# define S8_MAX su_S8_MAX
# define u8 su_u8
# define s8 su_s8

# define U16_MAX su_U16_MAX
# define S16_MIN su_S16_MIN
# define S16_MAX su_S16_MAX
# define u16 su_u16
# define s16 su_s16

# define U32_MAX su_U32_MAX
# define S32_MIN su_S32_MIN
# define S32_MAX su_S32_MAX
# define u32 su_u32
# define s32 su_s32

# define U64_MAX su_U64_MAX
# define S64_MIN su_S64_MIN
# define S64_MAX su_S64_MAX
# define U64_C su_U64_C
# define S64_C su_S64_C
# define u64 su_u64
# define s64 su_s64

# define UZ_MAX su_UZ_MAX
# define SZ_MIN su_SZ_MIN
# define SZ_MAX su_SZ_MAX
# define UZ_BITS su_UZ_BITS
# define uz su_uz
# define sz su_sz

# define up su_up
# define sp su_sp

# define FAL0 su_FAL0
# define TRU1 su_TRU1
# define TRUM1 su_TRUM1
# define boole su_boole

#endif /* C_LANG */

/* MEMORY */

#define su_ALLOC su_MEM_ALLOC
#define su_ALLOC_N su_MEM_ALLOC_N
#define su_CALLOC su_MEM_CALLOC
#define su_CALLOC_N su_MEM_CALLOC_N
#define su_REALLOC su_MEM_REALLOC
#define su_REALLOC_n su_MEM_REALLOC_N
#define su_TALLOC su_MEM_TALLOC
#define su_TCALLOC su_MEM_TCALLOC
#define su_TREALLOC su_MEM_TREALLOC
#define su_FREE su_MEM_FREE
#if !C_LANG
# define su_NEW su_MEM_NEW
# define su_CNEW su_MEM_CNEW
# define su_NEW_HEAP su_MEM_NEW_HEAP
# define su_DEL su_MEM_DEL
# define su_DEL_HEAP su_MEM_DEL_HEAP
# define su_DEL_PRIVATE su_MEM_DEL_PRIVATE
# define su_DEL_HEAP_PRIVATE su_MEM_DEL_HEAP_PRIVATE
#endif

/* s-it-mode */
