#!/usr/bin/env bash

die () {
    echo "ERROR: $*"
    exit 1
}

[ -z "$VERSION" ] && die "VERSION not defined"
[ -z "$RELEASE" ] && die "RELEASE not defined"

ARCH=amd64  # TODO: This should be an input

set -o verbose
set -o xtrace

DEB=/root/deb
BASE=/root/skiring_${NVERSION}-${RELEASE}_${ARCH}

mkdir $BASE $BASE/DEBIAN || die "failed to create packaging directories"

m4 -D VERSION=$NVERSION -D RELEASE=$RELEASE -D ARCH=$ARCH $DEB/control.m4 >$BASE/DEBIAN/control || die "m4 failed"

for S in postinst preinst postrm prerm; do
    if [ -f "${DEB}/${S}.sh" ]; then
        cp $DEB/${S}.sh $BASE/DEBIAN/$S || die "script copy error"
        chmod 755 $BASE/DEBIAN/$S || die "chmod failed"
    fi
done

cd /root || die "couldn't cd to /root"

tar xvzf /skiring-${VERSION}.tar.gz || die "untar failed"
cd /root/skiring-${VERSION} || die "failed to cd to build directory"
autoreconf || die "autoreconf failed"
./configure --prefix=$BASE/usr/local --sysconfdir=/etc --disable-install-hook || die "configure failed"
make || die "make failed"
make install || die "make install failed"

find $BASE

dpkg-deb --build --root-owner-group -v -D $BASE || die "dpkg build failed"

cp /root/skiring_${NVERSION}-${RELEASE}_${ARCH}.deb /out || die "failed to copy packages to /out"
