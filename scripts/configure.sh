#!/bin/bash
##---------------------------------------------------------------------------##
## CONFIGURE FACEMC with CMAKE
##---------------------------------------------------------------------------##

EXTRA_ARGS=$@
MOAB_PREFIX_PATH=
HDF5_PREFIX_PATH=
BOOST_PREFIX_PATH=
TPOR_PATH=
CURRENT_DIR=${PWD}

##---------------------------------------------------------------------------##

rm -rf CMakeCache.txt

##---------------------------------------------------------------------------##

cd $TPOR_PATH
git pull
cd $CURRENT_DIR

cmake \
    -D CMAKE_INSTALL_PREFIX:PATH= \
    -D CMAKE_BUILD_TYPE:STRING=Release \
    -D CMAKE_VERBOSE_CONFIGURE:BOOL=ON \
    -D CMAKE_VERBOSE_MAKEFILE:BOOL=ON \
    -D CMAKE_CXX_FLAGS:STRING="-D_GLIBCXX_USE_CXX11_ABI=0" \
    -D TPOR_ENABLE_DBC:BOOL=OFF \
    -D MOAB_PREFIX:PATH=$MOAB_PREFIX_PATH \
    -D HDF5_PREFIX:PATH=$HDF5_PREFIX_PATH \
    -D BOOST_PREFIX:PATH=$BOOST_PREFIX_PATH \
    $EXTRA_ARGS \
    $TPOR_PATH
