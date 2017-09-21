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
#ifndef su_PRIMARY_H
# error su/code-in.h must be included after su/primary.h
#endif
#ifdef su_CODE_IN_H
# error su/code-ou.h must be included before including su/code-in.h again
#endif
#define su_CODE_IN_H

/* LANG */

#undef C_LANG
#undef C_DECL_BEGIN
#undef c_DECL_END
#undef c_decl
#define C_LANG su_C_LANG
#define C_DECL_BEGIN su_C_DECL_BEGIN
#define C_DECL_END su_C_DECL_END
#define c_decl su_c_decl
#define NSPC_BEGIN su_NSPC_BEGIN
#define NSPC_END su_NSPC_END
#define NSPC_USE su_NSPC_USE

#define CLASS_NO_COPY su_CLASS_NO_COPY

#define pub su_public
#define pro su_protected
#define pri su_private
#define sta su_static
#define vir su_virtual

#define S su_S
#define R su_R
#define C su_C

#define CTA su_CTA
#define LCTA su_LCTA
#define CTAV su_CTAV
#define LCTAV su_LCTAV
#define MCTA su_MCTA

#define NIL su_NIL

/* CC */

#define INLINE su_INLINE

#define LIKELY su_LIKELY
#define UNLIKELY su_UNLIKELY

/* Support */

#undef ABS
#undef CLIP
#undef MAX
#undef MIN
#undef ISPOW2
#define ABS su_ABS
#define CLIP su_CLIP
#define MAX su_MAX
#define MIN su_MIN
#define ISPOW2 su_ISPOW2

#define ALIGN su_ALIGN
#define ALIGN_SMALL su_ALIGN_SMALL

/* ASSERT is last! */

#define BITENUM_MASK su_BITENUM_MASK

#define EMPTY_FILE su_EMPTY_FILE

#define DBG su_DBG
#define NDGB su_NDBG
#define DBGOR su_DBGOR

#define FIELD_INITN su_FIELD_INITN
#define FIELD_INITI su_FIELD_INITI

#define NELEM su_NELEM

/* NYD at bottom */

#define PTR2SIZE su_PTR2SIZE

#define PTRCMP su_PTRCMP

/* Translation: may NOT set errno! */
#undef _
#undef N_
#undef V_
#define _(S) S
#define N_(S) S
#define V_(S) S

#define UICMP su_UICMP

#define UNCONST su_UNCONST
#define UNVOLATILE su_UNVOLATILE
#define UNALIGN su_UNALIGN

#define UNINIT su_UNINIT

#define UNUSED su_UNUSED

#define VFIELD_SIZE su_VFIELD_SIZE
#define VSTRUCT_SIZEOF su_VSTRUCT_SIZEOF

/* TYPES */

#define UI8_MAX su_UI8_MAX
#define SI8_MIN su_SI8_MIN
#define SI8_MAX su_SI8_MAX
#define ui8 su_ui8
#define si8 su_si8

#define FAL0 su_FAL0
#define TRU1 su_TRU1
#define TRUM1 su_TRUM1
#define boole su_boole

#define UI16_MAX su_UI16_MAX
#define SI16_MIN su_SI16_MIN
#define SI16_MAX su_SI16_MAX
#define ui16 su_ui16
#define si16 su_si16

#define UI32_MAX su_UI32_MAX
#define SI32_MIN su_SI32_MIN
#define SI32_MAX su_SI32_MAX
#define ui32 su_ui32
#define si32 su_si32

#define UI64_MAX su_UI64_MAX
#define SI64_MIN su_SI64_MIN
#define SI64_MAX su_SI64_MAX
#define ui64 su_ui64
#define si64 su_si64

#define uiz su_uiz
#define siz su_siz

#define uip su_uip
#define sip su_sip

/* PROBLEMS */

/* Not-Yet-Dead macros TODO stub */
#ifndef NYD
# ifdef su_HAVE_DEVEL
#  define NYD_IN if(1){
#  define NYD_OU ;}else{}
#  define NYD if(0){}else{}
# else
#  define NYD_IN do{
#  define NYD_OU ;}while(0)
#  define NYD do{}while(0)
# endif
#endif
#ifndef NYD2
# ifdef su_HAVE_DEVEL
#  define NYD2_IN if(1){
#  define NYD2_OU ;}else{}
#  define NYD2 if(0){}else{}
# else
#  define NYD2_IN do{
#  define NYD2_OU ;}while(0)
#  define NYD2 do{}while(0)
# endif
#endif

/* The problem with ASSERT is that primary.h must be fully included first */
#ifdef su_ASSERT_H
# define ASSERT(X) do {if(!(X)) assert(0);} while(0)
# define ASSERT_EXEC(X,S) do {if(!(X)) S;} while(0)
# define ASSERT_JUMP(X,L) do {if(!(X)) goto L;} while(0)
# define ASSERT_NYD_RET(X) do {if(!(X)) {NYD_OU; return;}} while(0)
# define ASSERT_NYD_RET_THIS(X) do {if(!(X)) {NYD_OU; return *this;}} while(0)
# define ASSERT_NYD_RET_SELF(X) do {if(!(X)) {NYD_OU; return self;}} while(0)
# define ASSERT_NYD_RET_VAL(X,V) do {if(!(X)) {NYD_OU; return V;}} while(0)
# define ASSERT_RET(X) do {if(!(X)) return;} while(0)
# define ASSERT_RET_THIS(X) do {if(!(X)) return *this;} while(0)
# define ASSERT_RET_SELF(X) do {if(!(X)) return self;} while(0)
# define ASSERT_RET_VAL(X,V) do {if(!(X)) return V;} while(0)
#elif !defined su_HAVE_DEBUG || defined NDEBUG ||\
      !defined su_CODE_IN_H_EVER_SEEN
# define ASSERT(X) do{}while(0)
# define ASSERT_EXEC(X,S) do{}while(0)
# define ASSERT_JUMP(X,L) do{}while(0)
# define ASSERT_NYD_RET(X) do{}while(0)
# define ASSERT_NYD_RET_THIS(X) do{}while(0)
# define ASSERT_NYD_RET_SELF(X) do{}while(0)
# define ASSERT_NYD_RET_VAL(X,V) do{}while(0)
# define ASSERT_RET(X) do{}while(0)
# define ASSERT_RET_THIS(X) do{}while(0)
# define ASSERT_RET_SELF(X) do{}while(0)
# define ASSERT_RET_VAL(X,V) do{}while(0)
#else
# include <su/code-ou.h>
# include <su/assert.h>
# include <su/code-in.h>
#endif

#ifndef su_CODE_IN_H_EVER_SEEN
# define su_CODE_IN_H_EVER_SEEN
#endif
/* s-it-mode */
