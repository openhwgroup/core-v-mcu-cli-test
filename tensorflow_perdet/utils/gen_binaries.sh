#!/bin/bash

echo "Generating tensorflow_perdet_flash.bin for QSPI Flashing and tensorflow_perdet_sim.bin for simulation testing"
cat  "../header.bin" "tensorflow_perdet.bin"  >  "tensorflow_perdet_flash.bin"
cat  "../header_sim.bin" "tensorflow_perdet.bin"  >  "tensorflow_perdet_sim.bin"
echo "Done."

echo "Generating c array file for loading the app by an external host over I2C"
cd ../utils/bin2carray/bin/Debug
./bin2carray ../../../../Default/tensorflow_perdet_flash.bin
echo "Done. Generated c array file is in tensorflow_perdet/utils/generated_c_array_file"

echo "Generating interleaved RAM initalization files for simulation testing"
cd ../../../InterleavedRAMFileGen/bin/Debug
./InterleavedRAMFileGen ../../../../Default/tensorflow_perdet.bin
echo "Done. Generated files are in tensorflow_perdet/utils/memoryInitFiles"

echo "Generating memory initialization file for simulated flash model"
cd ../../../bin2txt/bin/Debug
./bin2txt ../../../../Default/tensorflow_perdet_sim.bin
./bin2txt ../../../../Default/tensorflow_perdet_flash.bin
echo "Done. Generated file is in tensorflow_perdet/utils/memoryInitFiles"
