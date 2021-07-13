import os
import sys
import subprocess

length = len(sys.argv)
args = sys.argv
JsonFolder = []

def autorun(length=length, console=sys.argv[1], uart=sys.argv[2], Type=sys.argv[3], args=args):
    test = '.'
    JSON = './json/'

    # run all json files if argument = all
    # need to make sure cam is not present in folder
    if Type == 'all':
        JsonFolder = os.listdir(JSON)
        for json in JsonFolder:
            os.chdir(test) # location of Test.py
            name= json[0:-5]
	
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + json + ' >& ~/NightlyBuild/arnold2/core-v-mcu-cli-test/autotest/TestOutputs/' + name + '.log'
            process = subprocess.call(command, shell=True)
            #print(command)
    
    # run only the specified apps
    if Type == '-s':
        i = 4
        while i < length:
            os.chdir(test)
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + args[i] + '.json >& ~/NightlyBuild/arnold2/core-v-mcu-cli-test/autotest/TestOutputs/' + args[i] + '.log'
            process = subprocess.call(command, shell=True)
            #print(command)
            i += 1

    # run all except specified logs
    if Type == '-e':
        JsonFolder = os.listdir(JSON)

        i = 4
        while i < length:
            jfile = args[i] + '.json'
            JsonFolder.remove(jfile)
            i += 1
        for json in JsonFolder:
            print("Testing " + json[0:-5] )
            os.chdir(test) # location of Test.py
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + json + ' >& ~/NightlyBuild/arnold2/core-v-mcu-cli-test/autotest/TestOutputs/' + json[0:-5] + '.log'
            process = subprocess.call(command, shell=True)
            #print(command)


if __name__ == "__main__":
    autorun()
