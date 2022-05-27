#!/usr/bin/env bash

die () {
    echo "ERROR: $*"
    exit 1
}

[ -z "$VERSION" ] && die "VERSION not defined"
[ -z "$RELEASE" ] && die "RELEASE not defined"

set -o verbose
set -o xtrace

m4 -D VERSION=$VERSION -D RELEASE=$RELEASE /skiring.spec.m4 >/root/rpmbuild/SPECS/skiring-${VERSION}.spec || die "m4 failed"
rpmbuild -ba /root/rpmbuild/SPECS/skiring-${VERSION}.spec || die "rpmbuild failed"
cp -f /root/rpmbuild/SRPMS/*.src.rpm /root/rpmbuild/RPMS/*/*.rpm /out || die "failed to copy files to output dir"
