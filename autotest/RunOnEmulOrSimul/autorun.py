import os
import sys
import subprocess

length = len(sys.argv)
args = sys.argv
JsonFolder = []

def autorun(length=length, Reg_Prog=sys.argv[1], console=sys.argv[2], uart=sys.argv[3], InterCharacterDelay=sys.argv[4], Type=sys.argv[5], args=args):
    arnold = os.path.expanduser("../")
    jsontest = arnold
    test = arnold + 'RunOnEmulOrSimul/'
    outputs = './TestOutputs/'

    # determine if to run Regression or Progression folder
    if Reg_Prog == 'regression':
        JSON = jsontest + 'regression/'
        outlog = outputs + 'regression/'

    if Reg_Prog == 'progression':
        JSON = jsontest + 'progression/'
        outlog = outputs + 'progression/'
    if not os.path.exists(outlog):
        os.makedirs(outlog)
    # run all json files if argument = all
    if Type == 'all':
        JsonFolder = os.listdir(JSON)
        for json in JsonFolder:
            os.chdir(test) # location of Test.py
            name= json[0:-5]
            print(name)
            command = 'python3 Test.py --console ' + console + ' --cdelay ' + InterCharacterDelay + ' --uart1 ' + uart + ' --test ' + json + ' >& ' + outlog + name + '.log --jloc ' + Reg_Prog 
            process = subprocess.call(command, shell=True)
    
    # run only the specified apps
    if Type == '-s':
        i = 6
        while i < length:
            os.chdir(test)
            command = 'python3 Test.py --console ' + console + ' --cdelay ' + InterCharacterDelay +  ' --uart1 ' + uart + ' --test ' + args[i] + '.json >& ' + outlog + args[i] + '.log --jloc ' + Reg_Prog
            process = subprocess.call(command, shell=True)
            i += 1

    # run all except specified logs
    if Type == '-e':
        JsonFolder = os.listdir(JSON)
        i = 6
        while i < length:
            jfile = args[i] + '.json'
            JsonFolder.remove(jfile)
            i += 1
        for json in JsonFolder:
            os.chdir(test) # location of Test.py
            command = 'python3 Test.py --console ' + console + ' --cdelay ' + InterCharacterDelay +  ' --uart1 ' + uart + ' --test ' + json + ' >& ' + outlog + json[0:-5] + '.log --jloc ' + Reg_Prog
            process = subprocess.call(command, shell=True)


if __name__ == "__main__":
    autorun()
