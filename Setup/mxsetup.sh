#!/bin/bash

echo "Hello World!"
echo App: "$0"
echo Cmd: "$1"

InstFile="mxinst"

if [ "$1" == "install" ]; then
	date >> $InstFile;
fi

if [ "$1" == "uninstall" ]; then
	rm $InstFile;
fi
