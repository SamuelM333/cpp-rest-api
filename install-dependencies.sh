#!/usr/bin/env bash

# Install RapidJSON
# https://github.com/Tencent/rapidjson#installation
echo "\e[92mInstalling RapidJSON...\e[0m"
cd /opt

git clone https://github.com/Tencent/rapidjson.git
cd rapidjson
cmake .
make -j 5 && make install

# Install Pistache
# http://pistache.io/quickstart
echo "\e[92mInstalling Pistache...\e[0m"

cd /opt

# TODO get tar for stable release
git clone https://github.com/oktal/pistache.git
cd pistache
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make -j 5 && make install

# Install mongo-c and mongo-cxx
# http://mongocxx.org/mongocxx-v3/installation/#installation

echo "\e[92mInstalling mongo-c...\e[0m"
mongo_c_version=1.14.0
cd /opt

wget -q https://github.com/mongodb/mongo-c-driver/releases/download/$mongo_c_version/mongo-c-driver-$mongo_c_version.tar.gz
tar -xzf mongo-c-driver-$mongo_c_version.tar.gz
cd mongo-c-driver-$mongo_c_version
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DCMAKE_BUILD_TYPE=Release ..
#cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
make -j 5 && make install

echo "\e[92mInstalling mongo-cxx...\e[0m"
mongo_cxx_version=3.4.0
cd /opt

wget -q https://github.com/mongodb/mongo-cxx-driver/archive/r$mongo_cxx_version.tar.gz
tar -xzf r$mongo_cxx_version.tar.gz
cd mongo-cxx-driver-r$mongo_cxx_version/build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j 5 && make install

rm /etc/ld.so.conf.d/x86_64-linux-gnu.conf
/sbin/ldconfig -v > /dev/null