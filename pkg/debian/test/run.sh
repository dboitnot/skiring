#!/usr/bin/env bash

die () {
    echo "ERROR: $*"
    exit 1
}

set -o verbose
set -o xtrace

apt-get install -y /out/skiring_${NVERSION}-${RELEASE}_amd64.deb || die "failed to install package"

v="$(date)"

su skiring -c "printf '${v}\n${v}\n' |skiring put test" || die "failed to put test key"
skiring get test && die "unauthorized access to test key"
su skiring -c "skiring grant test $(whoami)" || die "failure granting access to test key"
c=$(skiring get test)
[ "$v" == "$c" ] || die "value mismatch: $c != $v"
su skiring -c "skiring revoke test root" || die "failure revoking access to test key"
skiring get test && die "unauthorized access to test key" || echo "revokation successful"
