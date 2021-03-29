# core-v-mcu-cli-test
Eclipse/FreeRTOS/core-v-mcu example program

## Installation
core-v-mcu-cli-test depends on core-v-mcu-sdk, thus the preferred installation sequence is to install core-v-mcu-sdk first, and then
core-v-mcu-cli-test.

Core-v-mcu-sdk generates a library file that is used by the cli-test app.

## Steps to install

Start Eclipse Embedded IDE and target a new workspace.
The Eclipse start page will list a number of options:

1	Select 'Checkout projects from Git'
2 On the 'Select Repository Source' wondow, select 'Clone URL'
3 On the 'Source Git Repository' window paste in the repo name 'https://github.com/QuickLogic-Corp/core-v-mcu-sdk.git'
4 On the 'Branch Selection' window select the 'main' branch
5 On the 'Local Destination' window accept the default path '/...../git/core-v-mcu-sdk'
6 On the 'Select a wizard to use for importing projects' window select 'Import existing Eclipse projects'
7 On the 'Import projects' window accept the default by clicking finish

Next you have to install the core-v-mcu-cli-test project

1 Select 'File>Import...'
2 Select 'Git>Projects from Git'
3 On the 'Select Repository Source' wondow, select 'Clone URL'
4 On the 'Source Git Repository' window paste in the repo name 'https://github.com/QuickLogic-Corp/core-v-cli-test.git'
5 On the 'Branch Selection' window select the 'main' branch
6 On the 'Local Destination' window accept the default path '/...../git/core-v-mcu-sdk'
7 On the 'Select a wizard to use for importing projects' window select 'Import existing Eclipse projects'
8 On the 'Import projects' window accept the default by clicking finish

You should now see the cli-test project and the sdk project in the explorer window.
The final step is to import the launch configurations"

1 Select 'File>Import...'
2 Select 'Run/Debug>Launch Configuration'
3 On the 'From Directory' window, browser to '/...../git/core-v-mcu-cli-test/launch' and open
4 Select and import all

## Steps to use
First, buiild the SDK project
Second, build the cli-test project
Finally, run the cli-test project

