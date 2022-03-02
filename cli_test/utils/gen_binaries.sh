#!/bin/bash

echo "Building binary files generator programs"
[ ! -d "../utils/bin2carray/bin/Debug" ] && mkdir -p ../utils/bin2carray/bin/Debug

cc ../utils/bin2carray/main.c -o ../utils/bin2carray/bin/Debug/bin2carray

echo "Done."

echo "Generating cli_test.bin for QSPI Flashing and cli_sim.bin for simulation testing"
cat  "../header.bin" "cli_test.bin"  >  "cli.bin"
cat  "../header_sim.bin" "cli_test.bin"  >  "cli_sim.bin"
echo "Done."

echo "Generating c array file for loading the app by an external host over I2C"
cd ../utils/bin2carray/bin/Debug
./bin2carray ../../../../Default/cli.bin
echo "Done. Generated c array file is in cli_test/utils/generated_c_array_file"

