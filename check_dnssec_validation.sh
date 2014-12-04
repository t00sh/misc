#!/bin/sh

# Check the DNSSEC validation for a given DNS resolver
# Author: TOSH

set -e

check_program() {
    if [[ -z `which $1` ]]
    then
	echo "$1 not installed !"
	exit 1
    fi
}

get_dns_status() {
    if [[ -n $2 ]]
    then
	STATUS=`dig $1 @$2 +time=2 +tries=2 | grep "status\|timed out"`
	echo $STATUS | perl -ne 'print $1 if m/status: (\S+),/; print "TIMEOUT" if m/connection timed out/'
    else
	STATUS=`dig $1 +time=2 +tries=2 | grep "status\|time out"`
	echo $STATUS | perl -ne 'print $1 if m/status: (\S+),/; print "TIMEOUT" if m/connection timed out/'
    fi
}

check_program perl
check_program dig

if [[ -n $1 && ($1 = "-h" || $1 = "--help") ]]
then
    echo "Usages :"
    echo "- $0"
    echo "- $0 <resolver_ip>"
    exit 1
fi

DOMAIN_SIG_OK="sigok.verteiltesysteme.net"
DOMAIN_SIG_KO="sigfail.verteiltesysteme.net"

SERV_STATUS_OK=`get_dns_status $DOMAIN_SIG_OK $1`
SERV_STATUS_KO=`get_dns_status $DOMAIN_SIG_KO $1`

if [[ $SERV_STATUS_OK = "TIMEOUT" || $SERV_STATUS_KO = "TIMEOUT" ]]
then
    echo "[-] DNS query timed out"
    exit 1
fi

if [[ $SERV_STATUS_OK = "NOERROR" && $SERV_STATUS_KO = "SERVFAIL" ]]
then
    echo "[+] Your DNS resolver have dnssec validation"
else
    echo "[-] Your DNS resolver DO NOT HAVE dnssec validation !!!"
fi
	    
exit 0
