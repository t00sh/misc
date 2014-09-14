#!/bin/sh

# Get the expiration date of a x509 certificate
# Usage : $0 <certificate> or $0 < certificate
# Need : openssl + perl

set -e

if [[ -z `which openssl` ]]
then
    echo "openssl not installed"
    exit 1
fi

if [[ -z `which perl` ]]
then
    echo "perl not installed"
    exit 1
fi

if [[ -z $1 ]]
then
    DATES=`openssl x509 -dates | grep "notBefore\|notAfter"`
else
    DATES=`openssl x509 -dates -in $1 | grep "notBefore\|notAfter"`
fi

DATE_START=`echo $DATES | perl -ne 'print $1 if(m/notBefore=(.+? GMT)/)'`
DATE_END=`echo $DATES | perl -ne 'print $1 if(m/notAfter=(.+? GMT)/)'`

DAYS=$((`date -d "$DATE_END" +"%s"` - `date -d "$DATE_START" +"%s"`))
DAYS=$(($DAYS / (60*60*24)))

echo "Start: $DATE_START"
echo "End: $DATE_END"
if [[ $DAYS -le 0 ]]
then
    echo "CERTIFICAT EXPIRED"
else
    echo "Days: $DAYS"
fi
