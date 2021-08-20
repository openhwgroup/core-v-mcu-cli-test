#!/bin/bash
console_uart_found_flag=0
uart1_uart_found_flag=0
OUTPUTDIR=~/NightlyBuild/arnold2/core-v-mcu-cli-test/autotest/TestOutputs

while getopts ":c:d:u:U:" opt; do
    case $opt in
        c) CONSOLE_UART_PORT="/dev/tty${OPTARG}"
           console_uart_found_flag=1
           ;;
        d) OUTPUTDIR="$OPTARG"
           ;;
        u) UART1_UART_PORT="/dev/tty${OPTARG}"
           uart1_uart_found_flag=1
           ;;
        U) user="${OPTARG}"
           ;;
        \?) echo "Invalid Option -$OPTARG" >&2
            ;;
    esac
done

if [ "$user" == "greg" ]
then
    if [ $console_uart_found_flag == 0 ]
    then
       CONSOLE_UART_PORT=$(python3 -m serial.tools.list_ports 0403:6010 -q -n 1)
    fi
    if [ $uart1_uart_found_flag == 0 ]
    then       
       UART1_UART_PORT=$(python3 -m serial.tools.list_ports 067B:2303 -q -n 1)
    fi
       OUTPUTDIR=TestOutputs
elif [ "$user" == "simul" ]     #need to set the uart ports correctly to run on QuestaSim
then
    if [ $console_uart_found_flag == 0 ]
    then
       CONSOLE_UART_PORT=$(python3 -m serial.tools.list_ports 0403:6010 -q -n 1)
    fi
    if [ $uart1_uart_found_flag == 0 ]
    then       
       UART1_UART_PORT=$(python3 -m serial.tools.list_ports 067B:2303 -q -n 1)
    fi
       OUTPUTDIR=TestOutputs
else
    if [ $console_uart_found_flag == 0 ]
    then
       CONSOLE_UART_PORT=$(python3 -m serial.tools.list_ports 0403:6010 -q -n 2) #Second instance of Nexsys USB2UART
    fi
    if [ $uart1_uart_found_flag == 0 ]
    then       
       UART1_UART_PORT=$(python3 -m serial.tools.list_ports 0403:6001 -q -n 1) #First instance of PMOD USB2UART
    fi
fi


if [ "$CONSOLE_UART_PORT" == "" ]
then
    echo "Nexsys A7 not found"
else
    echo "Nexsys A7 found at $CONSOLE_UART_PORT"
    console_uart_found_flag=1
fi

if [ "$UART1_UART_PORT" == "" ]
then
    echo "UART1 not found"
else
    echo "UART1 found at $UART1_UART_PORT"
    uart1_uart_found_flag=1
fi

if [ $console_uart_found_flag == 1 ] && [ $uart1_uart_found_flag == 1 ] 
then
    echo "Both UART ports found"
    # run all tests
    if [ "$user" == "simul" ]
    then
        python3 autorun.py regression $CONSOLE_UART_PORT $UART1_UART_PORT 1 all      #1000ms intercharacter delay (1000/1000)
        python3 autorun.py progression $CONSOLE_UART_PORT $UART1_UART_PORT 1 -e cam
    else
        python3 autorun.py regression $CONSOLE_UART_PORT $UART1_UART_PORT 0.01 all      #10ms intercharacter delay (10/1000)
        python3 autorun.py progression $CONSOLE_UART_PORT $UART1_UART_PORT 0.01 -e cam
    fi
	echo "DONE. Results logged at autotest/RunOnEmulOrSimul/TestOutputs/"
else
    echo "Cannot start tests as console_uart_found_flag = $console_uart_found_flag and uart1_uart_found_flag = $uart1_uart_found_flag"
fi
