#!/usr/bin/python
import sys, getopt, struct


def help () :
    print ('Quickstart: srec2verilog.py -i <input.srec> -o <outputfile>')
    
def main (argv) :
    try :
        opts, args = getopt.getopt(argv, "i:tmr:l:c:d:",['infile','interleave'])
    except getopt.GetoptError :
        help()
        sys.exit(2)
    lsb = True
    left_bin_name = 'left.bin'
    right_bin_name = 'right.bin'
    left_txt_name = 'left.txt'
    right_txt_name = 'right.txt'
    textoutput = False
    interleave = False
    clk_header = 'PDM_CLK'
    data_header = 'DATA'
    for opt, arg in opts:
        if opt == '-h' : help(); sys.exit()
        elif opt == '-i' :
            infilename = arg
        elif opt == '-o' :
            outfilename = arg
    try:
        infile = open(infilename,'r')
    except:
        print ('Error: Input file not found')
        help()
        sys.exit(2)

    element = int(0);
    for line in infile :
        recordtype = line[:2]
        if recordtype == 'S3' :
            bytecount = int(line[2:4],16)
            address = line[4:12]
            dwords = []
            for i in range (int((bytecount - 5) / 4)) :
                dword = []
                for j in range (8) :
                    dword.append(line[(19-j)+(i)*8])
                dword[0], dword[6] = dword[6], dword[0]
                dword[1], dword[7] = dword[7], dword[1]
                dword[2], dword[4] = dword[4], dword[2]
                dword[3], dword[5] = dword[5], dword[3]
                print (element,": value <= 32'h","".join(dword),";",sep="")
                element = element + int(1);
    print ("default: value <= 0;") 
    infile.close()

if __name__ == "__main__" :
    main (sys.argv[1:])
          
def PrintException():
  exc_type, exc_obj, tb = sys.exc_info()
  f = tb.tb_frame
  lineno = tb.tb_lineno
  filename = f.f_code.co_filename
  linecache.checkcache(filename)
  line = linecache.getline(filename, lineno, f.f_globals)
  print ('Exception in ({}, Line {} "{}"): {}'.format(filename, lineno,line.strip(),exc_obj))
