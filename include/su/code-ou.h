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

/* LANG */

#undef C_LANG
#undef C_DECL_BEGIN
#undef c_DECL_END
#undef c_decl
#undef NSPC_BEGIN
#undef NSPC_END
#undef NSPC_USE

#undef CLASS_NO_COPY

#undef pub
#undef pro
#undef pri
#undef sta
#undef vir

#undef S
#undef R
#undef C

#undef CTA
#undef LCTA
#undef CTAV
#undef LCTAV
#undef MCTA

#undef NIL

/* CC */

#undef INLINE

#undef LIKELY
#undef UNLIKELY

/* Support */

#undef ABS
#undef CLIP
#undef MAX
#undef MIN
#undef ISPOW2

#undef ALIGN
#undef ALIGN_SMALL

#undef BITENUM_MASK

#undef DBG
#undef NDGB
#undef DBGOR

#undef EMPTY_FILE

#undef FIELD_INITN
#undef FIELD_INITI

#undef NELEM

#undef PTR2SIZE

#undef PTRCMP

#undef _
#undef N_
#undef V_

#undef UICMP

#undef UNCONST
#undef UNVOLATILE
#undef UNALIGN

#undef UNINIT

#undef UNUSED

#undef VFIELD_SIZE
#undef VSTRUCT_SIZEOF

/* TYPES */

#undef UI8_MAX
#undef SI8_MIN
#undef SI8_MAX
#undef ui8
#undef si8

#undef FAL0
#undef TRU1
#undef TRUM1
#undef boole

#undef UI16_MAX
#undef SI16_MIN
#undef SI16_MAX
#undef ui16
#undef si16

#undef UI32_MAX
#undef SI32_MIN
#undef SI32_MAX
#undef ui32
#undef si32

#undef UI64_MAX
#undef SI64_MIN
#undef SI64_MAX
#undef ui64
#undef si64

#undef uiz
#undef siz

#undef uip
#undef sip

/* PROBLEMS */

#undef NYD_IN
#undef NYD_OU
#undef NYD
#undef NYD2_IN
#undef NYD2_OU
#undef NYD2

#undef ASSERT
#undef ASSERT_EXEC
#undef ASSERT_JUMP
#undef ASSERT_NYD_RET
#undef ASSERT_NYD_RET_THIS
#undef ASSERT_NYD_RET_SELF
#undef ASSERT_NYD_RET_VAL
#undef ASSERT_RET
#undef ASSERT_RET_THIS
#undef ASSERT_RET_SELF
#undef ASSERT_RET_VAL

/* s-it-mode */
