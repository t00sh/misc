#!/bin/sh

# Petit script pour remplacer les caractères accentués d'un fichier
# en leurs code HTML (il en manque)

if test -z $1
then
    echo "Usage : $0 <file>"
    exit 1
fi

sed -i '
s/À/\&Agrave;/g;
s/É/\&Ecute;/g;
s/È/\&Egrave;/g;
s/à/\&agrave;/g;
s/â/\&acirc;/g;
s/ä/\&auml;/g;
s/ç/\&ccedil;/g;
s/è/\&egrave;/g;
s/é/\&eacute;/g;
s/ê/\&ecirc;/g;
s/ë/\&euml;/g;
s/î/\&icirc;/g;
s/ï/\&iuml;/g;
s/ô/\&ocirc;/g;
s/ù/\&ugrave;/g;
s/ü/\&uuml;/g;
s/û/\&ucirc;/g' "$1"
