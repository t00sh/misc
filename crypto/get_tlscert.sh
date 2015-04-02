#!/bin/sh

# Get a x509 certificate from a remote server

if [[ -z `which openssl` ]]
then
    echo "openssl not installed !"
    exit 1
fi

if [[ -z `which perl` ]]
then
    echo "perl not installed !"
    exit 1
fi

if [[ $# -lt 2 ]]
then
    echo "Usage : $0 <host> <port>"
    exit 1
fi

REMOTE_HOST=$1
REMOTE_PORT=$2

echo |
openssl s_client -connect $REMOTE_HOST:$REMOTE_PORT 2>&1 |
perl -ne 'undef $/; print $1 if (m/(-+BEGIN CERTIFICATE.+END CERTIFICATE-+\n)/s);'

exit 0
