#!/bin/bash

# Date: 2015-06-16
# Author: TOSH
#
# This script backup a file on a remote server
# Prepare backup with gen_backup.sh script
#
# See options for details

# Options, you can edit

HOSTNAME=$(hostname)
SERVER_DIR="/home/backup/$HOSTNAME"
SERVER_HOST="10.8.0.1"
SERVER_USER="backup"
SERVER_PASSWORD=''
FILENAME="backup_"$HOSTNAME"_"$(date "+%Y_%d_%m")".tar.gz.crypt"
USE_FTP=0


function usage() {
    echo "Usage : $1 [OPTIONS]"
    echo "   -h           help"
    echo "   -d <dir>     Remote server path"
    echo "   -f           Use ftp instead of rsync/ssh"
    echo "   -H <host>    Hostname serveur"
    echo "   -u <user>    Username"
    echo "   -F <file>    Select file to backup"
    echo "   -p <pass>    Password for FTP"
    exit 1
}

while getopts ":hd:fH:u:F:p:" opt
do
    case $opt in
	"h")
	    usage $0
	    ;;
	"d")
	    SERVER_DIR=$OPTARG
	    ;;
	"f")
	    USE_FTP=1
	    ;;
	"H")
	    SERVER_HOST=$OPTARG
	    ;;
	"u")
	    SERVER_USER=$OPTARG
	    ;;
	"F")
	    FILENAME=$OPTARG
	    ;;
	"p")
	    SERVER_PASSWORD=$OPTARG
	    ;;
	*)
	    usage $0
	    ;;
    esac
done

if [[ $USE_FTP -eq 1 ]]
then
    ftp -in <<EOF
    open $SERVER_HOST
    user $SERVER_USER $SERVER_PASSWORD
    bin
    verbose
    prompt
    passive
    put $HOME/$FILENAME $SERVER_DIR/$FILENAME
    bye
EOF
    
else
    rsync -e ssh --progress --partial $HOME/$FILENAME $SERVER_USER@$SERVER_HOST:$SERVER_DIR
fi

rm -f $HOME/$FILENAME
exit 0
