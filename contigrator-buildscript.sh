#!/bin/bash
# This file is executed by contigratord when a build is triggered.
# You can execute it manually too by executing /home/contigrator/contigrator/tool/trigBuild.sh

#These variables are available:
# $WD = The workspace $BN = Zero padded build Number (%07i) and $BN_NUM for unpadded.
# $OUT = This empty directory is created for you to store the output of the build.

OUT=/tmp/sdlballout/
mkdir /tmp/sdlballout

make -f Makefile.win clean
make -f Makefile clean


OUTDIR="SDL-Ball_1.04"

WINPKG="$OUTDIR"_windows.zip
LINPKG="$OUTDIR"_linux.tar.bz2
SRCPKG="$OUTDIR"_src.tar.xz

mkdir -p $OUTDIR

# Create the changelog
git log > changelog.txt
cp changelog.txt $OUTDIR

# Compile for linux
make -f Makefile
mv sdl-ball $OUTDIR/sdl-ball_linux_x64
make -f Makefile clean

# Compile for windows
make -f Makefile.win

# Package datafiles
cp -a leveleditor $OUTDIR
cp -a themes $OUTDIR
cp *.txt $OUTDIR

# Package linux version

tar -c $OUTDIR | pbzip2 > $LINPKG
rm $OUTDIR/sdl-ball_linux_x64

# Package windows version
mv sdl-ball.exe $OUTDIR/SDL-Ball.exe
cp win32/readme_SDL.txt $OUTDIR
unix2dos $OUTDIR/*.txt
cp win32/*.dll $OUTDIR
zip -r $WINPKG $OUTDIR
make -f Makefile.win clean

# Build the source code
git archive --prefix="SDL-Ball_src/" HEAD > "$OUTDIR"_src.tar
tar --file "$OUTDIR"_src.tar --append --transform 's%^%SDL-Ball_src/%' changelog.txt
cat "$OUTDIR"_src.tar | pxz > "$OUT/$SRCPKG"

rm -R $OUTDIR *.tar
rm changelog.txt

mv $WINPKG $OUT
mv $LINPKG $OUT

mv $OUT .
