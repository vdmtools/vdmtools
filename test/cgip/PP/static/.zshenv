# echo ZSHENV

typeset -x -U PATH path
typeset -x -TU TEXINPUTS texinputs

typeset -x -TU CLASSPATH classpath
typeset -x -TU PYTHONPATH pythonpath

path+=( /usr/sbin /sbin /usr/local/sbin )

if [[ -d $HOME/.zfunc ]]; then
  typeset -U fpath
  fpath=( $HOME/.zfunc $fpath )
  autoload -U ~/.zfunc/*(:t)
fi

export JDK_HOME=/usr/java/jdk1.6.0
export JAVA_HOME=/usr/java/jdk1.6.0
export JAVA_BINDIR=${JAVA_HOME}/bin

path=( $JAVA_BINDIR $path)

path=( $HOME/perl $path /opt/gnome2/bin)

export ANT_HOME=~/apache-ant-1.6.1

path=( $ANT_HOME/bin
       $HOME/bin $path
       /usr/local/Acrobat5/bin
       /usr/X11R6/bin
     )

path=( 
    # $HOME/xml-fop 
    /usr/local/kde/bin $path )

export KDEDIRS=/usr/local/kde

export QTDIR=/usr/lib/qt-3.3

export SGML_CATALOG_FILES=/etc/sgml/catalog

texinputs=( . "" )

classpath=( 
    ~/toolbox/test/runtest2;
    ~/classes/Jimi/JimiProClasses.zip
)

export BZREMAIL="Peter Gorm Larsen <pgl@iha.dk>"

export CORBAHOME=/usr/local/omniORB
export LD_LIBRARY_PATH=$CORBAHOME/lib:$LD_LIBRARY_PATH

path=( /usr/local/bin $HOME/firefox-bin 

    $HOME/work/bzr.dev
    $HOME/eclipse 
    $path )

path=( .
       $path
       /home/pgl/vpp
       /home/pgl/vdmtools/vdmsl/bin
       /home/pgl/vdmtools/vdm++/bin
       /home/pgl/vdmtools/vice/bin )

# path+=( /opt/cross/bin /opt/cross/avr/bin )

# path=($^path(N))

#Set the ant PATH.
export ANTHOME=/usr/local/apache-ant-1.6.5
#export ANTHOME=/usr

#Set the qt PATH.
#	If qt package automatically sets the PATH 
#	then skip
#	else export QTDIR=/usr/lib/qt-3.3

#Set the OSTYPE.
export OSTYPE=`uname`

#Set the code generation command PATH.
export CG_SL_CMD=/usr/local/vdmsl/bin/vdmde
export CG_PP_CMD=/usr/local/vdmpp/bin/vppde

##export QTDIR=/usr/local/qt

path=($QTDIR:$path)

MANPATH=$QTDIR/doc/man:$MANPATH
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QTDIR PATH MANPATH LD_LIBRARY_PATH

# set-up the CLASSPATH
export CLASSPATH=/home/pgl/vpp/VDM.jar:/home/pgl/vpp/standalone.jar
