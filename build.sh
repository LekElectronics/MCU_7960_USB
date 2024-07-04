#!/bin/sh
apt-get update
apt-get install -y build-essential wget zip make

if [ ! -f xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-x64.tar.gz ]; then
  rm -rf "xpack-arm-none-eabi-gcc*";
  # Download the latest xpack compiler
  wget https://github.com/xpack-dev-tools/arm-none-eabi-gcc-xpack/releases/download/v13.2.1-1.1/xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-x64.tar.gz
  # Extract it
  tar -xf xpack-arm-none-eabi-gcc-13.2.1-1.1-linux-x64.tar.gz
fi


echo "changing GCC path STM32Make.make to xpack-arm-none-eabi-gcc-13.2.1-1.1/bin"
# Change makefile to use this xpack compiler
sed -i '/^GCC_PATH=/c\GCC_PATH=\"xpack-arm-none-eabi-gcc-13.2.1-1.1/bin' STM32Make.make

# Read the current firmware version of source code
proj_name=$(grep -oP '(?<=#define PROJ_NAME ")[^"]*' Core/Src/Firmware_Version.c)
echo "proj_name = $proj_name"
fw_ver=$(grep -oP '(?<=#define PROJ_VERSION ")[^"]*' Core/Src/Firmware_Version.c)
echo "fw_ver = $fw_ver"

full_name="$proj_name"_"$fw_ver"
echo "Using full_name $full_name"
# Modify the project/output name to append the firmware version
#sed -i "/^TARGET = / s/$/_$fw_ver/" STM32Make.make
sed -i "s/^TARGET = .*/TARGET = $full_name/" STM32Make.make

# Build the project using the new compiler, and rename the output to contain the firmware version string. 
# This will make the artifact name teh same as teh firmware version 
make -j16 -f STM32Make.make

# output is in the build folder along with all obj/lst/d files. 
# zip the .hex file
build_dir=$(grep -oP '^BUILD_DIR = \K.*' STM32Make.make)
cd $build_dir
zip $full_name.zip *.hex
cd ..
mv $build_dir/$full_name.zip .
