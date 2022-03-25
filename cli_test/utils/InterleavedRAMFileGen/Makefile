###############################################################################
#
# Copyright 2022 OpenHW Group
#
# Licensed under the Solderpad Hardware Licence, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://solderpad.org/licenses/
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH SHL-2.0
#
###############################################################################
#
# Makefile to generate CORE-V-MCU memory files from a previously generated binary.
#

ECLIPSE_WORKSPACE ?= $(HOME)/EclipseWorkspaces
CLI_TEST_TOPLEVEL ?= $(shell (git rev-parse --show-toplevel))
CLI_TEST_DIR      ?= $(CLI_TEST_TOPLEVEL)/cli_test
CLI_BIN_DIR       ?= $(CLI_TEST_TOPLEVEL)/cli_test/Default
THIS_DIR          := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all: clean cli.bin InterleavedRAMFile

InterleavedRAMFileGen: InterleavedRAMFileGen.c
	cc -o InterleavedRAMFileGen 

InterleavedRAMFile: InterleavedRAMFileGen
	./InterleavedRAMFileGen -i $(CLI_BIN_DIR)/cli.bin -o 0x800

clean:
	rm -rf memoryInitFiles
	rm -rf gf22dxMemoryInitFiles
	rm -f  InterleavedRAMFileGen
	rm -f  $(CLI_BIN_DIR)/*.bin
	rm -f  $(CLI_BIN_DIR)/cli_test
	rm -f  $(CLI_BIN_DIR)/cli_test.*

cli.bin:
	cd $(CLI_TEST_DIR) && \
	eclipse -nosplash \
		--launcher.suppressErrors \
		-application org.eclipse.cdt.managedbuilder.core.headlessbuild \
		-data $(ECLIPSE_WORKSPACE) \
		-import . \
		-cleanBuild \
		cli_test/Default && \
	cd $(THIS_DIR)

tp:
	@echo $(CLI_TEST_TOPLEVEL)
	@echo $(THIS_DIR)

