import os
import sys
import subprocess

length = len(sys.argv)
args = sys.argv
JsonFolder = []

def autorun(length=length, Reg_Prog=sys.argv[1], console=sys.argv[2], uart=sys.argv[3], Type=sys.argv[4], args=args):
    arnold = os.path.expanduser("~/NightlyBuild/arnold2/")
    test = arnold + 'core-v-mcu-cli-test/autotest/'
    outputs = test + 'TestOutputs/'

    # determine if to run Regression or Progression folder
    if Reg_Prog == 'regression':
        JSON = test + 'regression/'
        outlog = outputs + 'regression/'

    if Reg_Prog == 'progression':
        JSON = test + 'progression/'
        outlog = outputs + 'progression/'
    # run all json files if argument = all
    if Type == 'all':
        JsonFolder = os.listdir(JSON)
        for json in JsonFolder:
            os.chdir(test) # location of Test.py
            name= json[0:-5]
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + json + ' >& ' + outlog + name + '.log --jloc ' + Reg_Prog 
            process = subprocess.call(command, shell=True)
    
    # run only the specified apps
    if Type == '-s':
        i = 5
        while i < length:
            os.chdir(test)
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + args[i] + '.json >& ' + outlog + args[i] + '.log --jloc ' + Reg_Prog
            process = subprocess.call(command, shell=True)
            i += 1

    # run all except specified logs
    if Type == '-e':
        JsonFolder = os.listdir(JSON)
        i = 5
        while i < length:
            jfile = args[i] + '.json'
            JsonFolder.remove(jfile)
            i += 1
        for json in JsonFolder:
            os.chdir(test) # location of Test.py
            command = 'python3 Test.py --console ' + console + ' --uart1 ' + uart + ' --test ' + json + ' >& ' + outlog + json[0:-5] + '.log --jloc ' + Reg_Prog
            process = subprocess.call(command, shell=True)


if __name__ == "__main__":
    autorun()
