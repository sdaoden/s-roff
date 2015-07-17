#!/bin/sh -
#@ Please see INSTALL and make.rc instead.

LC_ALL=C
export LC_ALL

# For heaven's sake auto-redirect on SunOS/Solaris
if [ "x${SHELL}" = x ] || [ "${SHELL}" = /bin/sh ] && \
      [ -f /usr/xpg4/bin/sh ] && [ -x /usr/xpg4/bin/sh ]; then
   SHELL=/usr/xpg4/bin/sh
   export SHELL
   exec /usr/xpg4/bin/sh "${0}" "${@}"
fi
[ -n "${SHELL}" ] || SHELL=/bin/sh
export SHELL

# The feature set, to be kept in sync with make.rc
# If no documentation given, the option is used as such; if doc is a hyphen,
# entry is suppressed when configuration overview is printed, and also in the
# *features* string: most likely for obsolete features etc.
XOPTIONS="\
   ICONV='Character set conversion using iconv(3)' \
   TERMCAP='Terminal capability queries (termcap(5))' \
      TERMCAP_VIA_TERMINFO='Terminal capability queries use terminfo(5)' \
"

# Options which are automatically deduced from host environment, i.e., these
# need special treatment all around here to warp from/to OPT_ stuff
# setlocale, C90AMEND1, NL_LANGINFO, wcwidth
XOPTIONS_DETECT="\
   LOCALES='Locale support - printable characters etc. depend on environment' \
   MULTIBYTE_CHARSETS='Multibyte character sets' \
   TERMINAL_CHARSET='Automatic detection of terminal character set' \
   WIDE_GLYPHS='Wide glyph support' \
"

# Rather special options, for custom building, or which always exist.
# Mostly for generating the visual overview and the *features* string
XOPTIONS_XTRA="\
   CROSS_BUILD='Cross-compilation: trust any detected environment' \
   DEBUG='Debug enabled binary, not for end-users: THANKS!' \
   DEVEL='Computers do not blunder' \
"

# The problem is that we don't have any tools we can use right now, so
# encapsulate stuff in functions which get called in right order later on

option_reset() {
   set -- ${OPTIONS}
   for i
   do
      eval OPT_${i}=0
   done
}

option_maximal() {
   set -- ${OPTIONS}
   for i
   do
      eval OPT_${i}=1
   done
   OPT_DOTLOCK=require OPT_ICONV=require OPT_REGEX=require
}

option_setup() {
   option_parse OPTIONS_DETECT "${XOPTIONS_DETECT}"
   option_parse OPTIONS "${XOPTIONS}"
   option_parse OPTIONS_XTRA "${XOPTIONS_XTRA}"
}

# Inter-relationships XXX sort this!
option_update() {
   if feat_no ICONV; then
   fi

   if feat_no TERMCAP; then
      OPT_TERMCAP_VIA_TERMINFO=0
   fi

   if feat_yes DEVEL; then
      OPT_DEBUG=1
   fi
}

rc=./make.rc
lst=./mk-config.lst
ev=./mk-config.ev
h=./mk-config.h h_name=mk-config.h
mk=./mk-config.mk

newlst=./mk-nconfig.lst
newmk=./mk-nconfig.mk
newev=./mk-nconfig.ev
newh=./mk-nconfig.h
tmp0=___tmp
tmp=./${tmp0}1$$
tmp2=./${tmp0}2$$

##  --  >8  - << OPTIONS | OS/CC >> -  8<  --  ##

# Note that potential duplicates in PATH, C_INCLUDE_PATH etc. will be cleaned
# via path_check() later on once possible

# TODO cc_maxopt is brute simple, we should compile test program and dig real
# compiler versions for known compilers, then be more specific
[ -n "${cc_maxopt}" ] || cc_maxopt=100
#cc_force_no_stackprot=
#ld_need_R_flags=
#ld_no_bind_now=
#ld_rpath_not_runpath=

_CFLAGS= _LDFLAGS=

os_early_setup() {
   # We don't "have any utility": only path adjustments and such in here!
   [ -n "${OS}" ] || OS=`uname -s`
   export OS

   if [ ${OS} = SunOS ]; then
      msg 'SunOS / Solaris?  Applying some "early setup" rules ...'
      _os_early_setup_sunos
   fi
}

_os_early_setup_sunos() {
   # According to standards(5), this is what we need to do
   if [ -d /usr/xpg4 ]; then :; else
      msg 'ERROR: On SunOS / Solaris we need /usr/xpg4 environment!  Sorry.'
      config_exit 1
   fi
   PATH="/usr/xpg4/bin:/usr/ccs/bin:/usr/bin:${PATH}"
   [ -d /usr/xpg6 ] && PATH="/usr/xpg6/bin:${PATH}"
   export PATH
}

os_setup() {
   # OSENV ends up in *build-osenv*
   # OSFULLSPEC is used to recognize changes (i.e., machine type, updates etc.)
   OS=`echo ${OS} | ${tr} '[A-Z]' '[a-z]'`
   [ -n "${OSENV}" ] || OSENV=`uname -srm`
   [ -n "${OSFULLSPEC}" ] || OSFULLSPEC=`uname -a`
   msg 'Operating system is %s' ${OS}

   if [ ${OS} = darwin ]; then
      msg ' . have special Darwin environmental addons...'
      LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DYLD_LIBRARY_PATH}
   elif [ ${OS} = sunos ]; then
      msg ' . have special SunOS / Solaris "setup" rules ...'
      _os_setup_sunos
   elif [ ${OS} = unixware ]; then
      if feat_yes AUTOCC && acmd_set CC cc; then
         msg ' . have special UnixWare environmental rules ...'
         feat_yes DEBUG && _CFLAGS='-v -Xa -g' || _CFLAGS='-Xa -O'

         CFLAGS="${_CFLAGS} ${EXTRA_CFLAGS}"
         LDFLAGS="${_LDFLAGS} ${EXTRA_LDFLAGS}"
         export CC CFLAGS LDFLAGS
         OPT_AUTOCC=0 ld_need_R_flags=-R
      fi
   elif [ -n "${VERBOSE}" ]; then
      msg ' . no special treatment for this system necessary or known'
   fi

   # Sledgehammer: better set _GNU_SOURCE
   # And in general: oh, boy!
   OS_DEFINES="${OS_DEFINES}#define _GNU_SOURCE\n"
   #OS_DEFINES="${OS_DEFINES}#define _POSIX_C_SOURCE 200809L\n"
   #OS_DEFINES="${OS_DEFINES}#define _XOPEN_SOURCE 700\n"
   #[ ${OS} = darwin ] && OS_DEFINES="${OS_DEFINES}#define _DARWIN_C_SOURCE\n"

   # On pkgsrc(7) systems automatically add /usr/pkg/*
   if [ -d /usr/pkg ] && feat_yes USE_PKGSYS; then
      msg ' . found pkgsrc(7), merging C_INCLUDE_PATH and LD_LIBRARY_PATH'
      C_INCLUDE_PATH=/usr/pkg/include:${C_INCLUDE_PATH}
      LD_LIBRARY_PATH=/usr/pkg/lib:${LD_LIBRARY_PATH}
      ld_rpath_not_runpath=1
   fi
}

_os_setup_sunos() {
   C_INCLUDE_PATH=/usr/xpg4/include:${C_INCLUDE_PATH}
   LD_LIBRARY_PATH=/usr/xpg4/lib:${LD_LIBRARY_PATH}

   # Include packages
   if [ -d /opt/csw ] && feat_yes USE_PKGSYS; then
      msg ' . found OpenCSW PKGSYS, merging C_INCLUDE_PATH and LD_LIBRARY_PATH'
      C_INCLUDE_PATH=/opt/csw/include:${C_INCLUDE_PATH}
      LD_LIBRARY_PATH=/opt/csw/lib:${LD_LIBRARY_PATH}
      ld_no_bind_now=1 ld_rpath_not_runpath=1
   fi
   if [ -d /opt/schily ] && feat_yes USE_PKGSYS; then
      msg ' . found Schily PKGSYS, merging C_INCLUDE_PATH and LD_LIBRARY_PATH'
      C_INCLUDE_PATH=/opt/schily/include:${C_INCLUDE_PATH}
      LD_LIBRARY_PATH=/opt/schily/lib:${LD_LIBRARY_PATH}
      ld_no_bind_now=1 ld_rpath_not_runpath=1
   fi

   OS_DEFINES="${OS_DEFINES}#define __EXTENSIONS__\n"
   #OS_DEFINES="${OS_DEFINES}#define _POSIX_C_SOURCE 200112L\n"

   if feat_yes AUTOCC; then
      if acmd_set CC cc; then
         feat_yes DEBUG && _CFLAGS="-v -Xa -g" || _CFLAGS="-Xa -O"

         CFLAGS="${_CFLAGS} ${EXTRA_CFLAGS}"
         LDFLAGS="${_LDFLAGS} ${EXTRA_LDFLAGS}"
         export CC CFLAGS LDFLAGS
         OPT_AUTOCC=0 ld_need_R_flags=-R
      else
         cc_maxopt=2 cc_force_no_stackprot=1
      fi
   fi
}

# Check out compiler ($CC) and -flags ($CFLAGS)
cc_setup() {
   # Even though it belongs into cc_flags we will try to compile and link
   # something, so ensure we have a clean state regarding CFLAGS/LDFLAGS or
   # EXTRA_CFLAGS/EXTRA_LDFLAGS
   if feat_no AUTOCC; then
      _cc_default
      # Ensure those don't do any harm
      EXTRA_CFLAGS= EXTRA_LDFLAGS=
      export EXTRA_CFLAGS EXTRA_LDFLAGS
      return
   else
      CFLAGS= LDFLAGS=
      export CFLAGS LDFLAGS
   fi

   [ -n "${CC}" ] && { _cc_default; return; }

   msg_nonl 'Searching for a usable C compiler .. $CC='
   if acmd_set CC clang || acmd_set CC gcc ||
         acmd_set CC tcc || acmd_set CC pcc ||
         acmd_set CC c89 || acmd_set CC c99; then
      :
   else
      msg 'boing booom tschak'
      msg 'ERROR: I cannot find a compiler!'
      msg ' Neither of clang(1), gcc(1), tcc(1), pcc(1), c89(1) and c99(1).'
      msg ' Please set ${CC} environment variable, maybe ${CFLAGS}, rerun.'
      config_exit 1
   fi
   msg '%s' "${CC}"
   export CC
}

_cc_default() {
   if [ -z "${CC}" ]; then
      msg 'To go on like you have chosen, please set $CC, rerun.'
      config_exit 1
   fi

   if [ -z "${VERBOSE}" ] && [ -f ${lst} ] && feat_no DEBUG; then
      :
   else
      msg 'Using C compiler ${CC}=%s' "${CC}"
   fi
}

cc_flags() {
   if feat_yes AUTOCC; then
      if [ -f ${lst} ] && feat_no DEBUG && [ -z "${VERBOSE}" ]; then
         cc_check_silent=1
         msg 'Detecting ${CFLAGS}/${LDFLAGS} for ${CC}=%s, just a second..' \
            "${CC}"
      else
         cc_check_silent=
         msg 'Testing usable ${CFLAGS}/${LDFLAGS} for ${CC}=%s' "${CC}"
      fi

      i=`echo "${CC}" | ${awk} 'BEGIN{FS="/"}{print $NF}'`
      if { echo "${i}" | ${grep} tcc; } >/dev/null 2>&1; then
         msg ' . have special tcc(1) environmental rules ...'
         _cc_flags_tcc
      else
         # As of pcc CVS 2016-04-02, stack protection support is announced but
         # will break if used on Linux
         if { echo "${i}" | ${grep} pcc; } >/dev/null 2>&1; then
            cc_force_no_stackprot=1
         fi
         _cc_flags_generic
      fi

      feat_no DEBUG && _CFLAGS="-DNDEBUG ${_CFLAGS}"
      CFLAGS="${_CFLAGS} ${EXTRA_CFLAGS}"
      LDFLAGS="${_LDFLAGS} ${EXTRA_LDFLAGS}"
   else
      if feat_no DEBUG; then
         CFLAGS="-DNDEBUG ${CFLAGS}"
      fi
   fi
   msg ''
   export CFLAGS LDFLAGS
}

_cc_flags_tcc() {
   __cflags=${_CFLAGS} __ldflags=${_LDFLAGS}
   _CFLAGS= _LDFLAGS=

   cc_check -Wall
   cc_check -Wextra
   cc_check -pedantic

   if feat_yes DEBUG; then
      # May have problems to find libtcc cc_check -b
      cc_check -g
   fi

   if ld_check -Wl,-rpath =./ no; then
      ld_need_R_flags=-Wl,-rpath=
      if [ -z "${ld_rpath_not_runpath}" ]; then
         ld_check -Wl,--enable-new-dtags
      else
         msg ' ! $LD_LIBRARY_PATH adjusted, not trying --enable-new-dtags'
      fi
      ld_runtime_flags # update!
   fi

   _CFLAGS="${_CFLAGS} ${__cflags}" _LDFLAGS="${_LDFLAGS} ${__ldflags}"
   unset __cflags __ldflags
}

_cc_flags_generic() {
   __cflags=${_CFLAGS} __ldflags=${_LDFLAGS}
   _CFLAGS= _LDFLAGS=
   feat_yes DEVEL && cc_check -std=c89 || cc_check -std=c99

   # E.g., valgrind does not work well with high optimization
   if [ ${cc_maxopt} -gt 1 ] && feat_yes NOMEMDBG &&
         feat_no ASAN_ADDRESS && feat_no ASAN_MEMORY; then
      msg ' ! OPT_NOMEMDBG, setting cc_maxopt=1 (-O1)'
      cc_maxopt=1
   fi
   # Check -g first since some others may rely upon -g / optim. level
   if feat_yes DEBUG; then
      cc_check -O
      cc_check -g
   elif [ ${cc_maxopt} -gt 2 ] && cc_check -O3; then
      :
   elif [ ${cc_maxopt} -gt 1 ] && cc_check -O2; then
      :
   elif [ ${cc_maxopt} -gt 0 ] && cc_check -O1; then
      :
   else
      cc_check -O
   fi

   if feat_yes AMALGAMATION; then
      cc_check -pipe
   fi

   #if feat_yes DEVEL && cc_check -Weverything; then
   #   :
   #else
      cc_check -Wall
      cc_check -Wextra
      cc_check -Wbad-function-cast
      cc_check -Wcast-align
      cc_check -Wcast-qual
      cc_check -Winit-self
      cc_check -Wmissing-prototypes
      cc_check -Wshadow
      cc_check -Wunused
      cc_check -Wwrite-strings
      cc_check -Wno-long-long
   #fi
   cc_check -pedantic

   if feat_yes AMALGAMATION && feat_no DEVEL; then
      cc_check -Wno-unused-function
   fi
   feat_no DEVEL && cc_check -Wno-unused-result # XXX do right way (pragma too)

   cc_check -fno-unwind-tables
   cc_check -fno-asynchronous-unwind-tables
   cc_check -fstrict-aliasing
   if cc_check -fstrict-overflow && feat_yes DEVEL; then
      cc_check -Wstrict-overflow=5
   fi

   if feat_yes DEBUG || feat_yes FORCED_STACKPROT; then
      if [ -z "${cc_force_no_stackprot}" ]; then
         if cc_check -fstack-protector-strong ||
               cc_check -fstack-protector-all; then
            cc_check -D_FORTIFY_SOURCE=2
         fi
      else
         msg ' ! Not checking for -fstack-protector compiler option,'
         msg ' ! since that caused errors in a "similar" configuration.'
         msg ' ! You may turn off OPT_AUTOCC and use your own settings, rerun'
      fi
   fi

   # LD (+ dependend CC)

   if feat_yes ASAN_ADDRESS; then
      _ccfg=${_CFLAGS}
      if cc_check -fsanitize=address &&
            ld_check -fsanitize=address; then
         :
      else
         feat_bail_required ASAN_ADDRESS
         _CFLAGS=${_ccfg}
      fi
   fi

   if feat_yes ASAN_MEMORY; then
      _ccfg=${_CFLAGS}
      if cc_check -fsanitize=memory &&
            ld_check -fsanitize=memory &&
            cc_check -fsanitize-memory-track-origins=2 &&
            ld_check -fsanitize-memory-track-origins=2; then
         :
      else
         feat_bail_required ASAN_MEMORY
         _CFLAGS=${_ccfg}
      fi
   fi

   ld_check -Wl,-z,relro
   if [ -z "${ld_no_bind_now}" ]; then
      ld_check -Wl,-z,now
   else
      msg ' ! $LD_LIBRARY_PATH adjusted, not trying -Wl,-z,now'
   fi
   ld_check -Wl,-z,noexecstack
   if ld_check -Wl,-rpath =./ no; then
      ld_need_R_flags=-Wl,-rpath=
      # Choose DT_RUNPATH (after $LD_LIBRARY_PATH) over DT_RPATH (before)
      if [ -z "${ld_rpath_not_runpath}" ]; then
         ld_check -Wl,--enable-new-dtags
      else
         msg ' ! $LD_LIBRARY_PATH adjusted, not trying --enable-new-dtags'
      fi
      ld_runtime_flags # update!
   elif ld_check -Wl,-R ./ no; then
      ld_need_R_flags=-Wl,-R
      if [ -z "${ld_rpath_not_runpath}" ]; then
         ld_check -Wl,--enable-new-dtags
      else
         msg ' ! $LD_LIBRARY_PATH adjusted, not trying --enable-new-dtags'
      fi
      ld_runtime_flags # update!
   fi

   # Address randomization
   _ccfg=${_CFLAGS}
   if cc_check -fPIE || cc_check -fpie; then
      ld_check -pie || _CFLAGS=${_ccfg}
   fi
   unset _ccfg

   _CFLAGS="${_CFLAGS} ${__cflags}" _LDFLAGS="${_LDFLAGS} ${__ldflags}"
   unset __cflags __ldflags
}

##  --  >8  - <<OS/CC | SUPPORT FUNS>> -  8<  --  ##

## Notes:
## - Heirloom sh(1) (and same origin) have _sometimes_ problems with ': >'
##   redirection, so use "printf '' >" instead

## Very first: we undergo several states regarding I/O redirection etc.,
## but need to deal with option updates from within all.  Since all the
## option stuff should be above the scissor line, define utility functions
## and redefine them as necessary.
## And, since we have those functions, simply use them for whatever

t1=ten10one1ten10one1
if ( [ ${t1##*ten10} = one1 ] && [ ${t1#*ten10} = one1ten10one1 ] &&
      [ ${t1%%one1*} = ten10 ] && [ ${t1%one1*} = ten10one1ten10 ]
      ) > /dev/null 2>&1; then
   good_shell=1
else
   unset good_shell
fi
unset t1

( set -o noglob ) >/dev/null 2>&1 && noglob_shell=1 || unset noglob_shell

config_exit() {
   exit ${1}
}

# which(1) not standardized, command(1) -v may return non-executable: unroll!
acmd_test() { __acmd "${1}" 1 0 0; }
acmd_test_fail() { __acmd "${1}" 1 1 0; }
acmd_set() { __acmd "${2}" 0 0 0 "${1}"; }
acmd_set_fail() { __acmd "${2}" 0 1 0 "${1}"; }
acmd_testandset() { __acmd "${2}" 1 0 0 "${1}"; }
acmd_testandset_fail() { __acmd "${2}" 1 1 0 "${1}"; }
thecmd_set() { __acmd "${2}" 0 0 1 "${1}"; }
thecmd_set_fail() { __acmd "${2}" 0 1 1 "${1}"; }
thecmd_testandset() { __acmd "${2}" 1 0 1 "${1}"; }
thecmd_testandset_fail() { __acmd "${2}" 1 1 1 "${1}"; }
__acmd() {
   pname=${1} dotest=${2} dofail=${3} verbok=${4} varname=${5}

   if [ "${dotest}" -ne 0 ]; then
      eval dotest=\$${varname}
      if [ -n "${dotest}" ]; then
         [ -n "${VERBOSE}" ] && [ ${verbok} -ne 0 ] &&
            msg ' . ${%s} ... %s' "${pname}" "${dotest}"
         return 0
      fi
   fi

   oifs=${IFS} IFS=:
   [ -n "${noglob_shell}" ] && set -o noglob
   set -- ${PATH}
   [ -n "${noglob_shell}" ] && set +o noglob
   IFS=${oifs}
   for path
   do
      if [ -z "${path}" ] || [ "${path}" = . ]; then
         if [ -d "${PWD}" ]; then
            path=${PWD}
         else
            path=.
         fi
      fi
      if [ -f "${path}/${pname}" ] && [ -x "${path}/${pname}" ]; then
         [ -n "${VERBOSE}" ] && [ ${verbok} -ne 0 ] &&
            msg ' . ${%s} ... %s' "${pname}" "${path}/${pname}"
         [ -n "${varname}" ] && eval ${varname}="${path}/${pname}"
         return 0
      fi
   done

   # We may have no builtin string functions, we yet have no programs we can
   # use, try to access once from the root, assuming it is an absolute path if
   # that finds the executable
   if ( cd && [ -f "${pname}" ] && [ -x "${pname}" ] ); then
     [ -n "${VERBOSE}" ] && [ ${verbok} -ne 0 ] &&
            msg ' . ${%s} ... %s' "${pname}" "${pname}"
      [ -n "${varname}" ] && eval ${varname}="${pname}"
      return 0
   fi

   [ ${dofail} -eq 0 ] && return 1
   msg 'ERROR: no trace of utility '"${pname}"
   exit 1
}

msg() {
   fmt=${1}
   shift
   printf >&2 -- "${fmt}\\n" "${@}"
}

msg_nonl() {
   fmt=${1}
   shift
   printf >&2 -- "${fmt}" "${@}"
}

# Our feature check environment
feat_val_no() {
   [ "x${1}" = x0 ] || [ "x${1}" = xn ] ||
   [ "x${1}" = xfalse ] || [ "x${1}" = xno ] || [ "x${1}" = xoff ]
}

feat_val_yes() {
   [ "x${1}" = x1 ] || [ "x${1}" = xy ] ||
   [ "x${1}" = xtrue ] || [ "x${1}" = xyes ] || [ "x${1}" = xon ] ||
         [ "x${1}" = xrequire ]
}

feat_val_require() {
   [ "x${1}" = xrequire ]
}

_feat_check() {
   eval i=\$OPT_${1}
   i="`echo ${i} | ${tr} '[A-Z]' '[a-z]'`"
   if feat_val_no "${i}"; then
      return 1
   elif feat_val_yes "${i}"; then
      return 0
   else
      msg "ERROR: %s: 0/n/false/no/off or 1/y/true/yes/on/require, got: %s" \
         "${1}" "${i}"
      config_exit 11
   fi
}

feat_yes() {
   _feat_check ${1}
}

feat_no() {
   _feat_check ${1} && return 1
   return 0
}

feat_require() {
   eval i=\$OPT_${1}
   i="`echo ${i} | ${tr} '[A-Z]' '[a-z]'`"
   [ "x${i}" = xrequire ] || [ "x${i}" = xrequired ]
}

feat_bail_required() {
   if feat_require ${1}; then
      msg 'ERROR: feature OPT_%s is required but not available' "${1}"
      config_exit 13
   fi
   eval OPT_${1}=0
   option_update # XXX this is rather useless here (dependency chain..)
}

option_parse() {
   # Parse one of our XOPTIONS* in $2 and assign the sh(1) compatible list of
   # options, without documentation, to $1
   j=\'
   i="`${awk} -v input=\"${2}\" '
      BEGIN{
         for(i = 0;;){
            voff = match(input, /[[:alnum:]_]+(='${j}'[^'${j}']+)?/)
            if(voff == 0)
               break
            v = substr(input, voff, RLENGTH)
            input = substr(input, voff + RLENGTH)
            doff = index(v, "=")
            if(doff > 0){
               d = substr(v, doff + 2, length(v) - doff - 1)
               v = substr(v, 1, doff - 1)
            }
            print v
         }
      }
      '`"
   eval ${1}=\"${i}\"
}

option_doc_of() {
   # Return the "documentation string" for option $1, itself if none such
   j=\'
   ${awk} -v want="${1}" \
      -v input="${XOPTIONS_DETECT}${XOPTIONS}${XOPTIONS_XTRA}" '
   BEGIN{
      for(;;){
         voff = match(input, /[[:alnum:]_]+(='${j}'[^'${j}']+)?/)
         if(voff == 0)
            break
         v = substr(input, voff, RLENGTH)
         input = substr(input, voff + RLENGTH)
         doff = index(v, "=")
         if(doff > 0){
            d = substr(v, doff + 2, length(v) - doff - 1)
            v = substr(v, 1, doff - 1)
         }else
            d = v
         if(v == want){
            if(d != "-")
               print d
            exit
         }
      }
   }
   '
}

option_join_rc() {
   # Join the values from make.rc into what currently is defined, not
   # overwriting yet existing settings
   ${rm} -f ${tmp}
   # We want read(1) to perform reverse solidus escaping in order to be able to
   # use multiline values in make.rc; the resulting sh(1)/sed(1) code was very
   # slow in VMs (see [fa2e248]), Aharon Robbins suggested the following
   < ${rc} ${awk} 'BEGIN{line = ""}{
      gsub(/^[[:space:]]+/, "", $0)
      gsub(/[[:space:]]+$/, "", $0)
      if(gsub(/\\$/, "", $0)){
         line = line $0
         next
      }else
         line = line $0
      if(index(line, "#") == 1){
         line = ""
      }else if(length(line)){
         print line
         line = ""
      }
   }' |
   while read line; do
      if [ -n "${good_shell}" ]; then
         i=${line%%=*}
      else
         i=`${awk} -v LINE="${line}" 'BEGIN{
            gsub(/=.*$/, "", LINE)
            print LINE
         }'`
      fi
      if [ "${i}" = "${line}" ]; then
         msg 'ERROR: invalid syntax in: %s' "${line}"
         continue
      fi

      eval j="\$${i}" jx="\${${i}+x}"
      if [ -n "${j}" ] || [ "${jx}" = x ]; then
         : # Yet present
      else
         j=`${awk} -v LINE="${line}" 'BEGIN{
            gsub(/^[^=]*=/, "", LINE)
            gsub(/^\"*/, "", LINE)
            gsub(/\"*$/, "", LINE)
            print LINE
         }'`
      fi
      [ "${i}" = "DESTDIR" ] && continue
      echo "${i}=\"${j}\""
   done > ${tmp}
   # Reread the mixed version right now
   . ./${tmp}
}

option_evaluate() {
   # Expand the option values, which may contain shell snippets
   ${rm} -f ${newlst} ${newmk} ${newh}
   exec 5<&0 6>&1 <${tmp} >${newlst}
   while read line; do
      z=
      if [ -n "${good_shell}" ]; then
         i=${line%%=*}
         [ "${i}" != "${i#OPT_}" ] && z=1
      else
         i=`${awk} -v LINE="${line}" 'BEGIN{
            gsub(/=.*$/, "", LINE);\
            print LINE
         }'`
         if echo "${i}" | ${grep} '^OPT_' >/dev/null 2>&1; then
            z=1
         fi
      fi

      eval j=\$${i}
      if [ -n "${z}" ]; then
         j="`echo ${j} | ${tr} '[A-Z]' '[a-z]'`"
         if [ -z "${j}" ] || feat_val_no "${j}"; then
            j=0
            printf "   /* #undef ${i} */\n" >> ${newh}
         elif feat_val_yes "${j}"; then
            if feat_val_require "${j}"; then
               j=require
            else
               j=1
            fi
            printf "   /* #define ${i} */\n" >> ${newh}
         else
            msg 'ERROR: cannot parse <%s>' "${line}"
            config_exit 1
         fi
      else
         printf "#define ${i} \"${j}\"\n" >> ${newh}
      fi
      printf "${i} = ${j}\n" >> ${newmk}
      printf "${i}=${j}\n"
      eval "${i}=\"${j}\""
   done
   exec 0<&5 1>&6 5<&- 6<&-
}

path_check() {
   # "path_check VARNAME" or "path_check VARNAME FLAG VARNAME"
   varname=${1} addflag=${2} flagvarname=${3}
   j=${IFS}
   IFS=:
   [ -n "${noglob_shell}" ] && set -o noglob
   eval "set -- \$${1}"
   [ -n "${noglob_shell}" ] && set +o noglob
   IFS=${j}
   j= k= y= z=
   for i
   do
      [ -z "${i}" ] && continue
      [ -d "${i}" ] || continue
      if [ -n "${j}" ]; then
         if { z=${y}; echo "${z}"; } | ${grep} ":${i}:" >/dev/null 2>&1; then
            :
         else
            y="${y} :${i}:"
            j="${j}:${i}"
            # But do not link any fakeroot path into our binaries!
            if [ -n "${addflag}" ]; then
               case "${i}" in *fakeroot*) continue;; esac
               k="${k} ${addflag}${i}"
            fi
         fi
      else
         y=" :${i}:"
         j="${i}"
         # But do not link any fakeroot path into our binaries!
         if [ -n "${addflag}" ]; then
            case "${i}" in *fakeroot*) continue;; esac
            k="${k} ${addflag}${i}"
         fi
      fi
   done
   eval "${varname}=\"${j}\""
   [ -n "${addflag}" ] && eval "${flagvarname}=\"${k}\""
   unset varname
}

ld_runtime_flags() {
   if [ -n "${ld_need_R_flags}" ]; then
      i=${IFS}
      IFS=:
      set -- ${LD_LIBRARY_PATH}
      IFS=${i}
      for i
      do
         # But do not link any fakeroot path into our binaries!
         case "${i}" in *fakeroot*) continue;; esac
         LDFLAGS="${LDFLAGS} ${ld_need_R_flags}${i}"
         _LDFLAGS="${_LDFLAGS} ${ld_need_R_flags}${i}"
      done
      export LDFLAGS
   fi
   # Disable it for a possible second run.
   ld_need_R_flags=
}

cc_check() {
   [ -n "${cc_check_silent}" ] || msg_nonl ' . CC %s .. ' "${1}"
   if "${CC}" ${INCS} \
         ${_CFLAGS} ${1} ${EXTRA_CFLAGS} ${_LDFLAGS} ${EXTRA_LDFLAGS} \
         -o ${tmp2} ${tmp}.c ${LIBS} >/dev/null 2>&1; then
      _CFLAGS="${_CFLAGS} ${1}"
      [ -n "${cc_check_silent}" ] || msg 'yes'
      return 0
   fi
   [ -n "${cc_check_silent}" ] || msg 'no'
   return 1
}

ld_check() {
   # $1=option [$2=option argument] [$3=if set, shall NOT be added to _LDFLAGS]
   [ -n "${cc_check_silent}" ] || msg_nonl ' . LD %s .. ' "${1}"
   if "${CC}" ${INCS} ${_CFLAGS} ${_LDFLAGS} ${1}${2} ${EXTRA_LDFLAGS} \
         -o ${tmp2} ${tmp}.c ${LIBS} >/dev/null 2>&1; then
      [ -n "${3}" ] || _LDFLAGS="${_LDFLAGS} ${1}"
      [ -n "${cc_check_silent}" ] || msg 'yes'
      return 0
   fi
   [ -n "${cc_check_silent}" ] || msg 'no'
   return 1
}

dump_test_program=1
_check_preface() {
   variable=$1 topic=$2 define=$3

   echo '**********'
   msg_nonl ' . %s ... ' "${topic}"
   echo "/* checked ${topic} */" >> ${h}
   ${rm} -f ${tmp} ${tmp}.o
   if [ "${dump_test_program}" = 1 ]; then
      echo '*** test program is'
      { echo '#include <'"${h_name}"'>'; cat; } | ${tee} ${tmp}.c
   else
      { echo '#include <'"${h_name}"'>'; cat; } > ${tmp}.c
   fi
   #echo '*** the preprocessor generates'
   #${make} -f ${makefile} ${tmp}.x
   #${cat} ${tmp}.x
   echo '*** tests results'
}

without_check() {
   yesno=$1 variable=$2 topic=$3 define=$4 libs=$5 incs=$6

   echo '**********'
   msg_nonl ' . %s ... ' "${topic}"

   echo '*** enforced unchecked results are'
   if feat_val_yes ${yesno}; then
      if [ -n "${incs}" ] || [ -n "${libs}" ]; then
         echo "*** adding INCS<${incs}> LIBS<${libs}>"
         LIBS="${LIBS} ${libs}"
         echo "${libs}" >> ${lib}
         INCS="${INCS} ${incs}"
         echo "${incs}" >> ${inc}
      fi
      msg 'yes (deduced)'
      echo "${define}" >> ${h}
      eval have_${variable}=yes
      return 0
   else
      echo "/* ${define} */" >> ${h}
      msg 'no (deduced)'
      eval unset have_${variable}
      return 1
   fi
}

compile_check() {
   variable=$1 topic=$2 define=$3

   _check_preface "${variable}" "${topic}" "${define}"

   if ${make} -f ${makefile} XINCS="${INCS}" \
            CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" \
            ./${tmp}.o &&
         [ -f ./${tmp}.o ]; then
      msg 'yes'
      echo "${define}" >> ${h}
      eval have_${variable}=yes
      return 0
   else
      echo "/* ${define} */" >> ${h}
      msg 'no'
      eval unset have_${variable}
      return 1
   fi
}

_link_mayrun() {
   run=$1 variable=$2 topic=$3 define=$4 libs=$5 incs=$6

   _check_preface "${variable}" "${topic}" "${define}"

   if feat_yes CROSS_BUILD; then
      if [ ${run} = 1 ]; then
         run=0
      fi
   fi

   if ${make} -f ${makefile} XINCS="${INCS} ${incs}" \
            CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" \
            XLIBS="${LIBS} ${libs}" \
            ./${tmp} &&
         [ -f ./${tmp} ] &&
         { [ ${run} -eq 0 ] || ./${tmp}; }; then
      echo "*** adding INCS<${incs}> LIBS<${libs}>; executed: ${run}"
      msg 'yes'
      echo "${define}" >> ${h}
      LIBS="${LIBS} ${libs}"
      echo "${libs}" >> ${lib}
      INCS="${INCS} ${incs}"
      echo "${incs}" >> ${inc}
      eval have_${variable}=yes
      return 0
   else
      msg 'no'
      echo "/* ${define} */" >> ${h}
      eval unset have_${variable}
      return 1
   fi
}

link_check() {
   _link_mayrun 0 "${1}" "${2}" "${3}" "${4}" "${5}"
}

run_check() {
   _link_mayrun 1 "${1}" "${2}" "${3}" "${4}" "${5}"
}

xrun_check() {
   _link_mayrun 2 "${1}" "${2}" "${3}" "${4}" "${5}"
}

feat_def() {
   if feat_yes ${1}; then
      echo '#define HAVE_'${1}'' >> ${h}
      return 0
   else
      echo '/* OPT_'${1}'=0 */' >> ${h}
      return 1
   fi
}

squeeze_em() {
   < "${1}" > "${2}" ${awk} \
   'BEGIN {ORS = " "} /^[^#]/ {print} {next} END {ORS = ""; print "\n"}'
}

##  --  >8  - <<SUPPORT FUNS | RUNNING>> -  8<  --  ##

# First of all, create new configuration and check whether it changed

# Very easy checks for the operating system in order to be able to adjust paths
# or similar very basic things which we need to be able to go at all
os_early_setup

# Check those tools right now that we need before including $rc
msg 'Checking for basic utility set'
thecmd_testandset_fail awk awk
thecmd_testandset_fail rm rm
thecmd_testandset_fail tr tr

# Initialize the option set
msg_nonl 'Setting up configuration options ... '
option_setup
msg 'done'

# Include $rc, but only take from it what wasn't overwritten by the user from
# within the command line or from a chosen fixed CONFIG=
# Note we leave alone the values
trap "exit 1" HUP INT TERM
trap "${rm} -f ${tmp}" EXIT

msg_nonl 'Joining in %s ... ' ${rc}
option_join_rc
msg 'done'

# We need to know about that now, in order to provide utility overwrites etc.
os_setup

msg 'Checking for remaining set of utilities'
thecmd_testandset_fail grep grep

# Before we step ahead with the other utilities perform a path cleanup first.
path_check PATH

# awk(1) above
thecmd_testandset_fail basename basename
thecmd_testandset_fail cat cat
thecmd_testandset_fail chmod chmod
thecmd_testandset_fail cp cp
thecmd_testandset_fail cmp cmp
# grep(1) above
thecmd_testandset_fail mkdir mkdir
thecmd_testandset_fail mv mv
# rm(1) above
thecmd_testandset_fail sed sed
thecmd_testandset_fail sort sort
thecmd_testandset_fail tee tee
__PATH=${PATH}
thecmd_testandset chown chown ||
   PATH="/sbin:${PATH}" thecmd_set chown chown ||
   PATH="/usr/sbin:${PATH}" thecmd_set_fail chown chown
PATH=${__PATH}
thecmd_testandset_fail MAKE make
make=${MAKE}
export MAKE
thecmd_testandset strip strip && HAVE_STRIP=1 || HAVE_STRIP=0

# For ./cc-test.sh only
thecmd_testandset_fail cksum cksum

# Update OPT_ options now, in order to get possible inter-dependencies right
option_update

# (No functions since some shells loose non-exported variables in traps)
trap "trap \"\" HUP INT TERM; exit 1" HUP INT TERM
trap "trap \"\" HUP INT TERM EXIT;\
   ${rm} -rf ${newlst} ${tmp0}.* ${tmp0}* ${newmk} ${newev} ${newh}" EXIT

# Our configuration options may at this point still contain shell snippets,
# we need to evaluate them in order to get them expanded, and we need those
# evaluated values not only in our new configuration file, but also at hand..
msg_nonl 'Evaluating all configuration items ... '
option_evaluate
msg 'done'

printf "#define VAL_UAGENT \"${VAL_SID}${VAL_MAILX}\"\n" >> ${newh}
printf "VAL_UAGENT = ${VAL_SID}${VAL_MAILX}\n" >> ${newmk}

# The problem now is that the test should be able to run in the users linker
# and path environment, so we need to place the test: rule first, before
# injecting the relevant make variables.  Set up necessary environment
if [ -z "${VERBOSE}" ]; then
   printf -- "ECHO_CC = @echo '  'CC \$(@);\n" >> ${newmk}
   printf -- "ECHO_LINK = @echo '  'LINK \$(@);\n" >> ${newmk}
   printf -- "ECHO_GEN = @echo '  'GEN \$(@);\n" >> ${newmk}
   printf -- "ECHO_TEST = @\n" >> ${newmk}
   printf -- "ECHO_CMD = @echo '  CMD';\n" >> ${newmk}
   printf -- "ECHO_BLOCK_BEGIN = @( \n" >> ${newmk}
   printf -- "ECHO_BLOCK_END = ) >/dev/null\n" >> ${newmk}
fi
printf 'test: all\n\t$(ECHO_TEST)%s %scc-test.sh --check-only ./%s\n' \
   "${SHELL}" "${SRCDIR}" "${VAL_SID}${VAL_MAILX}" >> ${newmk}

# Add the known utility and some other variables
printf "#define VAL_PRIVSEP \"${VAL_SID}${VAL_MAILX}-privsep\"\n" >> ${newh}
printf "VAL_PRIVSEP = \$(VAL_UAGENT)-privsep\n" >> ${newmk}
if feat_yes DOTLOCK; then
   printf "OPTIONAL_PRIVSEP = \$(VAL_PRIVSEP)\n" >> ${newmk}
else
   printf "OPTIONAL_PRIVSEP =\n" >> ${newmk}
fi

for i in \
      SRCDIR \
      awk basename cat chmod chown cp cmp grep mkdir mv rm sed sort tee tr \
      MAKE MAKEFLAGS make SHELL strip \
      cksum; do
   eval j=\$${i}
   printf "${i} = ${j}\n" >> ${newmk}
   printf "${i}=${j}\n" >> ${newlst}
   printf "${i}=\"${j}\";export ${i}; " >> ${newev}
done
printf "\n" >> ${newev}

# Build a basic set of INCS and LIBS according to user environment.
C_INCLUDE_PATH="${CWDDIR}:${SRCDIR}:${C_INCLUDE_PATH}"
path_check C_INCLUDE_PATH -I _INCS
INCS="${INCS} ${_INCS}"
path_check LD_LIBRARY_PATH -L _LIBS
LIBS="${LIBS} ${_LIBS}"
unset _INCS _LIBS
export C_INCLUDE_PATH LD_LIBRARY_PATH

# Some environments need runtime path flags to be able to go at all
ld_runtime_flags

## Detect CC, whether we can use it, and possibly which CFLAGS we can use

cc_setup

${cat} > ${tmp}.c << \!
#include <stdio.h>
#include <string.h>
static void doit(char const *s);
int
main(int argc, char **argv){
   (void)argc;
   (void)argv;
   doit("Hello world");
   return 0;
}
static void
doit(char const *s){
   char buf[12];
   memcpy(buf, s, strlen(s) +1);
   puts(s);
}
!

if "${CC}" ${INCS} ${CFLAGS} ${EXTRA_CFLAGS} ${LDFLAGS} ${EXTRA_LDFLAGS} \
      -o ${tmp2} ${tmp}.c ${LIBS}; then
   :
else
   msg 'ERROR: i cannot compile a "Hello world" via'
   msg '   %s' \
   "${CC} ${INCS} ${CFLAGS} ${EXTRA_CFLAGS} ${LDFLAGS} ${EXTRA_LDFLAGS} ${LIBS}"
   msg 'ERROR:   Please read INSTALL, rerun'
   config_exit 1
fi

# This may also update ld_runtime_flags() (again)
cc_flags

for i in \
      INCS LIBS \
      ; do
   eval j=\$${i}
   printf -- "${i}=${j}\n" >> ${newlst}
done
for i in \
      CC \
      CFLAGS \
      LDFLAGS \
      PATH C_INCLUDE_PATH LD_LIBRARY_PATH \
      OSFULLSPEC \
      ; do
   eval j=\$${i}
   printf -- "${i} = ${j}\n" >> ${newmk}
   printf -- "${i}=${j}\n" >> ${newlst}
done

# Now finally check whether we already have a configuration and if so, whether
# all those parameters are still the same.. or something has actually changed
if [ -f ${lst} ] && ${cmp} ${newlst} ${lst} >/dev/null 2>&1; then
   echo 'Configuration is up-to-date'
   exit 0
elif [ -f ${lst} ]; then
   echo 'Configuration has been updated..'
   ( eval "${MAKE} -f ./mk-config.mk clean" )
   echo
else
   echo 'Shiny configuration..'
fi

# Time to redefine helper 1
config_exit() {
   ${rm} -f ${lst} ${h} ${mk}
   exit ${1}
}

${mv} -f ${newlst} ${lst}
${mv} -f ${newev} ${ev}
${mv} -f ${newh} ${h}
${mv} -f ${newmk} ${mk}

## Compile and link checking

tmp3=./${tmp0}3$$
log=./mk-config.log
lib=./mk-config.lib
inc=./mk-config.inc
makefile=./${tmp0}.mk

# (No function since some shells loose non-exported variables in traps)
trap "trap \"\" HUP INT TERM;\
   ${rm} -f ${lst} ${h} ${mk} ${lib} ${inc}; exit 1" HUP INT TERM
trap "trap \"\" HUP INT TERM EXIT;\
   ${rm} -rf ${tmp0}.* ${tmp0}*" EXIT

# Time to redefine helper 2
msg() {
   fmt=${1}
   shift
   printf "*** ${fmt}\\n" "${@}"
   printf -- "${fmt}\\n" "${@}" >&5
}
msg_nonl() {
   fmt=${1}
   shift
   printf "*** ${fmt}\\n" "${@}"
   printf -- "${fmt}" "${@}" >&5
}

# !!
exec 5>&2 > ${log} 2>&1

echo "${LIBS}" > ${lib}
echo "${INCS}" > ${inc}
${cat} > ${makefile} << \!
.SUFFIXES: .o .c .x .y
.c.o:
	$(CC) -I./ $(XINCS) $(CFLAGS) -c $(<)
.c.x:
	$(CC) -I./ $(XINCS) -E $(<) > $(@)
.c:
	$(CC) -I./ $(XINCS) $(CFLAGS) $(LDFLAGS) -o $(@) $(<) $(XLIBS)
!

## Generics

# May be multiline..
echo >> ${h}
[ -n "${OS_DEFINES}" ] && printf -- "${OS_DEFINES}" >> ${h}
echo '#define VAL_BUILD_OS "'"${OS}"'"' >> ${h}
echo '#define VAL_BUILD_OSENV "'"${OSENV}"'"' >> ${h}

# Generate n_err_number OS mappings
dump_test_program=0
(
   feat_yes DEVEL && NV= || NV=noverbose
   SRCDIR="${SRCDIR}" TARGET="${h}" awk="${awk}" \
      ${SHELL} "${SRCDIR}"make-errors.sh ${NV} config
) | xrun_check oserrno 'OS error mapping table generated' || config_exit 1
dump_test_program=1

feat_def ALWAYS_UNICODE_LOCALE
feat_def CROSS_BUILD

feat_def ASAN_ADDRESS
feat_def ASAN_MEMORY
feat_def DEBUG
feat_def DEVEL
feat_def NYD2
feat_def NOMEMDBG

if xrun_check inline 'inline functions' \
   '#define HAVE_INLINE
   #define n_INLINE static inline' << \!
static inline int ilf(int i){return ++i;}
int main(void){return ilf(-1);}
!
then
   :
elif xrun_check inline 'inline functions (via __inline)' \
   '#define HAVE_INLINE
   #define n_INLINE static __inline' << \!
static __inline int ilf(int i){return ++i;}
int main(void){return ilf(-1);}
!
then
   :
fi

## Test for "basic" system-calls / functionality that is used by all parts
## of our program.  Once this is done fork away BASE_LIBS and other BASE_*
## macros to be used by only the subprograms (potentially).

if run_check clock_gettime 'clock_gettime(2)' \
   '#define HAVE_CLOCK_GETTIME' << \!
#include <time.h>
# include <errno.h>
int main(void){
   struct timespec ts;

   if(!clock_gettime(CLOCK_REALTIME, &ts) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
elif run_check clock_gettime 'clock_gettime(2) (via -lrt)' \
   '#define HAVE_CLOCK_GETTIME' '-lrt' << \!
#include <time.h>
# include <errno.h>
int main(void){
   struct timespec ts;

   if(!clock_gettime(CLOCK_REALTIME, &ts) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
elif run_check gettimeofday 'gettimeofday(2)' \
   '#define HAVE_GETTIMEOFDAY' << \!
#include <stdio.h> /* For C89 NULL */
#include <sys/time.h>
# include <errno.h>
int main(void){
   struct timeval tv;

   if(!gettimeofday(&tv, NULL) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
else
   have_no_subsecond_time=1
fi

if run_check nanosleep 'nanosleep(2)' \
   '#define HAVE_NANOSLEEP' << \!
#include <time.h>
# include <errno.h>
int main(void){
   struct timespec ts;

   ts.tv_sec = 1;
   ts.tv_nsec = 100000;
   if(!nanosleep(&ts, NULL) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
elif run_check nanosleep 'nanosleep(2) (via -lrt)' \
   '#define HAVE_NANOSLEEP' '-lrt' << \!
#include <time.h>
# include <errno.h>
int main(void){
   struct timespec ts;

   ts.tv_sec = 1;
   ts.tv_nsec = 100000;
   if(!nanosleep(&ts, NULL) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
# link_check is enough for this, that function is so old, trust the proto
elif link_check sleep 'sleep(3)' \
   '#define HAVE_SLEEP' << \!
#include <unistd.h>
# include <errno.h>
int main(void){
   if(!sleep(1) || errno != ENOSYS)
      return 0;
   return 1;
}
!
then
   :
else
   msg 'ERROR: we require one of nanosleep(2) and sleep(3).'
   config_exit 1
fi

if run_check userdb 'gete?[gu]id(2), getpwuid(3), getpwnam(3)' << \!
#include <pwd.h>
#include <unistd.h>
# include <errno.h>
int main(void){
   struct passwd *pw;
   gid_t gid;
   uid_t uid;

   if((gid = getgid()) != 0)
      gid = getegid();
   if((uid = getuid()) != 0)
      uid = geteuid();
   if((pw = getpwuid(uid)) == NULL && errno == ENOSYS)
      return 1;
   if((pw = getpwnam("root")) == NULL && errno == ENOSYS)
      return 1;
   return 0;
}
!
then
   :
else
   msg 'ERROR: we require user and group info / database searches.'
   msg 'That much Unix we indulge ourselfs.'
   config_exit 1
fi

if link_check ftruncate 'ftruncate(2)' \
   '#define HAVE_FTRUNCATE' << \!
#include <unistd.h>
#include <sys/types.h>
int main(void){
   return (ftruncate(0, 0) != 0);
}
!
then
   :
else
   msg 'ERROR: we require the ftruncate(2) system call.'
   config_exit 1
fi

if link_check snprintf 'snprintf(3)' << \!
#include <stdio.h>
int main(void){
   char b[20];

   snprintf(b, sizeof b, "%s", "string");
   return 0;
}
!
then
   :
else
   msg 'ERROR: we require the snprintf(3) function.'
   config_exit 1
fi

if link_check environ 'environ(3)' << \!
#include <stdio.h> /* For C89 NULL */
int main(void){
   extern char **environ;

   return environ[0] == NULL;
}
!
then
   :
else
   msg 'ERROR: we require the environ(3) array for subprocess control.'
   config_exit 1
fi

if link_check setenv '(un)?setenv(3)' '#define HAVE_SETENV' << \!
#include <stdlib.h>
int main(void){
   setenv("s-mailx", "i want to see it cute!", 1);
   unsetenv("s-mailx");
   return 0;
}
!
then
   :
elif link_check setenv 'putenv(3)' '#define HAVE_PUTENV' << \!
#include <stdlib.h>
int main(void){
   putenv("s-mailx=i want to see it cute!");
   return 0;
}
!
then
   :
else
   msg 'ERROR: we require either the setenv(3) or putenv(3) functions.'
   config_exit 1
fi

## optional stuff

if link_check vsnprintf 'vsnprintf(3)' << \!
#include <stdarg.h>
#include <stdio.h>
static void dome(char *buf, size_t blen, ...){
   va_list ap;

   va_start(ap, blen);
   vsnprintf(buf, blen, "%s", ap);
   va_end(ap);
}
int main(void){
   char b[20];

   dome(b, sizeof b, "string");
   return 0;
}
!
then
   :
else
   feat_bail_required ERRORS
fi

if [ "${have_vsnprintf}" = yes ]; then
   __va_copy() {
      link_check va_copy "va_copy(3) (as ${2})" \
         "#define HAVE_N_VA_COPY
#define n_va_copy ${2}" <<_EOT
#include <stdarg.h>
#include <stdio.h>
#if ${1}
# if defined __va_copy && !defined va_copy
#  define va_copy __va_copy
# endif
#endif
static void dome2(char *buf, size_t blen, va_list src){
   va_list ap;

   va_copy(ap, src);
   vsnprintf(buf, blen, "%s", ap);
   va_end(ap);
}
static void dome(char *buf, size_t blen, ...){
   va_list ap;

   va_start(ap, blen);
   dome2(buf, blen, ap);
   va_end(ap);
}
int main(void){
   char b[20];

   dome(b, sizeof b, "string");
   return 0;
}
_EOT
   }
   __va_copy 0 va_copy || __va_copy 1 __va_copy
fi

run_check pathconf 'f?pathconf(2)' '#define HAVE_PATHCONF' << \!
#include <unistd.h>
#include <errno.h>
int main(void){
   int rv = 0;

   errno = 0;
   rv |= !(pathconf(".", _PC_NAME_MAX) >= 0 || errno == 0 || errno != ENOSYS);
   errno = 0;
   rv |= !(pathconf(".", _PC_PATH_MAX) >= 0 || errno == 0 || errno != ENOSYS);

   /* Only link check */
   fpathconf(0, _PC_NAME_MAX);

   return rv;
}
!

run_check pipe2 'pipe2(2)' '#define HAVE_PIPE2' << \!
#include <fcntl.h>
#include <unistd.h>
# include <errno.h>
int main(void){
   int fds[2];

   if(!pipe2(fds, O_CLOEXEC) || errno != ENOSYS)
      return 0;
   return 1;
}
!

link_check tcgetwinsize 'tcgetwinsize(3)' '#define HAVE_TCGETWINSIZE' << \!
#include <termios.h>
int main(void){
   struct winsize ws;

   tcgetwinsize(0, &ws);
   return 0;
}
!

# We use this only then for now (need NOW+1)
run_check utimensat 'utimensat(2)' '#define HAVE_UTIMENSAT' << \!
#include <fcntl.h> /* For AT_* */
#include <sys/stat.h>
# include <errno.h>
int main(void){
   struct timespec ts[2];

   ts[0].tv_nsec = UTIME_NOW;
   ts[1].tv_nsec = UTIME_OMIT;
   if(!utimensat(AT_FDCWD, "", ts, 0) || errno != ENOSYS)
      return 0;
   return 1;
}
!

##

# XXX Add POSIX check once standardized
if link_check posix_random 'arc4random(3)' '#define HAVE_POSIX_RANDOM 0' << \!
#include <stdlib.h>
int main(void){
   arc4random();
   return 0;
}
!
then
   :
elif link_check getrandom 'getrandom(2) (in sys/random.h)' \
      '#define HAVE_GETRANDOM(B,S) getrandom(B, S, 0)
      #define HAVE_GETRANDOM_HEADER <sys/random.h>' <<\!
#include <sys/random.h>
int main(void){
   char buf[256];
   getrandom(buf, sizeof buf, 0);
   return 0;
}
!
then
   :
elif link_check getrandom 'getrandom(2) (via syscall(2))' \
      '#define HAVE_GETRANDOM(B,S) syscall(SYS_getrandom, B, S, 0)
      #define HAVE_GETRANDOM_HEADER <sys/syscall.h>' <<\!
#include <sys/syscall.h>
int main(void){
   char buf[256];
   syscall(SYS_getrandom, buf, sizeof buf, 0);
   return 0;
}
!
then
   :
elif [ -n "${have_no_subsecond_time}" ]; then
   msg 'ERROR: %s %s' 'without a native random' \
      'one of clock_gettime(2) and gettimeofday(2) is required.'
   config_exit 1
fi

link_check putc_unlocked 'putc_unlocked(3)' '#define HAVE_PUTC_UNLOCKED' <<\!
#include <stdio.h>
int main(void){
   putc_unlocked('@', stdout);
   return 0;
}
!

if run_check realpath 'realpath(3)' '#define HAVE_REALPATH' << \!
#include <stdlib.h>
int main(void){
   char x_buf[4096], *x = realpath(".", x_buf);

   return (x != NULL) ? 0 : 1;
}
!
then
   if run_check realpath_malloc 'realpath(3) takes NULL' \
         '#define HAVE_REALPATH_NULL' << \!
#include <stdlib.h>
int main(void){
   char *x = realpath(".", NULL);

   if(x != NULL)
      free(x);
   return (x != NULL) ? 0 : 1;
}
!
   then
      :
   fi
fi

## optional and selectable

## Now it is the time to fork away the BASE_ series

${rm} -f ${tmp}
squeeze_em ${inc} ${tmp}
${mv} ${tmp} ${inc}
squeeze_em ${lib} ${tmp}
${mv} ${tmp} ${lib}

echo "BASE_LIBS = `${cat} ${lib}`" >> ${mk}
echo "BASE_INCS = `${cat} ${inc}`" >> ${mk}

## The remains are expected to be used only by the main MUA binary!

OPT_LOCALES=0
link_check setlocale 'setlocale(3)' '#define HAVE_SETLOCALE' << \!
#include <locale.h>
int main(void){
   setlocale(LC_ALL, "");
   return 0;
}
!
[ -n "${have_setlocale}" ] && OPT_LOCALES=1

OPT_MULTIBYTE_CHARSETS=0
OPT_WIDE_GLYPHS=0
OPT_TERMINAL_CHARSET=0
if [ -n "${have_setlocale}" ]; then
   link_check c90amend1 'ISO/IEC 9899:1990/Amendment 1:1995' \
      '#define HAVE_C90AMEND1' << \!
#include <limits.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>
int main(void){
   char mbb[MB_LEN_MAX + 1];
   wchar_t wc;

   iswprint(L'c');
   towupper(L'c');
   mbtowc(&wc, "x", 1);
   mbrtowc(&wc, "x", 1, NULL);
   wctomb(mbb, wc);
   return (mblen("\0", 1) == 0);
}
!
   [ -n "${have_c90amend1}" ] && OPT_MULTIBYTE_CHARSETS=1

   if [ -n "${have_c90amend1}" ]; then
      link_check wcwidth 'wcwidth(3)' '#define HAVE_WCWIDTH' << \!
#include <wchar.h>
int main(void){
   wcwidth(L'c');
   return 0;
}
!
      [ -n "${have_wcwidth}" ] && OPT_WIDE_GLYPHS=1
   fi

   link_check nl_langinfo 'nl_langinfo(3)' '#define HAVE_NL_LANGINFO' << \!
#include <langinfo.h>
#include <stdlib.h>
int main(void){
   nl_langinfo(DAY_1);
   return (nl_langinfo(CODESET) == NULL);
}
!
   [ -n "${have_nl_langinfo}" ] && OPT_TERMINAL_CHARSET=1
fi # have_setlocale

## optional and selectable

if feat_yes ICONV; then
   ${cat} > ${tmp2}.c << \!
#include <stdio.h> /* For C89 NULL */
#include <iconv.h>
int main(void){
   iconv_t id;

   id = iconv_open("foo", "bar");
   iconv(id, NULL, NULL, NULL, NULL);
   iconv_close(id);
   return 0;
}
!
   < ${tmp2}.c link_check iconv 'iconv(3) functionality' \
         '#define HAVE_ICONV' ||
      < ${tmp2}.c link_check iconv 'iconv(3) functionality (via -liconv)' \
         '#define HAVE_ICONV' '-liconv' ||
      feat_bail_required ICONV
else
   echo '/* OPT_ICONV=0 */' >> ${h}
fi # feat_yes ICONV

if feat_yes TERMCAP; then
   __termcaplib() {
      link_check termcap "termcap(5) (via ${4})" \
         "#define HAVE_TERMCAP${3}" "${1}" << _EOT
#include <stdio.h>
#include <stdlib.h>
${2}
#include <term.h>
#define UNCONST(P) ((void*)(unsigned long)(void const*)(P))
static int my_putc(int c){return putchar(c);}
int main(void){
   char buf[1024+512], cmdbuf[2048], *cpb, *r1;
   int r2 = OK, r3 = ERR;

   tgetent(buf, getenv("TERM"));
   cpb = cmdbuf;
   r1 = tgetstr(UNCONST("cm"), &cpb);
   tgoto(r1, 1, 1);
   r2 = tgetnum(UNCONST("Co"));
   r3 = tgetflag(UNCONST("ut"));
   tputs("cr", 1, &my_putc);
   return (r1 == NULL || r2 == -1 || r3 == 0);
}
_EOT
   }

   __terminfolib() {
      link_check terminfo "terminfo(5) (via ${2})" \
         '#define HAVE_TERMCAP
         #define HAVE_TERMCAP_CURSES
         #define HAVE_TERMINFO' "${1}" << _EOT
#include <stdio.h>
#include <curses.h>
#include <term.h>
#define UNCONST(P) ((void*)(unsigned long)(void const*)(P))
static int my_putc(int c){return putchar(c);}
int main(void){
   int er, r0, r1, r2;
   char *r3, *tp;

   er = OK;
   r0 = setupterm(NULL, 1, &er);
   r1 = tigetflag(UNCONST("bce"));
   r2 = tigetnum(UNCONST("colors"));
   r3 = tigetstr(UNCONST("cr"));
   tp = tparm(r3, NULL, NULL, 0,0,0,0,0,0,0);
   tputs(tp, 1, &my_putc);
   return (r0 == ERR || r1 == -1 || r2 == -2 || r2 == -1 ||
      r3 == (char*)-1 || r3 == NULL);
}
_EOT
   }

   if feat_yes TERMCAP_VIA_TERMINFO; then
      __terminfolib -ltinfo -ltinfo ||
         __terminfolib -lcurses -lcurses ||
         __terminfolib -lcursesw -lcursesw ||
         feat_bail_required TERMCAP_VIA_TERMINFO
   fi

   if [ -z "${have_terminfo}" ]; then
      __termcaplib -ltermcap '' '' '-ltermcap' ||
         __termcaplib -ltermcap '#include <curses.h>' '
            #define HAVE_TERMCAP_CURSES' \
            'curses.h / -ltermcap' ||
         __termcaplib -lcurses '#include <curses.h>' '
            #define HAVE_TERMCAP_CURSES' \
            'curses.h / -lcurses' ||
         __termcaplib -lcursesw '#include <curses.h>' '
            #define HAVE_TERMCAP_CURSES' \
            'curses.h / -lcursesw' ||
         feat_bail_required TERMCAP

      if [ -n "${have_termcap}" ]; then
         run_check tgetent_null \
            "tgetent(3) of termcap(5) takes NULL buffer" \
            "#define HAVE_TGETENT_NULL_BUF" << _EOT
#include <stdio.h> /* For C89 NULL */
#include <stdlib.h>
#ifdef HAVE_TERMCAP_CURSES
# include <curses.h>
#endif
#include <term.h>
int main(void){
   tgetent(NULL, getenv("TERM"));
   return 0;
}
_EOT
      fi
   fi
else
   echo '/* OPT_TERMCAP=0 */' >> ${h}
   echo '/* OPT_TERMCAP_VIA_TERMINFO=0 */' >> ${h}
fi

## Summarizing

${rm} -f ${tmp}
squeeze_em ${inc} ${tmp}
${mv} ${tmp} ${inc}
squeeze_em ${lib} ${tmp}
${mv} ${tmp} ${lib}

# mk-config.h
${mv} ${h} ${tmp}
printf '#ifndef n_MK_CONFIG_H\n# define n_MK_CONFIG_H 1\n' > ${h}
${cat} ${tmp} >> ${h}
${rm} -f ${tmp}
printf '\n' >> ${h}

# Create the string that is used by *features* and `version'.
# Take this nice opportunity and generate a visual listing of included and
# non-included features for the person who runs the configuration
msg '\nThe following features are included (+) or not (-):'
set -- ${OPTIONS_DETECT} ${OPTIONS} ${OPTIONS_XTRA}
printf '/* The "feature string" */\n' >> ${h}
# Because + is expanded by *folder* if first in "echo $features", put something
printf '#define VAL_FEATURES_CNT '${#}'\n#define VAL_FEATURES "#' >> ${h}
sep=
for opt
do
   sdoc=`option_doc_of ${opt}`
   [ -z "${sdoc}" ] && continue
   sopt="`echo ${opt} | ${tr} '[A-Z]_' '[a-z]-'`"
   feat_yes ${opt} && sign=+ || sign=-
   printf -- "${sep}${sign}${sopt}" >> ${h}
   sep=','
   msg " %s %s: %s" ${sign} ${sopt} "${sdoc}"
done
# TODO instead of using sh+tr+awk+printf, use awk, drop option_doc_of, inc here
#exec 5>&1 >>${h}
#${awk} -v opts="${OPTIONS_DETECT} ${OPTIONS} ${OPTIONS_XTRA}" \
#   -v xopts="${XOPTIONS_DETECT} ${XOPTIONS} ${XOPTIONS_XTRA}" \
printf '"\n' >> ${h}

# Create the real mk-config.mk
# Note we cannout use explicit ./ filename prefix for source and object
# pathnames because of a bug in bmake(1)
${rm} -rf ${tmp0}.* ${tmp0}*
srclist= objlist=
for i in `printf '%s\n' "${SRCDIR}"*.c | ${sort}`; do
   i=`basename "${i}" .c`
   if [ "${i}" = privsep ]; then
      continue
   fi
   objlist="${objlist} ${i}.o"
   srclist="${srclist} \$(SRCDIR)${i}.c"
   printf '%s: %s\n\t$(ECHO_CC)$(CC) $(CFLAGS) $(INCS) -c %s\n' \
      "${i}.o" "\$(SRCDIR)${i}.c" "\$(SRCDIR)${i}.c" >> ${mk}
done
printf 'OBJ_SRC = %s\nOBJ = %s\n' "${srclist}" "${objlist}" >> "${mk}"

printf '#endif /* n_MK_CONFIG_H */\n' >> ${h}

echo "LIBS = `${cat} ${lib}`" >> ${mk}
echo "INCS = `${cat} ${inc}`" >> ${mk}
echo >> ${mk}
${cat} "${SRCDIR}"make-config.in >> ${mk}

## Finished!

msg '\nSetup:'
msg ' . System-wide resource file: %s/%s' "${VAL_SYSCONFDIR}" "${VAL_SYSCONFRC}"
msg ' . bindir: %s' "${VAL_BINDIR}"
if feat_yes DOTLOCK; then
   msg ' . libexecdir: %s' "${VAL_LIBEXECDIR}"
fi
msg ' . mandir: %s' "${VAL_MANDIR}"
msg ''

# s-it-mode
