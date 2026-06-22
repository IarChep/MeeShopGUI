#!/usr/bin/env bash
# Source this file in your shell before running the simulator build:
#   source simulator_env.sh

QTSIM_ROOT="/home/iarchep/QtSDK/Simulator/Qt/gcc"

export QTDIR="$QTSIM_ROOT"
export QMAKE="$QTSIM_ROOT/bin/qmake"
export PATH="$QTSIM_ROOT/bin:$PATH"
export QML_IMPORT_PATH="$QTSIM_ROOT/imports"
export QT_PLUGIN_PATH="$QTSIM_ROOT/plugins"
export LD_LIBRARY_PATH="$QTSIM_ROOT/lib:$(pwd)/build-sim/_deps/libpng12-build:${LD_LIBRARY_PATH:-}"

echo "Qt Simulator env loaded: QMAKE=$QMAKE"
