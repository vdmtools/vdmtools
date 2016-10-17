#!/bin/sh   

rm -f trace
vdmde  -p -R vdm.tc realm.vdm test.vdm 
for i in test/*.arg; do
	echo $i; echo
        time vdmde  -i -R vdm.tc -O "$i".res $i realm.vdm  test.vdm 
        echo $i >> trace
        diff "$i".res "$i".exp >> trace
done
vdmde -lr realm.vdm test.vdm
