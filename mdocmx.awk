#!/usr/bin/awk -f
#@ mdoc .Mx preprocessor -- allow the mdoc macro package to create references
#@ to anchors defined via .Mx.  Set DBG=1 to gain some verbosity and stderr.
#@ Synopsis: mdocmx.awk [-v DBG=1] [:- | MDOCFILE.X:]
#@ TODO WS normalization is applied (because regex WS skip is lost; search TODO)
#@ TODO use memory until config. limit exceeded, say 1 MB, only then tmpfile.
#
# Written by Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>, 2014.
# Public Domain

BEGIN {
  TMPDIR = "/tmp"

  # xxx TMP/TEMP are Windows, why test them?
  ENV_TMP_CNT = split("TMPDIR TMP TEMP", ENV_TMP)

  # Number of times we try to create our temporary file
  TMP_CREATE_RETRIES = 421

  #  --  >8  --  8<  --  #

  # Debugging mdocmx.awk?
  if (!DBG)
    DBG = 0
  if (DBG)
    TMP_CREATE_RETRIES = 2

  # The mdoc macros that support referencable anchors
  MACS_CNT = split("Ar Cm Dv Er Ev Fl Fn Fo Ic Pa Va", MACS)
  # We can support macro mappings on preprocessor level
  # (Since the preprocessor only exists because troff isn't multipass, in which
  # case the macros could act by themselves, it doesn't seem pretty useful to
  # outsource mapping knowledge from them, though.  Except for testing.)
  #MACS_MAP["Fo"] = "Fn"

  EX_USAGE = 64
  EX_DATAERR = 65
  EX_TEMPFAIL = 75

  #mx_fo = ""     # Our temporary output fork (cleaned of .Mx)
  #mx_anchors[]   # Readily prepared anchors..
  #mx_anchors_cnt # ..number thereof
  #mx_stack[]     # Stack of future anchors to be parsed off..
  #mx_stack_cnt   # ..number thereof
  #mx_valstack[]  # User specified `.Mx MACRO VALUE': store VALUE somewhere
  #ARG, [..]      # Next parsed argument (from parse_arg() helper)
}

END {
  # If we were forced to create referenceable anchors, dump the temporary file
  # after writing our table-of-anchors (TAO :)
  if (mx_fo) {
    if (mx_stack_cnt > 0)
      warn("At end of file: index stack not empty (" mx_stack_cnt " levels)")

    for (i = 1; i <= mx_anchors_cnt; ++i) {
      print mx_anchors[i]
      delete mx_anchors[i]
    }
    while (getline < mx_fo)
      print
    system("rm -f " mx_fo)
  }
}

## Note: beware of recursion issues of used temporaries: i, j, save, [_fal]

function f_a_l() { # XXX soelim..
  if (!_fal) {
    _fal = FILENAME
    if (!_fal || _fal == "-")
      _fal = "<stdin>"
  }
  return _fal ":" NR
}

function dbg(s) {
  if (DBG)
    print "DBG@" f_a_l() ": " s >> "/dev/stderr"
}

function warn(s) {
  print "WARN@" f_a_l() ": " s "." >> "/dev/stderr"
}

function fatal(e, s) {
  print "FATAL@" f_a_l() ": " s "." >> "/dev/stderr"
  exit e;
}

#
function tmpdir() {
  for (i = 1; i <= ENV_TMP_CNT; ++i) {
    j = ENVIRON[ENV_TMP[i]]
    if (j && system("test -d " j) == 0) {
      dbg("temporary directory via ENVIRON: `" j "'")
      return j
    }
  }
  j = TMPDIR
  if (system("test -d " j) != 0)
    fatal(EX_TEMPFAIL,
      "Can't find a usable temporary directory, please set $TMPDIR")
  dbg("temporary directory, fallback: `" j "'")
  return j
}

# Parse the next _roff_ argument from the awk(1) line (in $0).
# If "no" < 0, reset the parser and return wether the former state would
# have parsed another argument from the line.
# If "no" is >0 we start at $(no); it it is 0, iterate to the next argument.
# Returns ARG.  Only used when "hot"
# May NOT use "save".
function parse_arg(no) { # TODO I WANT `.troffctl 2-pass' INSTEAD!!!!
  if (no < 0) {
    no = __ARG_NO
    __ARG_NO = 0
    return no < NF
  }
  if (no == 0)
    no = __ARG_NO + 1

  ARG = ""
  for (i = 0; no <= NF; ++no) {
    j = $(no)
    if (j ~ /^.+".+/)
        fatal(EX_DATAERR, "`.Mx': quoting rules too complicated for mdocmx")

    if (j ~ /^"/) {
      if (i)
        fatal(EX_DATAERR, "`.Mx': quoting rules too complicated for mdocmx")
      i = 1;
      j = substr(j, 2)
    }

    if (j ~ /"$/) {
      if (!i)
        fatal(EX_DATAERR, "`.Mx': quoting rules too complicated for mdocmx")
      i = 0
      j = substr(j, 1, length(j) - 1)
    }

    if (ARG)
      j = " " j
    ARG = ARG j
    if (!i) {
      if (ARG != j)
        dbg("`.Mx': whitespace (possibly) normalized to single SPACE") # TODO
      break
    }
  }
  __ARG_NO = no
  return ARG
}

# `.Mx -enable' seen, create temporary file storage
function mx_enable() {
  j = tmpdir()
  for (i = 1; i <= TMP_CREATE_RETRIES; ++i) {
    mx_fo = j "/mdocmx-" i ".mx"
    # RW by user only, avoid overwriting of existing files
    if (system("{ umask 077; set -C; :> " mx_fo "; } >/dev/null 2>&1") == 0) {
      dbg("`.Mx -enable' ok, temporary file: `" mx_fo "'")
      print ".Mx -enable"
      return
    }
  }
  fatal(EX_TEMPFAIL, "Can't create a temporary file within `" j "/'")
}

# Deal with a non-`-enable' `.Mx' request
function mx_comm() {
  # No argument: plain push
  if (NF == 1) {
    ++mx_stack_cnt
    dbg(".Mx: [noarg] -> +1, stack size=" mx_stack_cnt)
    return
  }

  # `.Mx *DIGITS' awaits DIGITS anchors to come
  if (NF == 2 && $2 ~ /^\*[[:digit:]]+$/) {
    i = substr($2, 2) + 0
    mx_stack_cnt += i
    dbg(".Mx: " $2 " -> +" i ", stack size=" mx_stack_cnt)
    return
  }

  # This explicitly specifies the macro to create an anchor for next
  i = $2
  if (i ~ /^\./) {
    warn("`.Mx': stripping dot prefix from `" i "'")
    i = substr(i, 2)
  }

  for (j = 1; j <= MACS_CNT; ++j)
    if (i == MACS[j]) {
      save = MACS_MAP[i]
      if (save)
        i = save
      mx_stack[++mx_stack_cnt] = i
      dbg(".Mx: for next `." i "', stack size=" mx_stack_cnt)
      break
    }
  if (j > MACS_CNT)
    fatal(EX_DATAERR, "`.Mx': macro `" i "' not supported")

  # Do we also have a fixed value?
  if (NF == 2)
    return
  mx_valstack[mx_stack_cnt] = parse_arg(3)
  dbg("  ... VALUE given: <" ARG ">");
  if (parse_arg(-1))
    fatal(EX_DATAERR, "`.Mx': data after VALUE is faulty syntax")
}

# mx_stack_cnt is >0, check wether this line will pop the stack
function mx_check_line() {
  # Must be a non-comment macro line
  if ($0 !~ /^[[:space:]]*[.'][[:space:]]*[^"#]/)
    return

  # What do we have on the stack?
  for (parse_arg(-1); parse_arg(0);) {
    i = ARG
    if (i ~ /^[.']/) # XXX ?
      i = substr(i, 2)

    # We of course only take care of macros we know about
    for (j = 1; j <= MACS_CNT; ++j)
      if (i == MACS[j]) {
        save = MACS_MAP[i]
        if (save)
          i = save
        break
      }
    if (j > MACS_CNT)
      continue

    j = mx_stack[mx_stack_cnt]
    if (j) {
      if (i != j) {
        dbg("no stack pop due key (arg: <" ARG "|" i "> stack<" j ">)")
        continue
      }
    }

    # We need the VALUE
    save = ARG
    if (!parse_arg(0))
      fatal(EX_DATAERR, "`.Mx': expected VALUE after `" save "'")
    if (mx_valstack[mx_stack_cnt]) {
      i = mx_valstack[mx_stack_cnt]
      if (i != ARG) {
        dbg("no stack pop due value (arg: <" save "> value<" ARG " != " i ">)")
        continue
      }
      delete mx_valstack[mx_stack_cnt]
      i = "STACK"
    } else
      i = "USER"

    delete mx_stack[--mx_stack_cnt]
    dbg("pop stack: macro<" save "> " i " value <" ARG \
      "> stack size=" mx_stack_cnt)
    mx_anchors[++mx_anchors_cnt] = ".Mx -anchor-spass " save " \"" ARG "\""
  }
}

# `.Mx' is a line that we care about
/^[[:space:]]*\.[[:space:]]*M[Xx][[:space:]]*/ {
  # Strip possible existent trailing comment (xxx primitively)
  i = index($0, "#")
  if (i-- > 0)
    $0 = substr($0, 1, i)

  if (mx_fo) {
    if (NF > 1 && $2 == "-enable")
      fatal(EX_USAGE, "`.Mx -enable' may be used only once")
    mx_comm()
    print >> mx_fo
  } else if (NF != 2 || $2 != "-enable")
    fatal(EX_USAGE, "`.Mx -enable' must be the first `.Mx' command")
  else
    mx_enable()
  next
}

# All other lines are uninteresting unless mdocmx is -enable'd and we have
# pending anchor creation requests on the stack
{
  if (!mx_fo)
    print
  else {
    if (mx_stack_cnt)
      mx_check_line()
    print >> mx_fo
  }
}

# s-it2-mode
