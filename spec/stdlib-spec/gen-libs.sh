#! /bin/bash
# Usage: $0
# Create VDM specification files from source file.

function writevdm {
# $1: File name
# $2: Module name

# Remove VDM++ comments:
SLFILE=`perl -p -e 's/^-- VDM\+\+:.*\n//g' $1.src`

# Create VDMSL flat file:
cat <<EOF>$1flat.vdm
$SLFILE
EOF

# Create VDMSL module file:
cat <<EOF>$1.vdm
module $2
exports all
definitions
$SLFILE
end $2
EOF
}

function writevpp {
# $1: File name
# $2: Class name

# Extract VDM++ comments:
PPFILE=`perl -p -e 's/^-- VDM\+\+: //g' $1.src`

# Create VDM++ file:
cat <<EOF>$1.vpp
class $2
$PPFILE
end $2
EOF
}

function writeall {
# $1: File name
# $2: Class/module name
  writevdm $1 $2
  writevpp $1 $2
}

######################## MAIN #############################

NAME=MATH
name=math
writeall $name $NAME

NAME=IO
name=io
writeall $name $NAME

NAME=VDMUtil
name=VDMUtil
writeall $name $NAME

NAME=VDMByteUtil
name=VDMByteUtil
writeall $name $NAME
