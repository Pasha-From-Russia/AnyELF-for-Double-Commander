#!/bin/bash

CRESET="\e[0m"
CYELLOW="\e[93m"
CRED="\e[91m"
CGREEN="\e[92m"

echo_yellow()
{
	echo -e $CYELLOW"$1"$CRESET
}

echo_red()
{
	set -e
	echo -e $CRED"$1"$CRESET
	exit 1
}

echo_green()
{
	echo -e $CGREEN"$1"$CRESET
}

if [ ! -d ../ELFIO ]; then
	here=$(pwd)
	cd ..
	echo_yellow "Cloning ELFIO..."
	git clone https://github.com/serge1/ELFIO.git || echo_red "Can't clone ELFIO"
	cd $here
fi

DIRECTIVE=""
elfio=$(awk '/PACKAGE_VERSION=/ {split($0, a, "="); printf "%s", a[2]}' ../ELFIO/configure 2>/dev/null | sed s/"'"//g)
[ "$elfio" != "" ] && DIRECTIVE="DIRECTIVE=-DELFIO_VERSION=\"$elfio\""

TARGETNAME=anyelf_gtk.wlx
make clean
if [ "$DIRECTIVE" != "" ]; then
	make -j2 "$DIRECTIVE"
else
	make -j2
fi
[ $? -ne 0 ] && echo_red "Can't build $TARGETNAME"
make strip
[ $? -ne 0 ] && echo_red "Can't strip $TARGETNAME"

echo_green "$TARGETNAME assembled"
