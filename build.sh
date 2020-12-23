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

here=$(pwd)
ELFIO_DIR=../ELFIO
if [ ! -d $ELFIO_DIR ]; then
	cd ..
	echo_yellow "Cloning ELFIO..."
	git clone https://github.com/serge1/ELFIO.git || echo_red "Can't clone ELFIO"
	cd $here
else
	cd $ELFIO_DIR
	git pull
	cd $here
fi

PACKET=libgtk2.0-dev

HAS_GTK_HEADERS=$(apt list --installed 2>/dev/null | grep -c $PACKET)

if [[ "$HAS_GTK_HEADERS" == "0" || "$HAS_GTK_HEADERS" == "" ]]; then
	echo_yellow "$PACKET is not istalled\nrun the following command to install it:"
	echo -e "sudo apt install $PACKET\n\n"
	exit 1
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
