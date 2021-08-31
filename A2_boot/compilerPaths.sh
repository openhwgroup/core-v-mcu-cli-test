#!/bin/sh
export PULP_RISCV_GCC_TOOLCHAIN=/opt/corev-openhw-gcc-ubuntu2004-20200913
export PATH=$PULP_RISCV_GCC_TOOLCHAIN/bin:$PATH
export PATH=/opt/eclipse-embedcpp-2021-03-R-linux-gtk-x86_64/eclipse:$PATH
export PATH=~/bin:$PATH
source /tools/Xilinx/Vivado/2020.2/settings64.sh
