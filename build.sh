#!/bin/bash

# build.sh
# assemble AnyElf plugin for Double Commander
#
# usage:
# ./build.sh %SDK% [devmode]
# SDK:     qt5 or gtk
# devmode: (any char) developer only: don't download/pull ELFIO repo

CRESET="\e[0m"
CYELLOW="\e[93m"
CRED="\e[91m"
CGREEN="\e[92m"

echo_yellow()
{
    echo -e ${CYELLOW}"\n$1"${CRESET}
}

echo_red()
{
    set -e
    echo -e ${CRED}"\n$1"${CRESET}
    exit 1
}

echo_green()
{
    echo -e ${CGREEN}"\n$1"${CRESET}
}

SDK="$1"

[[ "${SDK}" != qt5 && "${SDK}" != gtk ]] && echo_red "Usage:\n./build.sh gtk\nor\n./build.sh qt5\n"

[ ${SDK} == gtk ] && PACKET=libgtk2.0-dev || PACKET=qtbase5-dev

PACKAGE_INSTALLED=$(apt list --installed 2>/dev/null | grep -c ${PACKET})

if [[ "${PACKAGE_INSTALLED}" == "0" || "${PACKAGE_INSTALLED}" == "" ]]; then
    echo_yellow "${PACKET} is not istalled\nrun the following command to install it:"
    echo -e "sudo apt install ${PACKET}\n\n"
    exit 1
fi

HERE=$(pwd)
ELFIO_DIR=../ELFIO
DEVMODE=0
[ -z "$2" ] || DEVMODE=1

if [ ${DEVMODE} -eq 0 ]; then
    if [ ! -d ${ELFIO_DIR} ]; then
        cd ..
        echo_yellow "Cloning ELFIO..."
        git clone https://github.com/serge1/ELFIO.git || echo_red "Can't clone ELFIO"
        cd ${HERE}
    else
        cd ${ELFIO_DIR}
        git pull
        cd ${HERE}
    fi
fi

TARGETNAME=anyelf_${SDK}.wlx
make clean SDK=${SDK}
make -j$(nproc) SDK=${SDK} || echo_red "Can't build ${TARGETNAME}"
make strip SDK=${SDK} || echo_red "Can't strip ${TARGETNAME}"

echo_green "${TARGETNAME} assembled"

echo_yellow "Don't forget to add the following block to your doublecmd.xml"
echo -e \
"\n"\
"<WlxPlugins>\n"\
"    <WlxPlugin Enabled=\"True\">\n"\
"        <Name>anyelf_${SDK}</Name>\n"\
"        <Path>%COMMANDER_PATH%/plugins/wlx/anyelf/anyelf_${SDK}.wlx</Path>\n"\
"        <DetectString>EXT=\"*\"</DetectString>\n"\
"    </WlxPlugin>\n"\
"</WlxPlugins>\n\n"