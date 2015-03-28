#!/bin/sh

set -e

if [ $# -lt 2 ]; then
  echo >&2 "Usage: $0 SOURCE DEST"
  exit 1
fi

if [ -d "$1" ]; then :; else
  echo >&2 "$1: No such directory"
  exit 1
fi
srcdir=`cd "$1" && pwd`

if [ -d "$2" ]; then :; else
  mkdir "$2"
fi
destdir=`cd "$2" && pwd`

basedir=`dirname "$0"`
basedir=`cd "$basedir" && pwd`

echo "Building host"

cd "$basedir"/../cmake
rm -rf build
[ -d build ] || mkdir build
cd build
cmake \
-DBUILD_LINUX=TRUE \
-DSDL_HOST=TRUE \
-DMOAI_BOX2D=TRUE \
-DMOAI_CHIPMUNK=TRUE \
-DMOAI_CURL=TRUE \
-DMOAI_CRYPTO=TRUE \
-DMOAI_EXPAT=TRUE \
-DMOAI_FREETYPE=TRUE \
-DMOAI_JSON=TRUE \
-DMOAI_JPG=TRUE \
-DMOAI_MONGOOSE=TRUE \
-DMOAI_LUAEXT=TRUE \
-DMOAI_OGG=TRUE \
-DMOAI_OPENSSL=TRUE \
-DMOAI_SQLITE3=TRUE \
-DMOAI_TINYXML=TRUE \
-DMOAI_PNG=TRUE \
-DMOAI_SFMT=TRUE \
-DMOAI_VORBIS=TRUE \
-DMOAI_UNTZ=TRUE \
-DMOAI_LUAJIT=FALSE \
-DMOAI_HTTP_CLIENT=TRUE \
-DCMAKE_BUILD_TYPE=Release \
-DPLUGIN_DIR="`pwd`"'/../../src/plugins' \
-DPLUGIN_DFHACK=true \
-DMAIN_SCRIPT='Data/Scripts/main.lua' \
-DDEBUG=ON \
-DCMAKE_C_FLAGS='-g' -DCMAKE_CXX_FLAGS='-g' \
../

make -j2

echo "Running installation script"
SRCDIR="$srcdir" DESTDIR="$destdir" JAVACMD="java" \
host-sdl/moai "$basedir"/hack-installer/main.lua

echo "Installing host as $destdir/Hack"
cp host-sdl/moai "$destdir"/Hack

echo "Done"
exit 0
