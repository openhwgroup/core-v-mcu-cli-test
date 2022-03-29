# core-v-mcu-cli-test
Command-Line-Interface test routines for the CORE-V MCU.

Core-v-mcu-cli-test is a stand-alone program that runs on the CORE-V MCU under FreeRTOS and is controlled from a terminal.
At the time of this writing (2022-03-09) "cli-test" runs in simulation under Verilator or in FPGA emulation on a Digilent Nexys A7-100T evaluation kit.
Setup instructions for the Nexys A7 and a terminal emulator to run cli-test can be found in the [CORE-V-MCU Quick Start Guide](https://github.com/MikeOpenHWGroup/core-v-mcu/blob/qsg/emulation/quickstart/README.md).
A bitstream compatible with the Nexys A7-100T that supports cli-test, and instructions for loading the bitstream onto the Nexys can also be found with the Quick Start Guide.

The Quick Start Guide will get you up and running with cli-test, which is a reasonable testing platform, but not a convenient development platform.
It is expected that the majority of cli-test users will want to run it from an Integrated Development Environment.
The remainder of this document will get you set up with the **Eclipse-IDE**.

## Eclipse IDE Installation
The Eclipse IDE relies on a larger ecosystem of tools and these instructions assume you are familar enough with the Eclispe IDE environment to make any required configuration changes to the IDE setup.

### Step 1:
Download and install the **Eclipse IoT Embedded CDT IDE** (this will eventually be replaced by a pre-configured CORE-V IDE based on the Eclipse IDE tools).  
~~~
https://projects.eclipse.org/projects/iot.embed-cdt/downloads
~~~

### Step 2:
Download and install the approriate corev-gcc toolchain from
~~~
https://www.embecosm.com/resources/tool-chain-downloads/#corev
~~~

### Step 3:
Install OpenOCD.  Recommend getting it from this location:
~~~
https://sourceforge.net/projects/openocd/files/openocd/
~~~
Version 0.11.0-rc2 is known to work with this setup.

Installation is accomplished by:
~~~
./configure --enable-ftdi && make
sudo make install
~~~

### Step 4: run the Eclipse IDE and import projects directly from this OpenHW git repo

NOTE: importing the cli_test project into the IDE does not import the information to launch the debugger, so after importing cli_test, be sure to either import the launch configuration (below in Step 5) or create your own.

#### Step 4.1: Launch Eclipse
~~~
eclipse
~~~
![step1](./images/1-IDE-Launcher.png)

#### Step 4.2: From Welcome, select Import
![step2](./images/2-Welcome.png)

#### Step 4.3: Select Import Projects from Git

![step3](./images/3-Import.png)

#### Step 4.4: Specify Clone URL

![step4](./images/4-ImportFromGit.png)

#### Step 4.5: Specify the Source Git Repository
Provide the full URL path to `core-v-mcu-cli-test`.
The path you provide will depend on the protocol used to access GitHub.
For example, [https://github.com/openhwgroup/core-v-mcu-cli-test.git](https://github.com/openhwgroup/core-v-mcu-cli-test.git) is the path when using https.

![step5](./images/5-RepoName.png)

#### Step 4.6: Select the branch to clone
Note: the list of branches will change over time.
The `main` branch is stable, but not necessarily the latest.
All other branches are development branches and may not be stable.

![step6](./images/6-BranchName.png)

#### Step 4.7: Specify directory for git to use (use the default)

![step7](./images/7-LocalDestination.png)

#### Step 4.8: Select which Wizard to use for import

![step8](./images/8-SelectWizard.png)

#### Step 4.9: Select projects to import
`cli_test` is the only one you need.

![step9](./images/9-ImportProjects.png)

#### Step 4.10: Select 'Finish'
You should see something like this after selecting 'Finish'.

![step10](./images/10-ProjectsAreSetup.png)

### Step 5: Configure the Toolchain
This step configures Eclipse so that it finds the CORE-V toolchain and OpenOCD that were downloaded and installed in **Step 2** and **Step 3**.

#### Step 5.1: Select Properties
In the Eclipse Project Explorer select the `cli_test` Project, right click and select `Properties` (typically appears at the bottom of the pop-up menu).
Select `C/C++ Build / Settings` to bring up the window below:

![Step 5.1](./images/5p1-Properties_for_cli_test.png)

#### Step 5.2: Configure Toolchain
In this example, the `Toolchain path` was changed by selecting the `workspace` preference page.
This brings up the Preferences selection window.
On the left-hand pane select  "MCU/Workspace RISC-V Toolchains Paths" and update the Toolchain folder to point to your toolchain install path.

![Step 5.2](./images/5p2-WorkspaceRISC-VToolchainsPaths.png)

#### Step 5.3: Configure OpenOCD
This is similar to setting the Toolchain tool.
In the Preferneces selectionn window select "MCU/Workspace OpenOCD Path" and update to point to your openocd install path.

![Step 5.3](./images/5p3-WorkspaceOpenOCDPaths.png)

### Step 6: Compile cli_test
Assuming everything above has completed this step is simplicity itself.
With `cli_test` selected in the Project Explorer pane, click on Build (the hammer icon):

![Step6.0](./images/BuildCliTest.png)

### Step 7: Import launch configuration

This step pulls in the launch configurations which are configured to use **OpenOCD** and the [JTAG-HS2](https://digilent.com/shop/jtag-hs2-programming-cable/) programmer.
Reminder: the instructions for connecting the HS2 to the Nexys A7 can be found in the CORE-V-MCU Quick Start Guide (a link can be found at the top of this README).

If your hardware setup is different, you can either ignore this step and create your own, or use this step and modify to fit your configuration.

#### Step 7.1:
Select 'File' and then 'Import...'.

![step11](./images/11-ImportLaunch.png)

#### Step 7.2:
Select 'Run/Debug' and then 'Launch Configurations'.

![step12](./images/12-SelectLaunchConfig.png)

#### Step 7.3:
Here you will fetch the HS2 launch configuration from your clone of core-v-mcu-cli-test repository.
In the `core-v-mcu-cli-test` repo there is a set of launch configurations in the `launch` directory.
Use the `Browse...` button to navigate to the git directory that was chosen as the 'Local Destination' in **Step 4.7** and then `launch`.

Select 'launch' and 'cli_test hs2.lauch'.

![step14](./images/14-SelectLaunchConfigurations.png)

#### Step 7.5:
Under the `Run` menu select `Debug Configurations...`

![step15](./images/15-DebugConfig.png)

#### Step 7.6: Run/Restart settings
Edit the debug configurations and in the Startup tab scroll down to the Run/Restart Commands section to make sure the `Pre-run/Restart Reset` box is _not_ checked.

![step16](./images/RunRestart.png)

#### Step 7.7:
Under `GDB OpenOCD Debugging` select `cli_test Default` and then `Debug`.

![step17](./images/16-cli_testDefault.png)

This should compile, link and load the application and stop at main waiting for you to start debugging.


## Peripheral support
The type and number of peripherals supported are defined by `pulp_soc_defines.sv`.
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
