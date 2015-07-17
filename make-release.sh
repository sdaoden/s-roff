#!/bin/sh -
#@ make-release.sh: simple somewhat generic release builder

# In order to be able to remove the release scripts from the release tarball,
# we must delete them, which some shells may not like while they are running.
# So be safe and move instances temporarily to .git/, the .inc will remove them
if [ "`basename \`pwd\``" != .git ]; then
   cp make-release.* .git/
   cd .git
   exec sh make-release.sh
fi
cd ..

command -v perl >/dev/null 2>&1 || exit 42

## Variables

: ${PROGRAM:=s-roff}
: ${UPROGRAM:=S-roff}
: ${MANUAL:=code-roff.html}

: ${UPLOAD:=steffen@sdaoden.eu:/var/www/localhost/downloads}

# Mail
: ${MAILX:=s-nail -Snofollowup-to -Sreply-to=roff -Ssmime-sign}
: ${ACCOUNT:=ich}
: ${MAILBCC:=roff-announce-bcc}
: ${MAILTO:=roff-announce}

## Hooks

update_stable_hook() (
   cd man || exit 99
   for f in *.*; do
      echo ${f}': expanding MKREL'
      < ${f} > ${f}.x awk '
         BEGIN{ written = 0 }
         /\.\\"--MKREL-START--/, /\.\\"--MKREL-END--/ {
            if (written++ != 0)
               next
            print ".\\\"--MKREL-START--"
            print ".\\\"@ '"${UPROGRAM}"' v'"${REL}"' / '"${DATE_ISO}"'"
            print ".ds DD '"${DATE_MAN}"'"
            print ".ds VV \\\\%v'"${REL}"'"
            print ".\\\"--MKREL-END--"
            next
         }
         {print}
      ' &&
      mv -f ${f}.x ${f}
      git add ${f}
   done
)

update_release_hook() {
   :
}

. ./make-release.inc

# s-sh-mode
