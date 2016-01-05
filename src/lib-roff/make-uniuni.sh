#!/bin/sh
#@ make-uniuni.sh -- script for creating the file uniuni.cpp
#@ Synopsis:
#   $ make-uniuni.sh <version-string> < UnicodeData.txt > uniuni.cpp
# `UnicodeData.txt' is the central database file from the Unicode standard.
# Unfortunately, it doesn't contain a version number which must be thus
# provided manually as a parameter to the filter.
# This program needs a C preprocessor.
#
# Copyright (c) 2014 - 2015 Steffen (Daode) Nurpmeso <steffen@sdaoden.eu>.
#
# Copyright (C) 2005, 2006
# Free Software Foundation, Inc.
#      Written by Werner Lemberg <wl@gnu.org>
#
# This is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2, or (at your option) any later
# version.
#
# This is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with groff; see the file COPYING.  If not, write to the Free Software
# Foundation, 51 Franklin St - Fifth Floor, Boston, MA 02110-1301, USA.

CPP=cpp

prog="$0"

if test $# -ne 1; then
  echo "usage: $0 <version-string> < UnicodeData.txt > uniuni.cpp"
  exit 1
fi

version_string="$1"

# Remove ranges and control characters,
# then extract the decomposition field,
# then remove lines without decomposition,
# then remove all compatibility decompositions.
sed -e '/^[^;]*;</d' \
| sed -e 's/;[^;]*;[^;]*;[^;]*;[^;]*;\([^;]*\);.*$/;\1/' \
| sed -e '/^[^;]*;$/d' \
| sed -e '/^[^;]*;</d' > $$1

# Prepare input for running cpp.
cat $$1 \
| sed -e 's/^\([^;]*\);/#define \1 /' \
      -e 's/ / u/g' > $$2
cat $$1 \
| sed -e 's/^\([^;]*\);.*$/\1 u\1/' >> $$2

# Run C preprocessor to recursively decompose.
$CPP $$2 $$3

# Convert it back to original format.
cat $$3 \
| sed -e '/#/d' \
      -e '/^$/d' \
      -e 's/ \+/ /g' \
      -e 's/ *$//' \
      -e 's/u//g' \
      -e 's/^\([^ ]*\) /\1;/' > $$4

# Write preamble.
cat <<END
/* Copyright (C) 2002 - 2005
 * Free Software Foundation, Inc.
 * Written by Werner Lemberg <wl@gnu.org> */

// This code has been algorithmically derived from the file
// UnicodeData.txt, version $version_string, available from unicode.org,
// on `date '+%Y-%m-%d'`.

#include "config.h"

#include "lib.h"
#include "ptable.h"
#include "stringclass.h"
#include "unicode.h"

struct unicode_decompose {
  char *value;
};

declare_ptable(unicode_decompose)
implement_ptable(unicode_decompose)

PTABLE(unicode_decompose) unicode_decompose_table;

// the first digit in the composite string gives the number of composites

struct S {
  const char *key;
  const char *value;
} unicode_decompose_list[] = { // FIXME const
END

# Emit Unicode data.
cat $$4 \
| sed -e 's/ /_/g' \
      -e 's/\(.*\);\(.*_.*_.*_.*\)$/  { "\1", "4\2" },/' \
      -e 's/\(.*\);\(.*_.*_.*\)$/  { "\1", "3\2" },/' \
      -e 's/\(.*\);\(.*_.*\)$/  { "\1", "2\2" },/' \
      -e 's/\(.*\);\(.*\)$/  { "\1", "1\2" },/'

# Write postamble.
cat <<END
};

// global constructor

static struct unicode_decompose_init { // FIXME UGH, static ctor init
  unicode_decompose_init();
} _unicode_decompose_init;

unicode_decompose_init::unicode_decompose_init()
{
  for (unsigned int i = 0;
       i < sizeof(unicode_decompose_list)/sizeof(unicode_decompose_list[0]);
       i++) {
    unicode_decompose *dec = new unicode_decompose[1];
    dec->value = (char *)unicode_decompose_list[i].value;
    unicode_decompose_table.define(unicode_decompose_list[i].key, dec);
  }
}

const char *decompose_unicode(const char *s)
{
  unicode_decompose *result = unicode_decompose_table.lookup(s);
  return result ? result->value : 0;
}

// s-it2-mode
END

# Remove temporary files.
rm $$1 $$2 $$3 $$4

# s-sh-mode
