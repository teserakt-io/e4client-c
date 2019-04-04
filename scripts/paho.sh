#!/bin/bash

rm -rf paho.mqtt.c
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
mkdir build
cd build
cmake -DPAHO_BUILD_STATIC=TRUE ..
make
cd ../..
mkdir -p lib
cp -vf paho.mqtt.c/build/src/libpaho-mqtt3c-static.a lib/libpaho-mqtt3c.a

