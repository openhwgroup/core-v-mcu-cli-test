#!/bin/python3

print("-----------------------------------")
print("-")
print("- core-v-mcu2 test")
print("-")
print("-----------------------------------")

import argparse
import serial
import json
import time
import os

##############################
#
#  Argument handling
#
##############################
parser = argparse.ArgumentParser()
parser.add_argument("--console", metavar="/dev/ttyUSB?", help="port associated with the console", required=True);
parser.add_argument("--uart1", metavar="/dev/ttyUSB?", help="port associated with the uart1", required=True);
parser.add_argument("--test", metavar="?", help=".json file to be tested", required=True);
parser.add_argument("--jloc", metavar="?", help="regression or progression", required=True);
args = parser.parse_args()

##############################
#
#  Script processing
#
##############################
fail_count = 0
pass_count = 0
no_status_count = 0
Json = '/home/qlblue/NightlyBuild/arnold2/core-v-mcu-cli-test/autotest/'
with serial.Serial(args.console, 115200) as console:
    print("Opened console(" + args.console + ")")
    with serial.Serial(args.uart1, 115200) as uart1:
        print("Opened uart1(" + args.uart1 + ")")
        json_loc = Json + args.jloc
        os.chdir(json_loc)
        with open(args.test) as ftest:
            print("ftest = ", args.test)
            script = json.load(ftest)
            for test in script:
                print("*** " + test + " ***")
                commands = script[test]
                for command in commands:
                    if console.in_waiting > 0:
                        consoledata = console.read(console.in_waiting)
                        consolestr = consoledata.decode("utf-8")
                        print("console: " + consolestr)
                    for item in command:
                        if item == ">console":
                            # send command to console
                            command_bytes = command[">console"]
                            print(">>" + command_bytes)
                            for c in command[">console"]:
                                console.write(str(c).encode('utf-8'))
                                time.sleep(0.01)
                            # scan for '[?] >' which tells us console has completed
                            state = 0
                            state_done = 0
                            str_done = "<<DONE>>"
                            saw_done = False
                            state_failed = 0
                            str_failed = "<<FAILED>>"
                            saw_failed = False
                            state_passed = 0
                            str_passed = "<<PASSED>>"
                            saw_passed = False
                            while True:
                                console_data = console.read(1)
                                rcvd_char = console_data.decode('utf-8')
                                print(rcvd_char, end='')
                                # State M/C looking for prompt
                                if (state == 0 and console_data == b' '):
                                    state = 1
                                elif (state == 1 and console_data == b'>'):
                                    state = 2
                                elif (state == 2 and console_data == b' '):
                                    state = 3
                                    break
                                else:
                                    state = 0
                                
                                # state M/C looking for <<DONE>>
                                if state_done < len(str_done):
                                    if rcvd_char == str_done[state_done]:
                                        state_done = state_done + 1
                                    else:
                                        state_done = 0
                                if state_failed < len(str_failed):
                                    if rcvd_char == str_failed[state_failed]:
                                        state_failed = state_failed + 1
                                    else:
                                        state_failed = 0
                                if state_passed < len(str_passed):
                                    if rcvd_char == str_passed[state_passed]:
                                        state_passed = state_passed + 1
                                    else:
                                        state_passed = 0
                            print("\n")           
                            if state_done == len(str_done):
                                saw_done = True
                            if state_failed == len(str_failed):
                                saw_failed = True
                                fail_count = fail_count + 1
                            if state_passed == len(str_passed):
                                saw_passed = True
                                pass_count = pass_count + 1
                            if not(saw_done or saw_failed or saw_passed):
                                print("ERROR: Command did not return status")
                                no_status_count = no_status_count + 1
                        elif item == '<uart1':
                            time.sleep(0.01)
                            if uart1.in_waiting != len(command["<uart1"]):
                                print("<<FAILED>>: uart1 length does not match")
                                fail_count = fail_count + 1
                                print(uart1.in_waiting)
                                print(len(command["<uart1"]))
                                uart1_data = uart1.read(uart1.in_waiting)
                                uart1_str = uart1_data.decode("utf-8")
                                print("Got uart1: " + uart1_str)
                            else:
                                uart1_data = uart1.read(uart1.in_waiting)
                                uart1_str = uart1_data.decode("utf-8")
                                print("Got uart1: " + uart1_str)
                                if uart1_str != command["<uart1"]:
                                    print("<<FAILED>>: expected: " + command["<uart1"])
                                    fail_count = fail_count + 1
                                else:
                                    print("<<PASSED>>")
                                    pass_count = pass_count + 1
                            
                    
                            
                            
        uart1.close()
    console.close()

print("\n")
print("%4d commands returned no status" % no_status_count)
print("%4d commands PASSED" % pass_count)
print("%4d commands FAILED" % fail_count)
