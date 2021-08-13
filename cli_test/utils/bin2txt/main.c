#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int main(int argc, char *argv[])
{
    FILE *lBinFileReadPtr = (FILE *)NULL;
    FILE *lTxtFileWritePtr = (FILE *)NULL;
    long int lBinFileSize = 0;
    uint8_t lData = 0;

    if(argc < 1 )
    {
        printf("Usage: bin2txt <input.bin>\n");
    }
    else
    {
        printf("Converting %s to %s file.\n", argv[1], argv[2]);

        lBinFileReadPtr = fopen(argv[1],"rb");  // r for read, b for binary
        lTxtFileWritePtr = fopen ("../../../memoryInitFiles/cli_sim.txt","w");

        if(  ( lBinFileReadPtr ) && ( lTxtFileWritePtr ) )
        {
            fseek(lBinFileReadPtr, 0, SEEK_END); // seek to end of file
            lBinFileSize = ftell(lBinFileReadPtr); // get current file pointer
            fseek(lBinFileReadPtr, 0, SEEK_SET); // seek back to beginning of file
            printf("File [%s] is of %ld bytes\n",argv[1], lBinFileSize);

            while( lBinFileSize )
            {
                fread(&lData, sizeof(lData), 1, lBinFileReadPtr);
                fprintf(lTxtFileWritePtr, "%02x \n",lData);
                lBinFileSize--;
            }
            fclose(lBinFileReadPtr);
            fclose(lTxtFileWritePtr);

            printf("[DONE]\n");
        }
        else
        {
            printf("FILE access error\n");
        }
    }

    return 0;
}
