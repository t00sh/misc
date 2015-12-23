#!/usr/bin/perl

use strict;
use warnings;

my $TRASH_DIR = "$ENV{HOME}/.Trash";

if(!@ARGV) {
    die "Usage : $0 <file1>, [<file2>, ...]\n";
}

unless(-d $TRASH_DIR) {
    mkdir $TRASH_DIR, 0700;
}

foreach my $path(@ARGV) {
    my $filename = get_file_name($path);
    my $trashed = "$TRASH_DIR/$filename";

    unless(-e $path) {
        die "$path doesn't exists\n";
    }

    if(-e $trashed) {
        my $i = 1;

        while(-e "$trashed($i)") {
            $i++;
        }
        $trashed = "$trashed($i)";
    }

    rename($path, $trashed);
}

sub get_file_name {
    my $path = shift;
    my $index;

    if(($index = rindex($path, '/')) == -1) {
        return $path;
    }

    return substr($path, $index+1);
}
