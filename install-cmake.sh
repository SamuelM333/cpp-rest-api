#!/usr/bin/env bash

version=3.12
build=4

mkdir ~/temp
cd ~/temp
echo "Downloading cmake $version.$build"
wget -q https://cmake.org/files/v$version/cmake-$version.$build-Linux-x86_64.sh
mkdir /opt/cmake
sh cmake-$version.$build-Linux-x86_64.sh --prefix=/opt/cmake --skip-license
ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
