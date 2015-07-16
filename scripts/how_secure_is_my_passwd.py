#!/usr/bin/python2.7

# Date : 2015-07-16
# Writen by TOSH
# Script to see how secure is a password (just to give an idea)

import sys
import math

def have_maj(s):
    for c in s:
        if ord('A') <= ord(c) <= ord('Z'):
            return True
    return False

def have_min(s):
    for c in s:
        if ord('a') <= ord(c) <= ord('z'):
            return True
    return False

def have_num(s):
    for c in s:
        if ord('0') <= ord(c) <= ord('9'):
            return True
    return False

def have_sym(s):
    for c in s:
        if (0x21 <= ord(c) <= 0x2f) or \
           (0x3a <= ord(c) <= 0x40) or \
           (0x5b <= ord(c) <= 0x60) or \
           (0x7b <= ord(c) <= 0x7e):
            return True
    return False

def nearest_pow2(v):
    if v == 0:
        return 0

    return int(math.log(v, 2))

def security_comment(bits):
    if bits < 64:
        return "very Low"
    if bits < 96:
        return "low"
    if bits < 128:
        return "medium"
    if bits < 256:
        return "strong"
    if bits < 512:
        return "very strong"
    
    return "imbreakable"
        
def main(argv):
    security = 0
    
    if len(argv) != 2:
        print "How secure is my password ?"
        print "Usage : %s <password>" % argv[0] 
        return 1

    if have_maj(argv[1]):
        security += 26
    if have_min(argv[1]):
        security += 26
    if have_num(argv[1]):
        security += 10
    if have_sym(argv[1]):
        security += 31

    bits_sec = nearest_pow2(security**(len(argv[1])))
    
    print "++++ Security of \"%s\" (%d chars len) : " % (argv[1], len(argv[1]))
    print "++++ %d bits : %s" % (bits_sec, security_comment(bits_sec))
    print "\nNote : assumed that you chose random chars"
    
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))

