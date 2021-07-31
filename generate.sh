#!/bin/bash

srcDir=.
libDir=lib
binDir=bin
common=../library

name=temp

function debugBuild {
	g++ -std=c++17 -Wall -g -fsanitize=address\
        -I ${common} -I ${libDir}\
        -o ${binDir}/${name}\
	    ${srcDir}/main-generator.cpp
}

function build {
	g++ -std=c++17 -O3 -flto -march=native\
        -I ${common} -I ${libDir}\
        -o ${binDir}/${name} \
		${srcDir}/main-generator.cpp
}

#* Compile the source files
build
# debugBuild

#* Run
./${binDir}/${name} >> log/generate.log
rm ${binDir}/${name}