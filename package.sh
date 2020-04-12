#!/bin/sh
# Created by: WestleyR
# Email: westleyr@nym.hush.com
# Url: https://github.com/WestleyR/list-files
# Last modified date: 2020-04-11
#
# This file is licensed under the terms of
#
# The Clear BSD License
#
# Copyright (c) 2019-2020 WestleyR
# All rights reserved.
#
# This software is licensed under a Clear BSD License.
#

#
# This script will make a gpack compatible tarball package.
#
# Usage:
#  $ ./package.sh
#

#
# The tarball package should be like this:
# list-files/
# └── 1.6.0 (the package version)
#     └── bin/
#         └── lf (the binary)
#

set -e


echo "  1. x86_64_linux"
echo "  2. macos"
echo "  3. armv6l"
echo "  4. armv7l"
echo -n "Select your arch: "
read user_in

case $user_in in
  1)
    package_arch="x86_64_linux"
    ;;
  2)
    package_arch="macos"
    ;;
  3)
    package_arch="armv6l"
    ;;
  4)
    package_arch="armv6l"
    ;;
  *)
    echo "Invalid option: $user_in"
    exit 1
esac

echo "Package arch: $package_arch"

make cleanall
make

package_version=`./lf -V | cut -dv -f2 | rev | cut -d, -f3 | rev`

mkdir -p ./pkg/list-files/${package_version}/bin
cp -f ./lf ./pkg/list-files/${package_version}/bin

tar -czf ./pkg/lf-v${package_version}-${package_arch}.tar.gz ./pkg/list-files

echo ""
echo "Tarball v${package_version} is ready: ./pkg/lf-v${package_version}-${package_arch}.tar.gz"
echo ""
echo "Checksums:"

cd ./pkg

if type sha256sum > /dev/null; then
  echo -n "sha256: "
  sha256sum lf-v${package_version}-${package_arch}.tar.gz
fi
if type ssum > /dev/null ; then
  echo -n "ssum20: "
  ssum lf-v${package_version}-${package_arch}.tar.gz
fi
if type md5sum > /dev/null ; then
  echo -n "md5sum: "
  md5sum lf-v${package_version}-${package_arch}.tar.gz
fi

# vim: tabstop=2 shiftwidth=2 expandtab autoindent softtabstop=0
