#!/bin/bash

MODE="Debug"
SOURCE_LOCATION="/home/cord/Documents/Source_Code"

echo "Installing plugin for SAM..."
mkdir -p "$SOURCE_LOCATION"/Sequential_Archive/build-Sequential_Archive_Manager-Desktop-"$MODE"/Plugins/
cp -rf "$SOURCE_LOCATION"/Sequential_Archive/build-Sequential_Archive-Desktop-"$MODE"/libSequential_Archive.so "$SOURCE_LOCATION"/Sequential_Archive/build-Sequential_Archive_Manager-Desktop-"$MODE"/Plugins/Sequential_Archive.so

echo "Runtime Environment Prepared!"
exit 0

