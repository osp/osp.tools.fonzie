#!/bin/sh

# univers.sh
# 
#
# Created by Pierre on 23/10/2010.
# Copyright 2010 __MyCompanyName__. All rights reserved.

SCALE=6.2
OFFSET=190

STYLE=$1
STYLE2=$2


rm -rf /Users/pierre/Documents/Univers_.ufo/
cp -a /Users/pierre/Documents/Untitled1.ufo /Users/pierre/Documents/Univers_.ufo
./src/fonzie \
strict=1\
r=Univers/Univers-${STYLE}${STYLE2}.otf \
i=scans/univers_${STYLE}${STYLE2}_U0000_1.jpg \
wc=/Users/pierre/Documents/Univers_.ufo/ \
cc=32 \
o=-$OFFSET \
s=$SCALE \
cp=0 \
spec=0+0+0+300+150+192 \
i1=scans/univers_${STYLE}${STYLE2}_U0080.jpg \
wc1=/Users/pierre/Documents/Univers_.ufo/ \
cc1=160 \
o1=-$OFFSET \
s1=$SCALE \
cp1=0 \
spec1=0+0+0+300+150+192 \
i2=scans/univers_${STYLE}${STYLE2}_U2000.jpg \
wc2=/Users/pierre/Documents/Univers_.ufo/ \
cc2=8208 \
o2=-$OFFSET \
s2=$SCALE \
cp2=0 \
spec2=0+300+0+150+150+192 \
#echo "Open(\"/Users/pierre/Documents/Univers_.ufo\"); familyname=\"Universale\"; fullname=\"Universale${STYLE}\"; fontname=\"Universale ${STYLE}\"; weight=\"${STYLE}\";Generate(\"Universale${STYLE}.otf\")" > script.ff
echo "import fontforge" > script.py
echo "font = fontforge.open(\"/Users/pierre/Documents/Univers_.ufo\")"  >> script.py
echo "font.familyname = \"UniversaleStrict\"" >> script.py
echo "font.fontname = \"UniversaleStrict-${STYLE}${STYLE2}\"" >> script.py
echo "font.fullname = \"UniversaleStrict ${STYLE} ${STYLE2}\"" >> script.py
echo "font.weight = \"${STYLE} ${STYLE2}\"" >> script.py
#echo "for g in font.glyphs():" >> script.py
#echo "	if(g.isWorthOutputting()):" >> script.py
#echo "		font.selection.select((\"unicode\", \"more\"), g.unicode)" >> script.py
#echo "font.autoWidth(10)"  >> script.py
#echo "for n in font.sfnt_names:" >> script.py
#echo "	print n" >> script.py
echo "font.appendSFNTName('English (US)', 'SubFamily' , font.weight)" >> script.py
echo "font.generate(\"UniversaleStrict${STYLE}${STYLE2}.otf\")" >> script.py

/Applications/FontForge.app/Contents/MacOS/FontForge  -lang=py -script script.py  