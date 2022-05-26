#!/usr/bin/env bash

die () {
	>&2 echo "ERROR: $*"
	exit 1
}

ref="$1"
[ -z "$ref" ] && die "version not specified"
>&2 echo "ref: $ref"

ver="$(echo "$ref" |grep -Eo '[0-9]+\.[0-9]+(\.[0-9]+)?$' || echo SNAPSHOT)"
>&2 echo "ver: $ver"

>&2 echo "Updating configure.ac"
sed -ie "s/^AC_INIT(.*/AC_INIT([skiring], [${ver}])/" configure.ac || die "failed to update configure.ac"

>&2 echo "Generating configure and Makefile.in"
autoconf || die "autoconf failed"
automake || die "automake failed"

echo "$ver"
