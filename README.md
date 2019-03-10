# Metal API

## Install Pistache

## Install MongoCxx
http://mongocxx.org/mongocxx-v3/installation/#installation
```
apt-get install libbson-1.0-0
git clone https://github.com/mongodb/mongo-cxx-driver.git \
    --branch releases/stable --depth 1
cd mongo-cxx-driver/build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
make && make install
```

## Install RapidJson
```
git clone https://github.com/Tencent/rapidjson.git
cd rapidjson
cmake .
make
sudo make install
```

## Build:
`cmake --build cmake-build-debug --target metal_api -- -j 6`
