#!/bin/bash

# Date: 2014-09-13
# Author: TOSH
#
# This script backup directories on a remote server
# The files are compressed and crypted with openssl utility
#
# See options for details

set -e

# Options, you can edit

DIRS="/etc /home"
EXCLUDE_STRING='*~ lost+found _* films Downloads .macromedia .cache .claws-mail *VirtualBox* git .irssi/log .local .mozilla'
EXCLUDE_FILE='/tmp/exclude_file.txt'
ENCRYPTION='aes-256-ofb'

HOSTNAME=$(hostname)

function usage() {
    echo "Usage : $1 [OPTIONS]"
    echo "   -h           help"
    echo "   -d <dirs>    List of directories to backup"
    echo "   -e <excl>    Paterns to exclude"
    echo "   -k <pass>    Specify the passphrase for encryption"
    exit 1
}

while getopts ":hd:e:k:" opt
do
    case $opt in
	"h")
	    usage $0
	    ;;
	"s")
	    SERVER=$OPTARG
	    ;;
	"d")
	    DIRS=$OPTARG
	    ;;
	"e")
	    EXCLUDE_STRING=$OPTARG
	    ;;
	"k")
	    PASSPHRASE=$OPTARG
	    ;;
	*)
	    usage $0
	    ;;
    esac
done

# Check UID, you probably need root access to backup system files
if [[ $EUID -ne 0 ]]
then
    echo "Script must be run as root"
    exit 1
fi

# Check if openssl is installed
if [[ $CRYPT -ne 0 ]]
then
    if [[ -z `which openssl` ]]
    then
	echo "openssl not installed"
	exit 1
    fi
fi

# Dump EXCLUDE_STRING to file
for i in $EXCLUDE_STRING
do
    echo $i >> $EXCLUDE_FILE
done


BACKUP_TMP="backup_"$HOSTNAME"_"$(date "+%Y_%d_%m")
BACKUP_TAR=$BACKUP_TMP".tar.gz"
BACKUP_CRYPT=$BACKUP_TAR".crypt"

# Go home
cd

# Detete old backups
rm -rf $BACKUP_TMP $BACKUP_TAR $BACKUP_CRYPT

mkdir $BACKUP_TMP

# Copy files to temporary directory
for dir in $DIRS
do
    rsync -avr --progress --relative \
	  --exclude-from $EXCLUDE_FILE \
	  $dir $BACKUP_TMP
done


# Compress backup
tar cvzf $BACKUP_TAR $BACKUP_TMP

# Crypt backup
if [[ -z $PASSPHRASE ]]
then
    openssl enc -e -$ENCRYPTION -in $BACKUP_TAR -out $BACKUP_CRYPT
else
    openssl enc -e -$ENCRYPTION -in $BACKUP_TAR -out $BACKUP_CRYPT -k $PASSPHRASE
fi

rm -rf $BACKUP_TMP $BACKUP_TAR

echo "[+] Done, your backup is available at : "$(pwd)"/"$BACKUP_CRYPT

exit 0
