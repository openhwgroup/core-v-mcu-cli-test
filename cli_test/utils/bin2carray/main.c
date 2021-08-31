#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv[])
{
    FILE *lBinFileReadPtr = (FILE *)NULL;
    FILE *lCArrayFileWritePtr = (FILE *)NULL;
    long int lBinFileSize = 0;
    uint8_t lData = 0;

    if(argc < 1 )
    {
        printf("Usage: bin2carray <input.bin>\n");
    }
    else
    {
        printf("Converting %s to c array file.\n", argv[1]);

        lBinFileReadPtr = fopen(argv[1],"rb");  // r for read, b for binary
        lCArrayFileWritePtr = fopen ("A2_App.c","w");

        if(  ( lBinFileReadPtr ) && ( lCArrayFileWritePtr ) )
        {
            fseek(lBinFileReadPtr, 0, SEEK_END); // seek to end of file
            lBinFileSize = ftell(lBinFileReadPtr); // get current file pointer
            fseek(lBinFileReadPtr, 0, SEEK_SET); // seek back to beginning of file
            printf("File [%s] is of %ld bytes\n",argv[1], lBinFileSize);

            fprintf(lCArrayFileWritePtr, "const unsigned char gArnold2AppFWBuf[%ld] = {\n",lBinFileSize);
            while( lBinFileSize )
            {
                fread(&lData, sizeof(lData), 1, lBinFileReadPtr);
                fprintf(lCArrayFileWritePtr, "0x%02x",lData);
                lBinFileSize--;
                if( lBinFileSize >= 1 )
                    fprintf(lCArrayFileWritePtr, ",");
                if( ( lBinFileSize %16 ) == 0 )
                    fprintf(lCArrayFileWritePtr, "\n");
            }
            fprintf(lCArrayFileWritePtr, "};\n");
            fclose(lBinFileReadPtr);
            fclose(lCArrayFileWritePtr);

            printf("[DONE]\n");
        }
        else
        {
            printf("FILE access error\n");
        }
    }

    return 0;
}
