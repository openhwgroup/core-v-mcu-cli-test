#!/bin/bash
if [ $# -eq 0 ]; then
    echo "Usage ./A2_build.sh new_workspace"
    exit 1
fi
eclipse -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data ~/$1 -import . -cleanBuild A2_boot/Release
python srec2verilog.py -i Release/A2_boot.srec -o fpga_bootrom.sv
