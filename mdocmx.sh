#!/bin/sh -
#@ mdocmx.sh - mdocmx(7) preprocessor for single-pass troff.
#@ mdocmx(7) extends the mdoc(7) semantic markup language by references,
#@ allowing mdoc(7) to create anchors and table of contents.
#@ Synopsis: mdocmx[.sh] [:-v:] [-t | -T Sh|sh|Ss|ss] [FILE]
#@ -v: increase verbosity
#@ -t: wether -toc lines shall be expanded (as defined)
#@ -T: wether -toc lines shall be expanded as specified: only .Sh / .Sh + .Ss.
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
  echo >&2 "Synopsis: ${0} [:-v:] [-t | -T Sh|sh|Ss|ss] [FILE]"
  exit ${ex}
}

##

find_awk || synopsis 1 "Cannot find a usable awk(1) implementation"

while getopts vtT: i; do
  case ${i} in
  v)
    V=`expr ${V} + 1`;;
  t)
    [ x != x"${T}" ] && synopsis ${EX_USAGE} "-toc line expansion yet defined"
    T=Sh;;
  T)
    [ x != x"${T}" ] && synopsis ${EX_USAGE} "-toc line expansion yet defined"
    case "${OPTARG}" in
    [Ss]h)  T=Sh;;
    [Ss]s)  T=Ss;;
    *)      synopsis ${EX_USAGE} "Invalid -T argument: -- ${OPTARG}";;
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
APOSTROPHE=\'
exec ${AWK} -v VERBOSE=${V} -v TOC="${T}" '
BEGIN {
  TMPDIR = "/tmp"

  # xxx TMP/TEMP are Windows, why test them?
  split("TMPDIR TMP TEMP", ENV_TMP)

  # Number of times we try to create our temporary file
  TMP_CREATE_RETRIES = 421

  # The mdoc macros that support referencable anchors.
  # .Sh and .Ss also create anchors, but since they do not require .Mx they are
  # treated special and handled directly -- update manual on change!
  UMACS = "Ar Cm Dv Er Ev Fl Fn Fo Ic Pa Va"

  # Some of those impose special rules for their arguments; mdocmx(1) solves
  # this by outsourcing such desires in argument parse preparation hooks
  UMACS_KEYHOOKS = "Fl Fn"

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

  # Punctuation to be ignored (without changing current mode)
  UPUNCTS = ". , : ; ( ) [ ] ? !"

  #  --  >8  --  8<  --  #

  if (!VERBOSE)
    VERBOSE = 0
  if (VERBOSE > 1)
    TMP_CREATE_RETRIES = 2

  i = split(UMACS, savea)
  for (j = 1; j <= i; ++j) {
    k = savea[j]
    MACS[k] = k
  }

  i = split(UMACS_KEYHOOKS, savea)
  for (j = 1; j <= i; ++j) {
    k = savea[j]
    MACS_KEYHOOKS[k] = k
  }

  i = split(UCOMMS, savea)
  for (j = 1; j <= i; ++j) {
    k = savea[j]
    COMMS[k] = k
  }

  i = split(UPUNCTS, savea)
  for (j = 1; j <= i; ++j) {
    k = savea[j]
    PUNCTS[k] = k
  }

  EX_USAGE = 64
  EX_DATAERR = 65
  EX_TEMPFAIL = 75

  mx_bypass = 0   # Avoid preprocessing if parsing preprocessed file!

  mx_nlcont = ""  # Line continuation in progress?  (Then: data so far)
  mx_nlcontfun = 0 # Which function to call after line complete
  NLCONT_SH_SS_COMM = 1
  NLCONT_MX_COMM = 2
  NLCONT_MX_CHECK_LINE = 3

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
  #ARG, [..]      # Next parsed argument (from arg_parse() helper)
}

END {
  # If we were forced to create referenceable anchors, dump the temporary file
  # after writing our table-of-anchors (TAO :)
  if (mx_fo) {
    close(mx_fo)

    if (mx_stack_cnt > 0)
      warn("At end of file: \".Mx\" stack not empty (" mx_stack_cnt " levels)")

    for (i = 1; i <= mx_sh_cnt; ++i)
      print ".Mx -anchor-spass", mx_sh[i], i
    for (i = 1; i <= mx_ss_cnt; ++i) {
      print ".Mx -anchor-spass", mx_ss[i], mx_sh_ss[i]
    }
    for (i = 1; i <= mx_anchors_cnt; ++i)
      print ".Mx -anchor-spass", mx_anchors[i]

    # If we are about to produce a TOC, intercept ".Mx -toc" lines and replace
    # them with the desired TOC content
    if (!TOC) {
      while (getline < mx_fo)
        print
    } else {
      while (getline < mx_fo) {
        if ($0 ~ /^[[:space:]]*\.[[:space:]]*Mx[[:space:]]+-toc[[:space:]]*/) {
          print ".Sh TABLE OF CONTENTS"
          if (mx_sh_cnt > 0) {
            print ".Bl -inset"
            for (i = 1; i <= mx_sh_cnt; ++i) {
              print ".It Sx", substr(mx_sh[i], 4)
              if (TOC == "Ss")
                toc_print_ss(i)
            }
            print ".El"
          }
          # Rather illegal, but it maybe we have .Ss yet no .Sh
          else if (TOC == "Ss" && mx_ss_cnt > 0) {
            print ".Bl -tag"
            for (i = 1; i <= mx_ss_cnt; ++i)
              print ".It Sx", substr(mx_ss[i], 4)
            print ".El"
          }
        } else
          print
      }
    }

    system("rm -f " mx_fo)
  }
}

function f_a_l() {
  if (!fal) {
    fal = FILENAME
    if (!fal || fal == "-")
      fal = "<stdin>"
  }
  return fal ":" NR
}

function dbg(s) {
  if (VERBOSE > 1)
    print "DBG@" f_a_l() ": " s >> "/dev/stderr"
}

function warn(s) {
  if (VERBOSE > 0)
    print "WARN@" f_a_l() ": mdocmx(7): " s "." >> "/dev/stderr"
}

function fatal(e, s) {
  print "FATAL@" f_a_l() ": mdocmx(7): " s "." >> "/dev/stderr"
  exit e
}

#
function tmpdir() {
  for (t_i in ENV_TMP) {
    t_j = ENVIRON[ENV_TMP[t_i]]
    if (t_j && system("test -d " t_j) == 0) {
      dbg("temporary directory via ENVIRON: \"" t_j "\"")
      return t_j
    }
  }
  t_j = TMPDIR
  if (system("test -d " t_j) != 0)
    fatal(EX_TEMPFAIL,
      "Cannot find a usable temporary directory, please set $TMPDIR")
  dbg("temporary directory, fallback: \"" t_j "\"")
  return t_j
}

# Dump all .Ss which belong to the .Sh with the index sh_idx, if any
function toc_print_ss(sh_idx)
{
  tps_any = 0
  for (tps_i = 1; tps_i <= mx_ss_cnt; ++tps_i) {
    tps_j = mx_sh_ss[tps_i]
    if (tps_j < sh_idx)
      continue
    if (tps_j > sh_idx)
      break

    if (!tps_any) {
      tps_any = 1
      print ".Bl -tag -offset indent"
    }
    print ".It Sx", substr(mx_ss[tps_i], 4)
  }
  if (tps_any)
    print ".El"
}

# Parse the next _roff_ argument from the awk(1) line (in $0).
# If "no" < 0, reset the parser and return wether the former state would
# have parsed another argument from the line.
# If "no" is >0 we start at $(no); if it is 0, iterate to the next argument.
# Returns ARG.  Only used when "hot"
function arg_pushback(arg) { pa_pushback = arg }
function arg_parse(no) { # TODO this is our (documented!) WS problem..
  if (no < 0) {
    no = pa_no
    pa_no = 0
    pa_pushback = ""
    return no < NF
  }
  if (no == 0) {
    if (pa_pushback) {
      ARG = pa_pushback
      pa_pushback = ""
      return ARG
    }
    no = pa_no + 1
  }
  pa_pushback = ""

  ARG = ""
  for (pa_i = 0; no <= NF; ++no) {
    pa_j = $(no)

    # TODO We currently cannot handle "XY" enclosed in single quotes,
    # TODO even though this special case is handled by mandoc(1) + mdoc(7)
    if (pa_j ~ /^.+".+/)
      fatal(EX_DATAERR, "\".Mx\": quoting rules too complicated for mdocmx(1)")

    if (pa_j ~ /^"/) {
      if (pa_i)
        fatal(EX_DATAERR,
          "\".Mx\": quoting rules too complicated for mdocmx(1)")
      pa_i = 1;
      pa_j = substr(pa_j, 2)
    }

    if (pa_j ~ /"$/) {
      if (!pa_i)
        fatal(EX_DATAERR,
          "\".Mx\": quoting rules too complicated for mdocmx(1)")
      pa_i = 0
      pa_j = substr(pa_j, 1, length(pa_j) - 1)
    }

    if (ARG)
      pa_j = " " pa_j
    ARG = ARG pa_j
    if (!pa_i) {
      if (ARG != pa_j)
        # This is documented in the manual (several times i think)
        warn("Whitespace (possibly) normalized to single SPACE")
      break
    }
  }
  pa_no = no
  return ARG
}

function arg_cleanup(arg) {
  # Deal with common special glyphs etc.
  # Note: must be in sync with mdoc(7) macros!
  ac_i = match(arg, /([ \t]|\\&|\\%|\\c)+$/)
  if (ac_i)
    arg = substr(arg, 1, ac_i - 1)
  while (arg ~ /^[ \t]/)
    arg = substr(arg, 1)
  while (arg ~ /^(\\&|\\%|\\c)/)
    arg = substr(arg, 3)
  return arg
}

# ".Mx -enable" seen, create temporary file storage
function mx_enable() {
  # However, are we running on an already preprocessed document?  Bypass!
  if (NF > 2) {
    if (NF > 3 || $3 != "-preprocessed")
      fatal(EX_DATAERR, "\".Mx\": synopsis: \".Mx -enable\"")
    mx_bypass = 1
    print ".Mx -enable -preprocessed"
    return
  }

  mxe_j = tmpdir()
  for (mxe_i = 1; mxe_i <= TMP_CREATE_RETRIES; ++mxe_i) {
    mx_fo = mxe_j "/mdocmx-" mxe_i ".mx"
    # RW by user only, avoid overwriting of existing files
    if (system("{ umask 077; set -C; :> " mx_fo "; } >/dev/null 2>&1") == 0) {
      dbg("\".Mx -enable\" ok, temporary file: \"" mx_fo "\"")
      print ".Mx -enable -preprocessed"
      return
    }
  }
  fatal(EX_TEMPFAIL, "Cannot create a temporary file within \"" mxe_j "/\"")
}

# Deal with a non-"-enable" ".Mx" request
function mx_comm() {
  # No argument: plain push
  if (NF == 1) {
    ++mx_stack_cnt
    dbg(".Mx: [noarg] -> +1, stack size=" mx_stack_cnt)
    return
  }

  # ".Mx -toc"
  if ($2 == "-toc") {
    # Nothing to do here, do not check device arguments
    return
  }

  # This explicitly specifies the macro to create an anchor for next
  mxc_i = $2
  if (mxc_i ~ /^\./) {
    warn("\".Mx\": stripping dot prefix from \"" mxc_i "\"")
    mxc_i = substr(mxc_i, 2)
  }

  mxc_j = MACS[mxc_i]
  if (!mxc_j)
    fatal(EX_DATAERR, "\".Mx\": macro \"" mxc_i "\" not supported")
  mx_stack[++mx_stack_cnt] = mxc_i
  dbg(".Mx: for next \"." mxc_i "\", stack size=" mx_stack_cnt)

  # Do we also have a fixed key?
  if (NF == 2)
    return
  mx_keystack[mx_stack_cnt] = arg_parse(3)
  dbg("  ... USER KEY given: <" ARG ">");
  if (arg_parse(-1))
    fatal(EX_DATAERR, "\".Mx\": data after USER KEY is faulty syntax")
}

# mx_stack_cnt is >0, check wether this line will pop the stack
function mx_check_line() {
  # May be line continuation in the middle of nowhere
  if (!mx_stack_cnt)
    return
  # Must be a non-comment macro line
  if ($0 !~ /^[[:space:]]*[.'${APOSTROPHE}'][[:space:]]*[^"#]/)
    return

  # Iterate over all arguments and try to classify them, comparing them against
  # stack content as applicable
  mcl_mac = ""
  mcl_cont = 0
  for (arg_parse(-1); arg_parse(0);) {
    # Solely ignore punctuation (are we too stupid here?)
    if (PUNCTS[ARG])
      continue

    # (xxx Do this at the end of the loop instead, after decrement?)
    if (mx_stack_cnt == 0) {
      dbg("stack empty, stopping arg processing before <" ARG ">")
      break
    }

    mcl_j = mx_stack[mx_stack_cnt]

    # Is this something we consider a macro?
    mcl_cont = 0
    if (ARG ~ /^\./)
      ARG = substr(ARG, 2)
    mcl_i = MACS[ARG]
    if (mcl_i)
      mcl_mac = mcl_i
    else {
      if (!mcl_mac)
        continue
      # It may be some mdoc command nonetheless, ensure it does not fool our
      # simpleminded processing, and end possible mcl_mac savings
      if (COMMS[ARG]) {
        if (mcl_j)
          dbg("NO POP due macro (got<" ARG "> want<" mcl_j ">)")
        mcl_mac = ""
        continue
      }
      mcl_i = mcl_mac
      mcl_cont = 1
    }

    # Current command matches the one on the stack, if there is any
    if (mcl_j) {
      if (mcl_i != mcl_j) {
        dbg("NO POP due macro (got<" mcl_i "> want<" mcl_j ">)")
        mcl_mac = ""
        continue
      }
    }

    # We need the KEY
    if (!mcl_cont && !arg_parse(0))
      fatal(EX_DATAERR, "\".Mx\": expected KEY after \"" mcl_mac "\"")
    ARG = arg_cleanup(ARG)
    if (MACS_KEYHOOKS[mcl_mac])
      _mx_check_line_keyhook()

    if (mx_keystack[mx_stack_cnt]) {
      mcl_i = mx_keystack[mx_stack_cnt]
      if (mcl_i != ARG) {
        dbg("NO POP mac<" mcl_mac "> due key (got<" ARG "> want <" mcl_i ">)")
        continue
      }
      delete mx_keystack[mx_stack_cnt]
      mcl_i = "STACK"
    } else
      mcl_i = "USER"

    delete mx_stack[mx_stack_cnt--]
    dbg("POP mac<" mcl_mac "> " mcl_i " key <" ARG \
      "> stack size=" mx_stack_cnt)

    # Warn about and ignore duplicate anchors
    ARG = mcl_mac " \"" ARG "\""
    for (mcl_i = 1; mcl_i <= mx_anchors_cnt; ++mcl_i)
      if (mx_anchors[mcl_i] == ARG)
        break
    if (mcl_i > mx_anchors_cnt)
      mx_anchors[++mx_anchors_cnt] = ARG
    else
      warn("\".Mx\": duplicate anchor avoided: " ARG)
  }
}

function _mx_check_line_keyhook() {
  # .Fl: arguments may be continued via |, as in ".Fl a | b | c"
  if (mcl_mac == "Fl") {
    mclpkh_i = ARG
    for (mclpkh_j = 0;; ++mclpkh_j) {
      if (!arg_parse(0))
        break
      if (ARG != "|") {
        arg_pushback(ARG)
        break
      }
      if (!arg_parse(0)) {
        warn("Premature end of \".Fl\" continuation via \"|\"")
        break
      }
      # Be aware that this argument may indeed be a macro
      # XXX However, only support another Fl as in
      # XXX  .Op Fl T | Fl t Ar \&Sh | sh | \&Ss | ss
      # XXX We are too stupid to recursively process any possible thing,
      # XXX more complicated recursions are simply not supported
      if (ARG == "Fl") {
        arg_pushback(ARG)
        break
      }
      ARG = arg_cleanup(ARG)
      mclpkh_i = mclpkh_i " | " ARG
    }
    ARG = mclpkh_i
  }
  # .Fn: in ".Fn const char *funcname" all we want is "funcname"
  else if (mcl_mac == "Fn") {
    if (ARG ~ /[*&[:space:]]/) {
      mclpkh_i = match(ARG, /[^*&[:space:]]+$/)
      ARG = arg_cleanup(substr(ARG, mclpkh_i))
    }
  }
}

# Handle a .Sh or .Ss
function sh_ss_comm() {
  ssc_s = ""
  ssc_i = 0
  for (arg_parse(-1); arg_parse(0); ++ssc_i) {
    if (ssc_i < 1)
      continue
    if (ssc_i > 1)
      ssc_s = ssc_s " "
    ssc_s = ssc_s ARG
  }
  ssc_s = arg_cleanup(ssc_s)
  if ($1 ~ /Sh/)
    mx_sh[++mx_sh_cnt] = "Sh \"" ssc_s "\""
  else {
    mx_ss[++mx_ss_cnt] = "Ss \"" ssc_s "\""
    mx_sh_ss[mx_ss_cnt] = mx_sh_cnt
  }
}

# This is our *very* primitive way of dealing with line continuation
function line_nlcont_add(fun) {
  mx_nlcont = mx_nlcont $0
  mx_nlcont = substr(mx_nlcont, 1, length(mx_nlcont) - 1)
  if (!mx_nlcontfun)
    mx_nlcontfun = fun
}

function line_nlcont_done() {
  lnd_save = $0
  $0 = mx_nlcont $0
  mx_nlcont = ""
  if (mx_nlcontfun == NLCONT_SH_SS_COMM)
    sh_ss_comm()
  else if (mx_nlcontfun == NLCONT_MX_COMM)
    mx_comm()
  else if (mx_nlcontfun == NLCONT_MX_CHECK_LINE)
    mx_check_line()
  else
    fatal(EX_DATAERR, "mdocmx(1) implementation error: line_nlcont_done()")
  mx_nlcontfun = 0
  $0 = lnd_save # simplify callees life
}

# .Mx is a line that we care about
/^[[:space:]]*[.'${APOSTROPHE}'][[:space:]]*M[Xx][[:space:]]*/ {
  if (mx_bypass)
    print
  else if (mx_fo) {
    if (NF > 1 && $2 == "-enable")
      fatal(EX_USAGE, "\".Mx -enable\" may be used only once")
    if (mx_nlcont)
      fatal(EX_DATAERR, "Line continuation too complicated for mdocmx(1)")
    if ($0 ~ /\\$/)
      line_nlcont_add(NLCONT_MX_COMM)
    else
      mx_comm()
    print >> mx_fo
  } else if (NF < 2 || $2 != "-enable")
    fatal(EX_USAGE, "\".Mx -enable\" must be the first \".Mx\" command")
  else
    mx_enable()
  next
}

# .Sh and .Ss are also lines we care about, but always store the data in
# main memory, since those commands occur in each mdoc file
/^[[:space:]]*[.'${APOSTROPHE}'][[:space:]]*S[hs][[:space:]]+/ {
  if (mx_fo) {
    if (mx_nlcont)
      fatal(EX_DATAERR, "Line continuation too complicated for mdocmx(1)")
    if ($0 ~ /\\$/)
      line_nlcont_add(NLCONT_SH_SS_COMM)
    else
      sh_ss_comm()
    print >> mx_fo
    next
  }
}

# All other lines are uninteresting unless mdocmx is -enabled and we have
# pending anchor creation requests on the stack
{
  if (!mx_fo)
    print
  else {
    # TODO No support for any macro END but ..
    if (/^[[:space:]]*[.'${APOSTROPHE}'][[:space:]]*dei?1?[[:space:]]+/) {
      if (mx_nlcont)
        fatal(EX_DATAERR, "Line continuation too complicated for mdocmx(1)")
      print >> mx_fo
      while (getline && $0 !~ /^\.\.$/)
        print >> mx_fo
    } else if ($0 ~ /\\$/)
      line_nlcont_add(NLCONT_MX_CHECK_LINE)
    else if (mx_nlcont)
      line_nlcont_done()
    else if (mx_stack_cnt)
      mx_check_line()
    print >> mx_fo
  }
}
' "${F}"
# }}}

# s-it2-mode
