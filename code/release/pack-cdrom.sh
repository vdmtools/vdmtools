#! /bin/bash

# Prepare CDROM distribution. Copy bins and docs from ftp site to
# <packdir>. Unpack all documentation. Copy README.cdrom from
# ~vdm/doc/cdrom
# Usage: pack-cdrom.sh <packdir>
#
# $1: Target directory to pack in

if [ $# -ne 1 ] ; then
  echo "Usage: $0 <packdir>"
  exit 1
fi
# Start setup section. Changes here should be reflected in README.cdrom
ROOT=$1
FTP=/home/ftp/pub
SL=vdm-sl
PP=vdm++
BIN=executables
DOC=doc
# End setup section

# Copy binaries from ftp site:

mkdir -p $ROOT/$SL/$BIN $ROOT/$PP/$BIN $ROOT/$SL/$DOC $ROOT/$PP/$DOC
cp  -pv $FTP/toolbox/bin.* $FTP/toolbox/setup* $FTP/toolbox/README $ROOT/$SL/$BIN
cp -pv $FTP/toolbox/README $ROOT/$SL/$BIN/README.txt
cp  -pv $FTP/vdm++_toolbox/bin.* $FTP/vdm++_toolbox/setup* $ROOT/$PP/$BIN
cp -pv $FTP/vdm++_toolbox/README  $ROOT/$PP/$BIN/README.txt

# Unpack pdf files:
(cd $ROOT/$SL/$DOC &&
 unzip $FTP/toolbox/doc_letter.pdf.zip &&
 unzip $FTP/toolbox/known_bugs.zip &&
 unzip doc_html_bitmaps.zip &&
 rm -f doc_html_bitmaps.zip)
(cd $ROOT/$PP/$DOC &&
 unzip $FTP/vdm++_toolbox/doc_letter.pdf.zip &&
 unzip $FTP/vdm++_toolbox/known_bugs.zip &&
 unzip doc_html_bitmaps.zip &&
 rm -f doc_html_bitmaps.zip)

# Copy README.cdrom:

cp -pv /home/vdm/doc/cdrom/README.cdrom $ROOT/README.txt
