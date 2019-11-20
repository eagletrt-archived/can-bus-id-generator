#!/bin/bash

# Here you can find the variables get it from Qt Creator

QMAKE="/opt/Qt/5.12.0/gcc_64/bin/qmake"

QT_PROJECT="can-bus-id-generator/can-bus-id-generator.pro"
BUILD_DIR=build/

if [ ! "$( ls -A $BUILD_DIR)" ]; then
    mkdir $BUILD_DIR
fi

cd $BUILD_DIR

$QMAKE $QT_PROJECT -o Makefile ../$QT_PROJECT
make || exit 1

./can-bus-id-generator