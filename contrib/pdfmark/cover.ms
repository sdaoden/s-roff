.\"@
.\" Copyright (c) 2014 - 2017 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
.\"
.\" Copyright 2004
.\"   Free Software Foundation, Inc.
.\"
.\" Copying and distribution of this file, with or without modification,
.\" are permitted in any medium without royalty provided the copyright
.\" notice and this notice are preserved.
.\"
.de CS
.if !rCO .nr CO 0
.if !rTL .nr TL 0
.\".nr PO*SAVED \\n[PO]
.nr LL*SAVED \\n[LL]
.nr HM*SAVED \\n[HM]
.nr HM 0
.nr PO (2.1c+\\n[CO]u)
.nr LL 17.1c
\&
.nr PS*SAVED \\n[PS]
.nr VS*SAVED \\n[VS]
.nr PS 24
.nr VS 30
.CD
.fam T
.sp |(5.9c+\\n[TL]u)
.als AU au@first
..
.de au@first
.sp 1.5v
.als AU au@next
.AU \\$@
..
.de au@next
.DE
.nr PS 18
.nr VS 18
.CD
.sp 0.5v
\\$*
..
.de AI
\H'-4z'\\$*\H'0'
..
.de CE
.DE
.sp |17.5c
.PSPIC logo.eps
.nr PS 19
.CD
.DE
.\".nr PO \\n[PO*SAVED]
.nr LL \\n[LL*SAVED]
.nr PS \\n[PS*SAVED]
.nr VS \\n[VS*SAVED]
.nr HM \\n[HM*SAVED]
.\".rr PO*SAVED
.rr LL*SAVED
.rr PS*SAVED
.rr VS*SAVED
.rr HM*SAVED
.fam
..
.\" s-ts-mode
