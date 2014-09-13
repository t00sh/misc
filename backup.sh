#!/bin/sh

# Date: 2014-09-13
# Author: TOSH
#
# This script backup directories on a remote server
# The files are compressed and crypted with ccrypt utility
#
# Files which begin with '_' arn't backup
# See options for details

set -e

HOSTNAME=`hostname`
SERVER="backup@10.8.0.1"
SERVER_DIR="/var/backups/BACKUPS/$HOSTNAME"
DIRS="/etc /home"
CRYPT=1

function usage() {
    echo "Usage : $1 [OPTIONS]"
    echo "   -h           help"
    echo "   -s <server>  Remote backup server (ex: user@ip)"
    echo "   -b <dir>     Remote dir where backup will be sent"
    echo "   -d <dirs>    List of directories to backup"
    echo "   -n           Don't compress and don't crypt"
    exit 1
}

while getopts ":hs:b:d:n" opt
do
    case $opt in
	"h")
	    usage $0
	    ;;
	"s")
	    SERVER=$OPTARG
	    ;;
	"b")
	    SERVER_DIRS=$OPTARG
	    ;;
	"d")
	    DIRS=$OPTARG
	    ;;
	"n")
	    CRYPT=0
	    ;;
	*)
	    usage $0
	    ;;
    esac
done

if [[ $EUID -ne 0 ]]
then
    echo "Script must be run as root"
    exit 1
fi

if [[ $CRYPT -ne 0 ]]
then
    if [[ -z `which ccrypt` ]]
    then
	echo "Ccrypt not installed"
	exit 1
    fi
fi

BACKUP_TMP="backup"

cd
rm -rf $BACKUP_TMP
mkdir $BACKUP_TMP

for dir in $DIRS
do
    rsync -avr --progress --relative --delete \
	--exclude '*~' --exclude '_*' --exclude 'lost+found' \
	$dir $BACKUP_TMP
done

if [[ $CRYPT -ne 0 ]]
then
    tar cvzf "$BACKUP_TMP.tar.gz" $BACKUP_TMP
    ccrypt -e "$BACKUP_TMP.tar.gz"
    rsync -avr --progress --delete \
	"$BACKUP_TMP.tar.gz.cpt" $SERVER:$SERVER_DIR
else
      rsync -avr --progress --delete \
	"$BACKUP_TMP" $SERVER:$SERVER_DIR
fi

rm -rf "$BACKUP_TMP" "$BACKUP_TMP.tar.gz" "$BACKUP_TMP.tar.gz.cpt"

exit 0
