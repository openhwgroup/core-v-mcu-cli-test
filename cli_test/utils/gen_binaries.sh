#!/bin/bash

echo "Generating cli_test.bin for QSPI Flashing and cli_sim.bin for simulation testing"
cat  "../header.bin" "cli_test.bin"  >  "cli.bin"
cat  "../header_sim.bin" "cli_test.bin"  >  "cli_sim.bin"
echo "Done."

echo "Generating interleaved RAM initalization files for simulation testing"
cd ../utils/InterleavedRAMFileGen/bin/Debug
./InterleavedRAMFileGen ../../../../Default/cli_test.bin
echo "Done."

echo "Generating memory initialization file for simulated flash model"
cd ../../../bin2txt/bin/Debug
./bin2txt ../../../../Default/cli_sim.bin cli_sim.txt
echo "Done."
