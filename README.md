# core-v-mcu-cli-test
Eclipse IDE based CORE-V MCU test routines running on FreeRTOS

### Installation
The Eclipse IDE relies on a larger ecosystem of tools and these instructions assume you are familar enough with the Eclispe IDE environment to make any required configuration changes to the IDE setup.
### Step 1:
Downlad and install the approriate corev-gcc toolchain from
~~~
https://www.embecosm.com/resources/tool-chain-downloads/#corev
~~~

### Step 2: 
Download and install the Eclipse IoT Embedded CDT IDE (this will eventually be replaced by a pre-configured CORE-V IDE based on the Eclipse IDE tools).  
~~~
https://projects.eclipse.org/projects/iot.embed-cdt/downloads
~~~

### Step 3:
Install OpenOCD.  Recommend getting it from this location:
~~~
https://sourceforge.net/projects/openocd/files/openocd/
~~~
Version 0.11.0-rc2 is know to work with this setup.

Installation is accomplished by:
~~~
./configure --enable-ftdi && make
sudo make install
~~~ 


### Step 4: run the Eclipse IDE, import projects directly from this OpenHW git repo

NOTE: importing the cli_test project into the IDE does not import the information to launch the debugger, so after importing cli_test, be sure to either import the launch configuration (below in Step 5) or create your own.

1 Launch Eclipse
~~~
eclipse
~~~
![step1](./images/1-IDE-Launcher.png)

2 From Welcome, select Import
![step2](./images/2-Welcome.png)

3 Select Import Projects from Git

![step3](./images/3-Import.png)

4 Specify Clone URL

![step4](./images/4-ImportFromGit.png)

5 Tell Git which repo to use (core-v-mcu-eclipse-cli-test)

![step5](./images/5-RepoName.png)

6 Select the branch to clone

![step6](./images/6-BranchName.png)

7 Specify directory for git to use (use the default)

![step7](./images/7-LocalDestination.png)

8 Select which Wizard to use for import

![step8](./images/8-SelectWizard.png)

9 Tell it which projects to import (cli_test is the only one you need)

![step9](./images/9-ImportProjects.png)

10 What you should see after slected 'Finish'

![step10](./images/10-ProjectsAreSetup.png)

# Step 5: Import launch configuration
This step pulls in the launch configurations which are configured to use hs2 and OpenOCD.
If your hardware setup is different, you can either ignore this step and create your own, or use this step and modify to fit your configuration.

1 Select 'Import...'

![step11](./images/11-ImportLaunch.png)

2 Select Launch Configurations

![step12](./images/12-SelectLaunchConfig.png)

13 Browse to the git directory that was chosen as the Local Destination

![step13](./images/13-ImportLaunchConfigurations.png)

14 Select launchconfig and cli_test Default

![step14](./images/14-SelectLaunchConfigurations.png)

15 Under the 'Run' menu select 'Debug Configurations...'

![step15](./images/15-DebugConfig.png)

15 Under 'GDB OpenOCD Debugging'  select 'cli_test Default' and then 'Debug'

![step16](./images/16-cli_testDefault.png)

This should compile, link and load the application and stop at main waiting for you to start debugging.


## Emulation hardware
The intent is to support more than one emulation platform.
Currently only one board is supported:

* Nexys A7 100T

![Nexys Setup](./images/nexys-setup.png)

## Emulation bitsream
Bistreams compatible with the Nexys A7 100T can be found at:
http://downloads.openhwgroup.org/

## Peripheral support
The type and number of peripherals supported are defined by 'pulp_soc_defines.sv'.
The information is made available to software applications as a series of #define values in the 'SOC options' section of 'target/core-v-mcu/include/core-v-mcu-config.h'.
An example configuration is:
~~~
// SOC options
#define N_IO                 48         // Total available IO
#define N_SYSIO              3
#define N_GPIO               32
#define N_UART               2          // Number of uarts
#define N_QSPIM              1          // Number of QuadSPI masters
#define N_I2CM               2          // Number of I2C masters
#define N_I2SC               0
#define N_CSI2               0
#define N_HYPER              0
#define N_SDIO               0
#define N_CAM                1
#define N_JTAG               0
#define N_MRAM               0
#define N_FILTER             1
#define N_FPGA               1
#define N_EXT_PER            0
#define N_EFPGA_TCDM_PORTS   4
#define N_FPGAIO             43
#define N_EFPGA_EVENTS       16
~~~
