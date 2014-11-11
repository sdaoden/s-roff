#!/bin/sh -
#@ mdoc .Mx preprocessor -- allow the mdoc macro package to create references
#@ to anchors defined via the new .Mx command and the existing .Sx command.
#@ Synopsis: mdoxmx[.sh] [:-v:] [-t Sh|Ss] [FILE]
#@ -v: increase verbosity; -t: wether TOC shall be expanded (for .Sh / .Sh+.Ss)
#
# TODO use memory until config. limit exceeded, say 1 MB, only then tmpfile.
#
# Written 2014 by Steffen (Daode) Nurpmeso <sdaoden@users.sf.net>.
# Public Domain

AWK= V=0 T= F=
EX_USAGE=64

find_awk() {
  i=${IFS}
  IFS=:
  set -- ${PATH}:/bin
  IFS=${i}
  AWK=
  # for i; do -- new in POSIX Issue 7 + TC1
  for i
  do
    if [ -x "${i}/awk" ]; then
      AWK="${i}/awk"
      return 0
    fi
  done
  return 1
}

synopsis() {
  ex=${1} msg=${2}
  [ -n "${msg}" ] && echo >&2 ${msg}
  echo >&2 "Synopsis: ${0} [:-v:] [-t Sh|Ss] [FILE]"
  exit ${ex}
}

##

find_awk || synopsis 1 "Cannot find a usable awk(1) implementation"

while getopts vt: i; do
  case ${i} in
  v)
    V=`expr ${V} + 1`;;
  t)
    case "${OPTARG}" in
    Sh) T=Sh;;
    Ss) T=Ss;;
    *)  synopsis ${EX_USAGE} "Invalid -t argument: -- ${OPTARG}";;
    esac;;
  ?)
    synopsis ${EX_USAGE} "";;
  esac
done
OPTIND=`expr ${OPTIND} - 1`
shift ${OPTIND}

[ ${#} -gt 1 ] && synopsis ${EX_USAGE} "Excess arguments given"
[ ${#} -eq 0 ] && F=- || F="${1}"

# Let's go awk {{{
exec ${AWK} -v VERBOSE=${V} -v TOC="${T}" '
BEGIN {
  TMPDIR = "/tmp"

  # xxx TMP/TEMP are Windows, why test them?
  split("TMPDIR TMP TEMP", ENV_TMP)

  # Number of times we try to create our temporary file
  TMP_CREATE_RETRIES = 421

  # The mdoc macros that support referencable anchors.
  # .Sh and .Ss also create anchors, but since they do not require .Mx they are
  # treated special and handled directly
  # Update manual on change!
  UMACS = "Ar Cm Dv Er Ev Fl Fn Fo Ic Pa Va"

  # We can support macro mappings on preprocessor level
  # (Since the preprocessor only exists because troff is not multipass, in which
  # case the macros could solely act by themselves, it does not seem pretty
  # useful to outsource mapping knowledge from them, though.  But for testing.)
  # Update manual on change!
  MACS_MAP["Fo"] = "Fn"

  # A list of all mdoc commands; taken from mdocml, "mdoc.c,v 1.226 2014/10/16"
  UCOMMS = \
      "%A %B %C %D %I %J %N %O %P %Q %R %T %U %V " \
      "Ac Ad An Ao Ap Aq Ar At Bc Bd " \
        "Bf Bk Bl Bo Bq Brc Bro Brq Bsx Bt Bx " \
      "Cd Cm D1 Db Dc Dd Dl Do Dq Dt Dv Dx " \
      "Ec Ed Ef Ek El Em En Eo Er Es Ev Ex Fa Fc Fd Fl Fn Fo Fr Ft Fx " \
      "Hf " \
      "Ic In It " \
      "Lb Li Lk Lp " \
      "Ms Mt Nd Nm No Ns Nx " \
      "Oc Oo Op Os Ot Ox Pa Pc Pf Po Pp Pq " \
      "Qc Ql Qo Qq Re Rs Rv " \
      "Sc Sh Sm So Sq Ss St Sx Sy Ta Tn " \
      "Ud Ux Va Vt Xc Xo Xr " \
      "br ll sp "

  # Punctuation to be ignored (without changing current mode
  UPUNCT = ". , : ; ( ) [ ] ? !"

  #  --  >8  --  8<  --  #

  if (!VERBOSE)
    VERBOSE = 0
  if (VERBOSE > 1)
    TMP_CREATE_RETRIES = 2

  split(UMACS, savea)
  for (i in savea) {
    i = savea[i]
    MACS[i] = i
  }

  split(UCOMMS, savea)
  for (i in savea) {
    i = savea[i]
    COMMS[i] = i
  }

  split(UPUNCTS, savea)
  for (i in savea) {
    i = savea[i]
    PUNCTS[i] = i
  }

  EX_USAGE = 64
  EX_DATAERR = 65
  EX_TEMPFAIL = 75

  #mx_sh[]        # Arrays which store headlines, and their sizes
  #mx_sh_cnt
  #mx_ss[]
  #mx_ss_cnt
  #mx_sh_ss[]     # With TOC we need relation of .Ss with its .Sh
  #mx_fo = ""     # Our temporary output fork (cleaned of .Mx)
  #mx_anchors[]   # Readily prepared anchors..
  #mx_anchors_cnt # ..number thereof
  #mx_stack[]     # Stack of future anchors to be parsed off..
  #mx_stack_cnt   # ..number thereof
  #mx_keystack[]  # User specified ".Mx MACRO KEY": store KEY somewhere
  #ARG, [..]      # Next parsed argument (from parse_arg() helper)
  # Global temporaries: i, j, k, save (further notes as applicable)
}

END {
  # If we were forced to create referenceable anchors, dump the temporary file
  # after writing our table-of-anchors (TAO :)
  if (mx_fo) {
    close(mx_fo)
    if (mx_stack_cnt > 0)
      warn("At end of file: index stack not empty (" mx_stack_cnt " levels)")

    for (i in mx_sh)
      print ".Mx -anchor-spass", mx_sh[i]
    for (i in mx_ss)
      print ".Mx -anchor-spass", mx_ss[i]
    for (i in mx_anchors)
      print ".Mx -anchor-spass", mx_anchors[i]

    # If we are about to produce a TOC, intercept ".Mx -toc" lines and replace
    # them with the desired TOC content
    if (!TOC) {
      while (getline < mx_fo)
        print
    } else {
      while (getline < mx_fo) {
        if ($0 ~ /^[[:space:]]*\.[[:space:]]*Mx[[:space:]]+-toc[[:space:]]*$/) {
          print ".Sh TABLE OF CONTENTS"
          if (mx_sh_cnt > 0) {
            print ".Bl -inset"
            for (i in mx_sh) {
              j = i
              i = mx_sh[i]
              i = substr(i, 4)
              print ".It Sx", i
              if (TOC == "Ss")
                toc_print_ss(j)
            }
            print ".El"
          }
          # Rather illegal, but it maybe we have .Ss yet no .Sh
          else if (TOC == "Ss" && mx_ss_cnt > 0) {
            print ".Bl -tag"
            for (i in mx_ss) {
              i = mx_ss[i]
              i = substr(i, 4)
              print ".It Sx", i
            }
            print ".El"
          }
        } else
          print
      }
    }

    system("rm -f " mx_fo)
  }
}

## Note: beware of recursion issues of used temporaries: i, j, k, save, [_fal]

function f_a_l() { # XXX soelim..
  if (!_fal) {
    _fal = FILENAME
    if (!_fal || _fal == "-")
      _fal = "<stdin>"
  }
  return _fal ":" NR
}

function dbg(s) {
  if (VERBOSE > 1)
    print "DBG@" f_a_l() ": " s >> "/dev/stderr"
}

function warn(s) {
  if (VERBOSE > 0)
    print "WARN@" f_a_l() ": " s "." >> "/dev/stderr"
}

function fatal(e, s) {
  print "FATAL@" f_a_l() ": " s "." >> "/dev/stderr"
  exit e
}

#
function tmpdir() {
  for (i in ENV_TMP) {
    j = ENVIRON[ENV_TMP[i]]
    if (j && system("test -d " j) == 0) {
      dbg("temporary directory via ENVIRON: \"" j "\"")
      return j
    }
  }
  j = TMPDIR
  if (system("test -d " j) != 0)
    fatal(EX_TEMPFAIL,
      "Cannot find a usable temporary directory, please set $TMPDIR")
  dbg("temporary directory, fallback: \"" j "\"")
  return j
}

# Dump all .Ss which belong to the .Sh with the index sh_idx, if any
function toc_print_ss(sh_idx)
{
  any = 0
  for (i in mx_sh_ss) {
    j = mx_sh_ss[i]
    if (j < sh_idx)
      continue
    if (j > sh_idx)
      break
    if (!any) {
      any = 1
      print ".Bl -tag -offset indent"
    }
    j = mx_ss[i]
    j = substr(j, 4)
    print ".It Sx", j
  }
  if (any)
    print ".El"
}

# Parse the next _roff_ argument from the awk(1) line (in $0).
# If "no" < 0, reset the parser and return wether the former state would
# have parsed another argument from the line.
# If "no" is >0 we start at $(no); it it is 0, iterate to the next argument.
# Returns ARG.  Only used when "hot"
# May NOT use: "k", "save".
function parse_arg(no) { # TODO this is our problem.. (no, -troff-2pass is..)
  if (no < 0) {
    no = _pa_no
    _pa_no = 0
    return no < NF
  }
  if (no == 0)
    no = _pa_no + 1

  ARG = ""
  for (i = 0; no <= NF; ++no) {
    j = $(no)
    if (j ~ /^.+".+/)
        fatal(EX_DATAERR, "\".Mx\": quoting rules too complicated for mdocmx")

    if (j ~ /^"/) {
      if (i)
        fatal(EX_DATAERR, "\".Mx\": quoting rules too complicated for mdocmx")
      i = 1;
      j = substr(j, 2)
    }

    if (j ~ /"$/) {
      if (!i)
        fatal(EX_DATAERR, "\".Mx\": quoting rules too complicated for mdocmx")
      i = 0
      j = substr(j, 1, length(j) - 1)
    }

    if (ARG)
      j = " " j
    ARG = ARG j
    if (!i) {
      if (ARG != j)
        # This is documented in the manual (several times i think)
        warn("\".Mx\": whitespace (possibly) normalized to single SPACE")
      break
    }
  }
  _pa_no = no
  return ARG
}

# ".Mx -enable" seen, create temporary file storage
function mx_enable() {
  j = tmpdir()
  for (i = 1; i <= TMP_CREATE_RETRIES; ++i) {
    mx_fo = j "/mdocmx-" i ".mx"
    # RW by user only, avoid overwriting of existing files
    if (system("{ umask 077; set -C; :> " mx_fo "; } >/dev/null 2>&1") == 0) {
      dbg("\".Mx -enable\" ok, temporary file: \"" mx_fo "\"")
      print ".Mx -enable"
      return
    }
  }
  fatal(EX_TEMPFAIL, "Cannot create a temporary file within \"" j "/\"")
}

# Deal with a non-"-enable" ".Mx" request
function mx_comm() {
  # No argument: plain push
  if (NF == 1) {
    ++mx_stack_cnt
    dbg(".Mx: [noarg] -> +1, stack size=" mx_stack_cnt)
    return
  }

  # ".Mx *DIGITS" awaits DIGITS anchors to come
  # Also: ".Mx -toc"
  if (NF == 2) {
    if ($2 ~ /^\*[[:digit:]]+$/) {
      i = substr($2, 2) + 0
      mx_stack_cnt += i
      dbg(".Mx: " $2 " -> +" i ", stack size=" mx_stack_cnt)
    } else if ($2 == "-toc") {
      # Ignored sofar
    }
    return
  }

  # This explicitly specifies the macro to create an anchor for next
  i = $2
  if (i ~ /^\./) {
    warn("\".Mx\": stripping dot prefix from \"" i "\"")
    i = substr(i, 2)
  }

  j = MACS[i]
  if (!j)
    fatal(EX_DATAERR, "\".Mx\": macro \"" i "\" not supported")
  j = MACS_MAP[i]
  if (j)
    i = j
  mx_stack[++mx_stack_cnt] = i
  dbg(".Mx: for next \"." i "\", stack size=" mx_stack_cnt)

  # Do we also have a fixed key?
  if (NF == 2)
    return
  mx_keystack[mx_stack_cnt] = parse_arg(3)
  dbg("  ... USER KEY given: <" ARG ">");
  if (parse_arg(-1))
    fatal(EX_DATAERR, "\".Mx\": data after USER KEY is faulty syntax")
}

# mx_stack_cnt is >0, check wether this line will pop the stack
function mx_check_line() {
  # Must be a non-comment macro line
  if ($0 !~ /^[[:space:]]*[.'\''][[:space:]]*[^"#]/)
    return

  # Iterate over all arguments and try to classify them, comparing them against
  # stack content as applicable
  _mcl_mac = ""
  _mcl_cont = 0
  for (parse_arg(-1); parse_arg(0);) {
    # Solely ignore sole punctuation, we are too stupid for such things
    if (PUNCTS[ARG])
      continue

    # (xxx Do this at the end of the loop instead, after decrement?)
    if (mx_stack_cnt == 0) {
      dbg("stack empty, stopping arg processing before <" ARG ">")
      break
    }

    j = mx_stack[mx_stack_cnt]

    # Is this something we consider a macro?
    _mcl_cont = 0
    if (ARG ~ /^\./)
      ARG = substr(ARG, 2)
    i = MACS[ARG]
    if (i)
      _mcl_mac = i
    else {
      if (!_mcl_mac)
        continue
      # It may be some mdoc command nonetheless, ensure it does not fool our
      # simpleminded processing, and end possible _mcl_mac savings
      if (COMMS[ARG]) {
        if (j)
          dbg("NO POP due macro (got<" ARG "> want<" j ">)")
        _mcl_mac = ""
        continue
      }
      i = _mcl_mac
      _mcl_cont = 1
    }

    # Current command matches the one on the stack, if there is any
    if (j) {
      if (i != j) {
        dbg("NO POP due macro (got<" i "> want<" j ">)")
        _mcl_mac = ""
        continue
      }
    }

    # We need the KEY
    if (!_mcl_cont && !parse_arg(0))
      fatal(EX_DATAERR, "\".Mx\": expected KEY after \"" _mcl_mac "\"")
    if (mx_keystack[mx_stack_cnt]) {
      i = mx_keystack[mx_stack_cnt]
      if (i != ARG) {
        dbg("NO POP mac<" _mcl_mac "> due key (got<" ARG "> want <" i ">)")
        continue
      }
      delete mx_keystack[mx_stack_cnt]
      i = "STACK"
    } else
      i = "USER"

    delete mx_stack[--mx_stack_cnt]
    dbg("POP mac<" _mcl_mac "> " i " key <" ARG "> stack size=" mx_stack_cnt)
    mx_anchors[++mx_anchors_cnt] = _mcl_mac " \"" ARG "\""
  }
}

# Handle a .Sh or .Ss
function sh_ss_comm() {
  save = ""
  k = 0
  for (parse_arg(-1); parse_arg(0); ++k) {
    if (k < 1)
      continue
    if (k > 1)
      save = save " "
    save = save ARG
  }
  if ($1 ~ /Sh/)
    mx_sh[++mx_sh_cnt] = "Sh \"" save "\""
  else {
    mx_ss[++mx_ss_cnt] = "Ss \"" save "\""
    mx_sh_ss[mx_ss_cnt] = mx_sh_cnt
  }
}

# .Mx is a line that we care about
/^[[:space:]]*\.[[:space:]]*M[Xx][[:space:]]*/ {
  # Strip possible existent trailing comment (xxx primitively)
  #i = index($0, "#")
  #if (i-- > 0)
  #  $0 = substr($0, 1, i)

  if (mx_fo) {
    if (NF > 1 && $2 == "-enable")
      fatal(EX_USAGE, "\".Mx -enable\" may be used only once")
    mx_comm()
    print >> mx_fo
  } else if (NF != 2 || $2 != "-enable")
    fatal(EX_USAGE, "\".Mx -enable\" must be the first \".Mx\" command")
  else
    mx_enable()
  next
}

# .Sh and .Ss are also lines we care about, but always store the data in
# main memory, since those commands occur in each mdoc file
/^[[:space:]]*\.[[:space:]]*S[hs][[:space:]]+/ {
  sh_ss_comm()
  # ..and process normally, too
}

# All other lines are uninteresting unless mdocmx is -enabled and we have
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
' "${F}"
# }}}

# s-it2-mode
