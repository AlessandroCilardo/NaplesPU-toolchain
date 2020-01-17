#!/bin/bash

#
# Build the compiler
#

#PARAMETERS:
#   -l,--lib       : builds the NaplesPU libraries.
#   -t=,--thread=   : sets the number of threads for compilation.
#   -d,--debug      : sets the building type to debug.
#   -r,--release    : sets the building type to release.
#   -n,--new        : make a new compiler build.
#   -k,--linker    : builds only the linker.

TOP_DIR=`pwd`
COMPILER_PATH=$TOP_DIR/compiler

#default values
BUILDPATHREL=$COMPILER_PATH/build
BUILDPATHDEB=$COMPILER_PATH/buildDebug


DEBUG=0
NEW=0
THREAD=4
LINKER=0

#input variables
LIBRARIES=
NEWDEBUG=
NEWTHREAD=
BUILDPATH=$BUILDPATHREL

set -e

for i in "$@"
do
case $i in
    -l|--lib)
        if [ -z "$LIBRARIES" ]; then
            LIBRARIES=1
            #echo "LIBRARIES  =   ${LIBRARIES}"
        fi
        shift
    ;;
    -t=*|--thread=*)
        if [ -z "$NEWTHREAD" ]; then
            NEWTHREAD="${i#*=}"
            #echo "THREAD  =   ${NEWTHREAD}"
        fi
        shift
    ;;
    -d|--debug)
        if [ -z "$NEWDEBUG" ]; then
            NEWDEBUG=1
            BUILDPATH=$BUILDPATHDEB
            #echo "DEBUG  =   ${NEWDEBUG}"
        fi
        shift
    ;;
    -n|--new)
        NEW=1
        #echo "NEW  =   ${NEW}"
        shift
    ;;
    -k|--linker)
        LINKER=1
        #echo "LINKER  =   ${LINKER}"
        shift
    ;;
    -h|--help)
        echo "This script helps you to build the NaplesPU compiler and libraries."
        echo "PARAMETERS:"
        echo "-l,--lib       : builds the NaplesPU libraries."
        echo "-t=,--thread=   : sets the number of threads for compilation."
        echo "-d,--debug      : sets the building type to debug."
        echo "-n,--new        : make a new compiler build."
        echo "-k,--linker    : builds only the linker."
        exit
    ;;
    *)
            # unknown option
    ;;
esac
done


if [ ! -z "$NEWDEBUG" ]; then
    DEBUG=$NEWDEBUG
fi

if [ ! -z "$NEWTHREAD" ]; then
    THREAD=$NEWTHREAD
fi

    if [ "$LINKER" = 0 ]; then
        if [ "$NEW" = "1" ]; then
            tput setaf 4
            echo -e "Creating build directory..."
            tput sgr0

            rm -rf "${BUILDPATH}"
            mkdir -p "${BUILDPATH}"
            cd "${BUILDPATH}"

            if [ "$DEBUG" = "1" ]; then
                tput setaf 1
                echo -e "Configuring build in Debug mode..."
                tput sgr0
                echo "The Toolchain will be installed in /usr/local/llvm-npu/"

                cmake -G Ninja -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/llvm-npu/ -DCMAKE_BUILD_TYPE=Debug "${COMPILER_PATH}"
            else
                tput setaf 4
                echo -e "Configuring build in Release mode..."
                tput sgr0
                echo "The Toolchain will be installed in /usr/local/llvm-npu/"

                cmake -G Ninja -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/llvm-npu/ -DCMAKE_BUILD_TYPE=Release "${COMPILER_PATH}"
            fi
        else
            echo "cd {$BUILDPATH}"

            cd "${BUILDPATH}"
        fi
        tput setaf 2
        echo -e "Building LLVM with Ninja..."
        tput sgr0
        ninja
    else
        echo "cd ${BUILDPATH}"
        echo "make lld"

        cd "${BUILDPATH}"
        make lld
    fi
    tput setaf 2
    echo -e "Installing LLVM..."
    tput sgr0

    sudo ninja install
    tput setaf 4
echo -e "Done."

cd $TOP_DIR

if [ "$LIBRARIES" = 1 ]; then
    echo "Building and Installing NaplesPU libraries..."
    cd libs && mkdir -p build && cd build && cmake .. &&  make clean && make && sudo make install && cd ../..
    echo "-- Installing: /usr/local/llvm-npu/misc-npu"
    sudo cp -rf misc/* /usr/local/llvm-npu/misc-npu
fi
    tput sgr0
