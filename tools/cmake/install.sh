#!/usr/bin/env bash
# Downloads and installs cmake. The default prefix used for the installation is
# ${HOME}/local, so there is no need for root privileges. The user can specify
# a different prefix path in the first argument. Note that the script does not
# check the integrity of the download contents.

prefix_dir=${HOME}/local

build_jobs=4
host=$(hostname)
if [[ ${host} =~ "latedays.andrew" ]]; then
    build_jobs=24
elif [[ ${host} =~ "ghc.andrew" ]]; then
    build_jobs=16
fi

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
wget -qO- --no-check-certificate ${downloadurl} | tar xzv
pushd ${fname}
./bootstrap --prefix=${prefix_dir} --parallel=${build_jobs}
make -j ${build_jobs}
make install
popd
rm -fr ${fname}

echo "Make sure the following is in your path: ${prefix_dir}/bin"
