#!/usr/bin/env bash
# Downloads and installs cmake. The default prefix used for the installation is
# ${HOME}/local, so there is no need for root privileges. The user can specify
# a different prefix path in the first argument. Note that the script does not
# check the integrity of the download contents.

prefix_dir=${HOME}/local

# Terminate on error
set -e

if [[ $# -eq 1 ]]; then
  prefix_dir=$1
fi
mkdir -p ${prefix_dir}
minor_ver=3.11
full_ver=${minor_ver}.0
fname=cmake-${full_ver}
downloadurl=https://cmake.org/files/v${minor_ver}/${fname}.tar.gz
wget -qO- ${downloadurl} | tar xzv
pushd ${fname}
./bootstrap --prefix=${prefix_dir}
make
make install
popd
rm -fr ${fname}

echo "Make sure the following is in your path: ${prefix_dir}/bin"
