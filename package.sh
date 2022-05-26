#!/usr/bin/env bash

usage () {
    echo "usage: $0 <platform> <version> <release>"
    exit 1
}

die () {
    echo "ERROR: $*"
    exit 1
}

platform=$1; shift
version=$1; shift
release=$1; shift

[ -z "$platform" ] && usage
[ -z "$version" ] && usage
[ -z "$release" ] && usage
[ -d "pkg/$platform" ] || die "Platform not found: $platform"

tag=skiring-pkg:$platform

OUT_DIR=$(pwd)/dist
mkdir -p $OUT_DIR || die "Error creating $OUT_DIR"

docker build -f pkg/$platform/Dockerfile -t $tag . || die "Error building image"
docker run -t --rm -e VERSION=$version -e RELEASE=$release \
    --mount type=bind,source="$OUT_DIR",target=/out $tag || die "Error building packages"
