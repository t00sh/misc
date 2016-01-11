#!/usr/bin/perl

use strict;
use warnings;

my @CERTS = (
    "/etc/openvpn/niobe/ca.crt",
    "/etc/openvpn/niobe/seraphin.crt"
    );

my $EXTRACT_TLS_DATE_SCRIPT = "/home/tosh/bin/extract_tlsdate.sh";
my $CURL_PATH = "/usr/bin/curl";
my $DAYS_WARNING = 45;
my $FREE_USER = "user";
my $FREE_PASS = "pass";
my $FREE_URL = "https://smsapi.free-mobile.fr/sendmsg?user=".$FREE_USER."&pass=".$FREE_PASS."&msg=";


for my $cert (@CERTS) {
    my @output = `$EXTRACT_TLS_DATE_SCRIPT $cert`;
    my $days = 0;

    for my $line(@output) {
        if($line =~ m/Days: (\d+)/) {
            $days = $1;
            last;
        }
    }

    print "$cert -> $days\n";

    if($days <= $DAYS_WARNING) {
        my $url = $FREE_URL . substr($cert, rindex($cert,"/")+1) . " expire dans $days jours !";
        $url =~ s/\s+/%20/g;
        `$CURL_PATH '$url'`;
    }
}
