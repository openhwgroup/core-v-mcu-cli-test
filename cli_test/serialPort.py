import serial
import sys
import os

argLen = len(sys.argv)
print("Total arguments passed:", argLen)

if( argLen < 3 ):
    print ("Usage:", sys.argv[0], "[/dev/ttyUSBx] [./Default/sample.srec]")
else:
    if not os.path.exists(sys.argv[2]):
        print("File does not exist")
    else:
        serialPort = serial.Serial(port = sys.argv[1], baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

        serialString = ""                           # Used to hold data coming over UART
        print("Opening " + sys.argv[2])
        srecfile = open(sys.argv[2], 'r')
        
        #line = srecfile.readline()
        #print(line.encode(), end = "")
        #line = srecfile.readline()
        #print(line, end = "")

        while(1):

            # Wait until there is data waiting in the serial buffer
            if(serialPort.in_waiting > 0):

                # Read data out of the buffer until a carraige return / new line is found
                serialString = serialPort.readline()

                # Print the contents of the serial data
                print(serialString.decode('Ascii'), end = "")
                if( (serialString.decode('Ascii') == "A2 BOOTME\r\n") or (serialString.decode('Ascii') == "OK\r\n" ) ):
                    line = srecfile.readline()
                    serialPort.write(line.encode())
                    print(line, end = "")
                    if not line:
                        print("File read complete")
                        srecfile.close()
                # Tell the device connected over the serial port that we recevied the data!
                # The b at the beginning is used to indicate bytes!
                #serialPort.write(b"Thank you for sending data \r\n")

