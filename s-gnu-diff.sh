#@ Generate real (whitespace normalized) difference of S-roff and GNU roff.

DIFF='diff -Napruw'
MANUALS=true
LDIRBASE=./ #roff.git/
GDIRBASE=../groff.git/

##  --  >8  --  8<  --  ##

[ ${#} -gt 0 ] && WANT="${*}" || WANT=
LDIR= GDIR=

should() {
   [ -z "${WANT}" ] && return 0
   { i=${WANT}; echo "_${i}_" | grep "_${1}_"; } >/dev/null 2>&1
}

new() {
   if [ -f ${LDIRBASE}${LDIR}/${1} ]; then
      $DIFF ${LDIRBASE}${LDIR}/${1} /dev/null
   else
      echo >&2 '============'
      echo >&2 "new no file=${1} in LDIR=${LDIR}"
      echo >&2 '============'
      exit 1
   fi
}

eqd() {
   if [ -f ${LDIRBASE}${LDIR}/${1} ] &&
         [ -f ${GDIRBASE}${GDIR}/${1} ]; then
      ${DIFF} ${LDIRBASE}${LDIR}/${1} ${GDIRBASE}${GDIR}/${1}
   else
      echo >&2 '============'
      echo >&2 "eqd no file=${1} in LDIR=${LDIR} or GDIR=${GDIR}"
      echo >&2 '============'
      exit 1
   fi
}

ned() {
   if [ -f ${LDIRBASE}${LDIR}/${1} ] &&
         [ -f ${GDIRBASE}${GDIR}/${2} ]; then
      ${DIFF} ${LDIRBASE}${LDIR}/${1} ${GDIRBASE}${GDIR}/${2}
   elif [ -f ${LDIRBASE}${LDIR}/${1} ]; then
      echo >&2 '============'
      echo >&2 "ned no GDIR=${GDIR}/${2}"
      echo >&2 '============'
      exit 1
   else
      echo >&2 '============'
      echo >&2 "ned no LDIR=${LDIR}/${1} (at least)"
      echo >&2 '============'
      exit 1
   fi
}

# include
if should include; then
   LDIR=include GDIR=src/${LDIR}

   new config.h.in
   new file_case.h

   eqd cmap.h
   eqd color.h
   eqd cset.h
   eqd device.h
   eqd driver.h
   eqd errarg.h
   eqd error.h
   eqd font.h
   eqd geometry.h
   eqd getopt.h
   eqd getopt_int.h
   eqd gettext.h
   eqd html-strings.h
   eqd htmlhint.h
   eqd index.h
   eqd itable.h
   eqd lib.h
   eqd localcharset.h
   eqd macropath.h
   eqd nonposix.h
   eqd paper.h
   eqd posix.h
   eqd printer.h
   eqd ptable.h
   eqd refid.h
   eqd relocate.h
   eqd search.h
   eqd searchpath.h
   eqd stringclass.h
   eqd symbol.h
   eqd unicode.h
fi

## src/

LDIR=src/dev-dvi
if should dev-dvi; then
   GDIR=src/devices/grodvi

   new dvi-config.h.in
   eqd dvi.cpp

   if ${MANUALS}; then
      LDIR=man
      ned dev-dvi.1.in grodvi.man
   fi
fi

LDIR=src/dev-html
if should dev-html; then
   GDIR=src/devices/grohtml

   new html-config.h.in
   eqd html-table.cpp
   eqd html-table.h
   eqd html-text.cpp
   eqd html-text.h
   eqd html.h
   eqd output.cpp
   eqd post-html.cpp

   if ${MANUALS}; then
      LDIR=man
      ned dev-html.1.in grohtml.man
   fi
fi

LDIR=src/dev-ps
if should dev-ps; then
   GDIR=src/devices/grops

   new ps-config.h.in
   eqd psrm.cpp
   eqd ps.h
   eqd ps.cpp

   if ${MANUALS}; then
      LDIR=man
      ned dev-ps.1.in grops.man
   fi
fi

LDIR=src/dev-tty
if should dev-tty; then
   GDIR=src/devices/grotty

   new tty-config.h.in
   eqd tty.cpp

   if ${MANUALS}; then
      LDIR=man
      ned dev-tty.1.in grotty.man
   fi
fi

LDIR=src/lib-bib
if should lib-bib; then
   GDIR=src/libs/libbib

   new bib.h
   eqd common.cpp
   eqd index.cpp
   eqd linear.cpp
   eqd map.c
   eqd search.cpp
fi

LDIR=src/lib-driver
if should lib-driver; then
   GDIR=src/libs/libdriver

   eqd input.cpp
   eqd printer.cpp
fi

LDIR=src/lib-roff
if should lib-roff; then
   GDIR=src/libs/libgroff

   eqd change_lf.cpp
   eqd cmap.cpp
   eqd color.cpp
   eqd config.charset
   eqd cset.cpp
   eqd device.cpp
   eqd errarg.cpp
   eqd error.cpp
   eqd fatal.cpp
   new file_case.cpp
   eqd filename.cpp
   eqd font.cpp
   eqd fontfile.cpp
   eqd geometry.cpp
   eqd getopt.c
   eqd getopt1.c
   eqd glyphuni.cpp
   eqd htmlhint.cpp
   eqd hypot.cpp
   eqd iftoa.c
   eqd invalid.cpp
   eqd itoa.c
   eqd lf.cpp
   eqd lineno.cpp
   eqd localcharset.c
   eqd macropath.cpp
   ned make-uniuni.sh make-uniuni
   eqd matherr.c
   eqd maxfilename.cpp
   eqd maxpathname.cpp
   eqd mksdir.cpp
   eqd mkstemp.cpp
   eqd nametoindex.cpp
   eqd new.cpp
   eqd paper.cpp
   eqd prime.cpp
   eqd progname.c
   eqd ptable.cpp
   eqd putenv.c
   eqd quotearg.c
   eqd relocatable.h
   eqd relocate.cpp
   eqd searchpath.cpp
   eqd spawnvp.c
   eqd strcasecmp.c
   eqd string.cpp
   eqd strncasecmp.c
   eqd strsave.cpp
   eqd symbol.cpp
   eqd tmpfile.cpp
   eqd tmpname.cpp
   eqd unicode.cpp
   eqd uniglyph.cpp
   eqd uniuni.cpp
fi

LDIR=src/lib-snprintf
if should lib-snprintf; then
   GDIR=src/libs/snprintf

   eqd snprintf.c
fi

LDIR=src/nroff
if should nroff; then
   GDIR=src/roff/nroff

   ned nroff.sh.in nroff.sh

   if ${MANUALS}; then
      LDIR=man
      ned src-nroff.1.in nroff.man
   fi
fi

LDIR=src/pre-eqn
if should pre-eqn; then
   GDIR=src/preproc/eqn

   new eqn-config.h.in
   eqd box.cpp
   eqd box.h
   eqd delim.cpp
   eqd eqn.h
   ned eqn.y eqn.ypp
   eqd lex.cpp
   eqd limit.cpp
   eqd list.cpp
   eqd main.cpp
   eqd mark.cpp
   ned neqn.sh.in neqn.sh
   eqd other.cpp
   eqd over.cpp
   eqd pbox.h
   eqd pile.cpp
   eqd script.cpp
   eqd special.cpp
   eqd sqrt.cpp
   eqd text.cpp

   if ${MANUALS}; then
      LDIR=man
      ned pre-eqn.1.in eqn.man
      ned pre-neqn.1.in neqn.man
   fi
fi

LDIR=src/pre-grn
if should pre-grn; then
   GDIR=src/preproc/grn

   new grn-config.h.in
   eqd README
   eqd gprint.h
   eqd hdb.cpp
   eqd hgraph.cpp
   eqd hpoint.cpp
   eqd main.cpp

   if ${MANUALS}; then
      LDIR=man
      ned pre-grn.1.in grn.man
   fi
fi

LDIR=src/pre-html
if should pre-html; then
   GDIR=src/preproc/html

   new html-config.h.in
   eqd pre-html.cpp
   eqd pre-html.h
   eqd pushback.cpp
   eqd pushback.h
fi

LDIR=src/pre-pic
if should pre-pic; then
   GDIR=src/preproc/pic

   new pic-config.h.in
   eqd common.cpp
   eqd common.h
   eqd lex.cpp
   eqd main.cpp
   eqd object.cpp
   eqd object.h
   eqd output.h
   eqd pic.h
   ned pic.y pic.ypp
   eqd position.h
   eqd tex.cpp
   eqd text.h
   eqd troff.cpp

   if ${MANUALS}; then
      LDIR=man
      ned pre-pic.1.in pic.man
   fi
fi

LDIR=src/pre-preconv
if should pre-preconv; then
   GDIR=src/preproc/preconv

   new preconv-config.h.in
   eqd preconv.cpp

   if ${MANUALS}; then
      LDIR=man
      ned pre-preconv.1.in preconv.man
   fi
fi

LDIR=src/pre-refer
if should pre-refer; then
   GDIR=src/preproc/refer

   new refer-config.h.in
   eqd command.cpp
   eqd command.h
   ned label.y label.ypp
   eqd ref.cpp
   eqd ref.h
   eqd refer.cpp
   eqd refer.h
   eqd token.cpp
   eqd token.h

   if ${MANUALS}; then
      LDIR=man
      ned pre-refer.1.in refer.man
   fi
fi

LDIR=src/pre-soelim
if should pre-soelim; then
   GDIR=src/preproc/soelim

   new soelim-config.h.in
   eqd soelim.cpp

   if ${MANUALS}; then
      LDIR=man
      ned pre-soelim.1.in soelim.man
   fi
fi

LDIR=src/pre-tbl
if should pre-tbl; then
   GDIR=src/preproc/tbl

   new tbl-config.h.in
   eqd main.cpp
   eqd table.cpp
   eqd table.h

   if ${MANUALS}; then
      LDIR=man
      ned pre-tbl.1.in tbl.man
   fi
fi

LDIR=src/roff
if should roff; then
   GDIR=src/roff/groff

   new roff-config.h
   eqd pipeline.c
   eqd pipeline.h
   ned roff.cpp groff.cpp

   if ${MANUALS}; then
      LDIR=man
      ned src-roff.1.in groff.man
   fi
fi

LDIR=src/troff
if should troff; then
   GDIR=src/roff/troff

   new troff-config.h
   eqd charinfo.h
   eqd column.cpp
   eqd dictionary.cpp
   eqd dictionary.h
   eqd div.cpp
   eqd div.h
   eqd env.cpp
   eqd env.h
   eqd hvunits.h
   eqd input.cpp
   eqd input.h
   eqd mtsm.cpp
   eqd mtsm.h
   eqd node.cpp
   eqd node.h
   eqd number.cpp
   eqd reg.cpp
   eqd reg.h
   eqd request.h
   eqd token.h
   eqd troff.h

   if ${MANUALS}; then
      LDIR=man
      ned src-troff.1.in troff.man
   fi
fi

LDIR=src/ute-addftinfo
if should ute-addftinfo; then
   GDIR=src/utils/addftinfo

   new addftinfo-config.h.in
   eqd addftinfo.cpp
   eqd guess.cpp
   eqd guess.h

   if ${MANUALS}; then
      LDIR=man
      ned ute-addftinfo.1.in addftinfo.man
   fi
fi

LDIR=src/ute-afmtodit
if should ute-afmtodit; then
   GDIR=src/utils/afmtodit

   ned afmtodit.pl.in afmtodit.pl
   eqd afmtodit.tables
   eqd make-afmtodit-tables

   if ${MANUALS}; then
      LDIR=man
      ned ute-afmtodit.1.in afmtodit.man
   fi
fi

LDIR=src/ute-indxbib
if should ute-indxbib; then
   GDIR=src/utils/indxbib

   new indxbib-config.h.in
   eqd eign
   eqd indxbib.cpp

   if ${MANUALS}; then
      LDIR=man
      ned ute-indxbib.1.in indxbib.man
   fi
fi

LDIR=src/ute-lkbib
if should ute-lkbib; then
   GDIR=src/utils/lkbib

   new lkbib-config.h.in
   eqd lkbib.cpp

   if ${MANUALS}; then
      LDIR=man
      ned ute-lkbib.1.in lkbib.man
   fi
fi

LDIR=src/ute-lookbib
if should ute-lookbib; then
   GDIR=src/utils/lookbib

   new lookbib-config.h
   eqd lookbib.cpp

   if ${MANUALS}; then
      LDIR=man
      ned ute-lookbib.1.in lookbib.man
   fi
fi

LDIR=src/ute-pfbtops
if should ute-pfbtops; then
   GDIR=src/utils/pfbtops

   new pfbtops-config.h
   eqd pfbtops.c

   if ${MANUALS}; then
      LDIR=man
      ned ute-pfbtops.1.in pfbtops.man
   fi
fi

LDIR=src/ute-tfmtodit
if should ute-tfmtodit; then
   GDIR=src/utils/tfmtodit

   new tfmtodit-config.h
   eqd tfmtodit.cpp

   if ${MANUALS}; then
      LDIR=man
      ned ute-tfmtodit.1.in tfmtodit.man
   fi
fi

## font/

if should font; then
   LDIR=font GDIR=${LDIR}/util
   eqd make-Rproto

   LDIR=font/dev-ascii GDIR=font/devascii
   ned DESC.proto.in DESC.proto
   eqd R.proto

   LDIR=font/dev-cp1047 GDIR=font/devcp1047
   ned DESC.proto.in DESC.proto
   eqd R.proto

   LDIR=font/dev-dvi GDIR=font/devdvi
   eqd CW
   eqd CWEC
   eqd CWI
   eqd CWIEC
   eqd CWITC
   eqd CWTC
   eqd DESC.in
   eqd EX
   eqd HB
   eqd HBEC
   eqd HBI
   eqd HBIEC
   eqd HBITC
   eqd HBTC
   eqd HI
   eqd HIEC
   eqd HITC
   eqd HR
   eqd HREC
   eqd HRTC
   eqd MI
   eqd S
   eqd SA
   eqd SB
   eqd SC
   eqd TB
   eqd TBEC
   eqd TBI
   eqd TBIEC
   eqd TBITC
   eqd TBTC
   eqd TI
   eqd TIEC
   eqd TITC
   eqd TR
   eqd TREC
   eqd TRTC

   LDIR=font/dev-dvi/generate GDIR=font/devdvi/generate
   ned compile.sh CompileFonts
   eqd ec.map
   eqd msam.map
   eqd msbm.map
   eqd tc.map
   eqd texb.map
   eqd texex.map
   eqd texi.map
   eqd texmi.map
   eqd texr.map
   eqd texsy.map
   eqd textex.map
   eqd textt.map

   LDIR=font/dev-html GDIR=font/devhtml
   ned DESC.proto.in DESC.proto
   eqd R.in
   eqd R.proto

   LDIR=font/dev-latin1 GDIR=font/devlatin1
   ned DESC.proto.in DESC.proto
   eqd R.proto

   LDIR=font/dev-ps GDIR=font/devps
   eqd AB
   eqd ABI
   eqd AI
   eqd AR
   eqd BMB
   eqd BMBI
   eqd BMI
   eqd BMR
   eqd CB
   eqd CBI
   eqd CI
   eqd CR
   eqd DESC.in
   eqd EURO
   eqd HB
   eqd HBI
   eqd HI
   eqd HNB
   eqd HNBI
   eqd HNI
   eqd HNR
   eqd HR
   eqd NB
   eqd NBI
   eqd NI
   eqd NR
   eqd PB
   eqd PBI
   eqd PI
   eqd PR
   eqd S
   eqd SS
   eqd TB
   eqd TBI
   eqd TI
   eqd TR
   eqd ZCMI
   eqd ZD
   eqd ZDR
   eqd download
   eqd freeeuro.afm
   eqd freeeuro.pfa
   eqd prologue.ps
   eqd psstrip.sed
   eqd symbol.afm
   eqd symbolmap
   eqd symbolsl.ps
   eqd text.enc
   eqd zapfdr.afm
   eqd zapfdr.ps

   LDIR=font/dev-ps/generate GDIR=font/devps/generate
   eqd afmname
   eqd dingbats.map
   eqd dingbats.rmap
   eqd freeeuro.sfd
   eqd lgreekmap
   eqd make-zapfdr
   eqd sfdtopfa.pe
   eqd symbol.sed
   eqd symbolchars
   eqd symbolsl.afm
   eqd symbolsl.awk
   eqd textmap
   eqd zapfdr.sed

   LDIR=font/dev-utf8 GDIR=font/devutf8
   ned DESC.proto.in DESC.proto
   eqd NOTES
   eqd R.in
   eqd R.proto
fi

# tmac
if should tmac; then
   LDIR=tmac GDIR=${LDIR}

   eqd 62bit.tmac
   eqd LOCALIZATION
   eqd Xps.tmac
   eqd a4.tmac
   eqd an-ext.tmac
   eqd an-old.tmac
   eqd an.tmac
   eqd andoc.tmac
   new ascii.tmac
   eqd composite.tmac
   eqd cp1047.tmac
   eqd cs.tmac
   eqd de.tmac
   eqd den.tmac
   eqd devtag.tmac
   ned doc.tmac doc.tmac-u
   eqd dvi.tmac
   ned e.tmac e.tmac-u
   eqd ec.tmac
   eqd eqnrc
   eqd europs.tmac
   eqd fallbacks.tmac
   eqd fr.tmac
   eqd html-end.tmac
   eqd html.tmac
   eqd hyphen.cs
   eqd hyphen.den
   eqd hyphen.det
   eqd hyphen.fr
   eqd hyphen.sv
   eqd hyphen.us
   eqd hyphenex.cs
   eqd hyphenex.det
   eqd hyphenex.pl
   eqd hyphenex.us
   eqd latin1.tmac
   eqd latin2.tmac
   eqd latin5.tmac
   eqd latin9.tmac
   eqd man.local
   eqd man.tmac
   eqd mandoc.tmac
   ned mdoc-common doc-common-u
   ned mdoc-ditroff doc-ditroff-u
   ned mdoc-nroff doc-nroff-u
   ned mdoc-syms doc-syms-u
   eqd mdoc.local
   eqd mdoc.tmac
   eqd me.tmac
   eqd ms.tmac
   eqd papersize.tmac
   eqd pic.tmac
   eqd ps.tmac
   eqd psfig.tmac
   eqd psold.tmac
   eqd pspic.tmac
   eqd ptx.tmac
   eqd refer-me.tmac
   eqd refer-ms.tmac
   eqd refer.tmac
   eqd s.tmac
   eqd safer.tmac
   eqd sv.tmac
   eqd trace.tmac
   eqd trans.tmac
   eqd troffrc
   eqd troffrc-end
   eqd tty-char.tmac
   eqd tty.tmac
   eqd unicode.tmac
   ned www.tmac www.tmac.in

   if ${MANUALS}; then
      LDIR=man
      ned tmac-man.7.in groff_man.man
      ned tmac-mdoc.7.in groff_mdoc.man
      ned tmac-me.7.in groff_me.man
      ned tmac-ms.7.in groff_ms.man
      ned tmac-trace.7.in groff_trace.man
      ned tmac-www.7.in groff_www.man
   fi
fi

# man (only the remains not handled by the others)
if should man && ${MANUALS}; then
   LDIR=man GDIR=${LDIR}

   ned l_ditroff.7.in ditroff.man
   ned l_roff.7.in groff.man
   ned l_roff-char.7.in groff_char.man
   ned l_roff-diff.7.in groff_diff.man
   ned l_roff-font.5.in groff_font.man
   ned l_roff-history.7.in roff.man
   ned l_roff-out.5.in groff_out.man
   ned l_roff-tmac.5.in groff_tmac.man
fi

# doc
if should doc && ${MANUALS}; then
   LDIR=doc GDIR=${LDIR}

   eqd grnexmpl.g
   eqd grnexmpl.me
   #ned groff.texinfo groff.texi
   ned meintro.me.in meintro.me
   ned meref.me.in meref.me
   eqd pic.ms
fi

## contrib/

# hdtbl
if should hdtbl; then
   LDIR=contrib/hdtbl GDIR=${LDIR}

   ned hdtbl.tmac hdtbl.tmac-u
   ned hdmisc.tmac hdmisc.tmac-u
   if ${MANUALS}; then
      ned hdtbl.7.in groff_hdtbl.man
   fi

   LDIR=${LDIR}/examples GDIR=${GDIR}/examples

   eqd chess_board.roff
   eqd col_rowspan_colors.roff
   eqd color_boxes.roff
   eqd color_nested_tables.roff
   eqd color_table_cells.roff
   eqd color_transitions.roff
   ned common.tmac common.roff
   eqd fonts_n.in
   eqd fonts_x.in
   ned mixed_pickles.roff.in mixed_pickles.roff
   eqd rainbow.roff
   eqd short_reference.roff
fi

# pdfmark
if should pdfmark; then
   LDIR=contrib/pdfmark GDIR=${LDIR}

   eqd cover.ms
   ned pdfmark.ms.in pdfmark.ms
   eqd pdfmark.tmac
   ned pdfroff.sh.in pdfroff.sh
   eqd spdf.tmac
   if ${MANUALS}; then
      ned pdfroff.1.in pdfroff.man
   fi
fi

# s-sh-mode
