#!/bin/sh

# TODO exit if failed installing 

# Install Pistache
# http://pistache.io/quickstart
echo "Installing Pistache..."
cd /opt
# TODO get tar for stable release
git clone https://github.com/oktal/pistache.git
cd pistache
# git submodule update --init
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
make install

# Install RapidJson
echo "Installing RapidJson..."
# TODO get tar for stable release
cd /opt

git clone https://github.com/Tencent/rapidjson.git
cd rapidjson
cmake .
make
make install

# Install Mongocxx
# http://mongocxx.org/mongocxx-v3/installation/#installation

echo "Installing Mongocxx..."
cd /opt

wget https://github.com/mongodb/mongo-c-driver/releases/download/1.14.0/mongo-c-driver-1.14.0.tar.gz
tar xzf mongo-c-driver-1.14.0.tar.gz
cd mongo-c-driver-1.14.0
mkdir cmake-build
cd cmake-build
cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF ..
make && make install

cd /opt
# git clone https://github.com/mongodb/mongo-cxx-driver.git --branch releases/stable --depth 1
# cd mongo-cxx-driver/build
curl -OL https://github.com/mongodb/mongo-cxx-driver/archive/r3.4.0.tar.gz
tar -xzf r3.4.0.tar.gz
cd mongo-cxx-driver-r3.4.0/build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make && make install


