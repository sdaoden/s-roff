/*@ Internal: opposite of code-in.h.
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
#ifndef su_CODE_IN_H
# error su/code-ou.h is useless if su/code-in.h has not been included
#endif
#undef su_CODE_IN_H

#undef su_HEADER
#undef rf_HEADER

/* LANG */

#undef C_LANG
#undef C_DECL_BEGIN
#undef C_DECL_END
#undef NSPC_BEGIN
#undef NSPC_END
#undef NSPC_USE
#undef NSPC

#undef CLASS_NO_COPY

#undef PUB
#undef PRO
#undef PRI
#undef STA
#undef VIR
#undef OVW
#undef OVR

#undef S
#undef R
#undef C

#undef NIL

#undef SHADOW

#undef EXPORT
#undef EXPORT_DATA
#undef IMPORT
#undef IMPORT_DATA

#undef CTA
#undef LCTA
#undef CTAV
#undef LCTAV
#undef MCTA

/* CC */

#undef PACKED

#undef INLINE

#undef LIKELY
#undef UNLIKELY

/* SUPPORT MACROS+ */

#undef ABS
#undef CLIP
#undef MAX
#undef MIN
#undef IS_POW2

#undef ALIGN
#undef ALIGN_SMALL

#undef ASSERT
#undef ASSERT_EXEC
#undef ASSERT_JUMP
#undef ASSERT_RET
#undef ASSERT_NYD_RET

#undef BITENUM_MASK

#undef DBG
#undef NDGB
#undef DBGOR

#undef EMPTY_FILE

#undef FIELD_INITN
#undef FIELD_INITI

#undef NELEM

#undef NYD_IN
#undef NYD_OU
#undef NYD
#undef NYD2_IN
#undef NYD2_OU
#undef NYD2

#undef PTR2UZ

#undef PTRCMP

#undef _
#undef N_
#undef V_

#undef UCMP

#undef UNCONST
#undef UNVOLATILE
#undef UNALIGN

#undef UNINIT

#undef UNUSED

#undef VFIELD_SIZE
#undef VSTRUCT_SIZEOF

/* POD TYPE SUPPORT (only if !C++) */

#if C_LANG
# undef ul
# undef ui
# undef us
# undef uc

# undef sl
# undef si
# undef ss
# undef sc

# undef U8_MAX
# undef S8_MIN
# undef S8_MAX
# undef u8
# undef s8

# undef U16_MAX
# undef S16_MIN
# undef S16_MAX
# undef u16
# undef s16

# undef U32_MAX
# undef S32_MIN
# undef S32_MAX
# undef u32
# undef s32

# undef U64_MAX
# undef S64_MIN
# undef S64_MAX
# undef U64_C
# undef S64_C
# undef u64
# undef s64

# undef UZ_MAX
# undef SZ_MIN
# undef SZ_MAX
# undef UZ_BITS
# undef uz
# undef sz

# undef up
# undef sp

# undef FAL0
# undef TRU1
# undef TRUM1
# undef boole
#endif /* C_LANG */

/* MEMORY */

#undef su_ALLOC
#undef su_NALLOC
#undef su_CALLOC
#undef su_NCALLOC
#undef su_REALLOC
#undef su_NREALLOC
#undef su_TALLOC
#undef su_TCALLOC
#undef su_TREALLOC
#undef su_FREE
#if !C_LANG
# undef su_NEW
# undef su_CNEW
# undef su_NEW_HEAP
# undef su_DEL
# undef su_DEL_HEAP
# undef su_DEL_PRIVATE
# undef su_DEL_HEAP_PRIVATE
#endif

/* s-it-mode */
