#!/bin/sh
for dodo in `find . -name "*body" -print | sed -e "s/.body$//g"`; do
export dede=`echo $dodo | sed -e "s/[0-9\-]*$//g"`
echo Handling $dodo
rm -f tmpspec $dodo.vdm
if [ -e $dede.head ]; then cat $dede.head > tmpspec; fi
if [ -e $dodo.body ]; then cat $dodo.body >> tmpspec; fi
if [ -e $dede.tail ]; then cat $dede.tail >> tmpspec; fi
if [ -s tmpspec ]; then mv -f tmpspec $dodo.vdm; fi
if [ -e $dodo.dynval ]; then cp -f $dodo.dynval $dodo.res; fi
done
find . -name "*vdm" -print | wc -l
