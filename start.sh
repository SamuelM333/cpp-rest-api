#!/bin/sh

# TODO wait-for-it mongo
# TODO Import MongoDB sample data

mkdir build
cd /usr/src/metal-api/build
cmake -G 'Unix Makefiles' ..
make

./metal_api
