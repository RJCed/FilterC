#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Check for proper Usage layout
    if (argc != 4){
        printf("Usage: ./filter [Filter] ./inputPath.bmp ./outputPath.bmp\n");
        printf("Filters: 'Blur', 'Grayscal', 'Sepia'\n");
        return 1;
    }

    char *inputPath = argv[2];
    char *outputPath = argv[3];

    // Open the input file
    FILE *inputFile = fopen(inputPath, "rb");
    if (inputFile == NULL)
    {
        printf("Input File can't be open.\n");
        return 2;
    }

    // Create the output file
    FILE *outputFile = fopen(outputPath, "wb");
    if (inputFile == NULL)
    {
        printf("Output File can't be created.\n");
        return 2;
    }


    // Close Files
    fclose(inputFile);
    fclose(outputFile);
    printf("%s, %s %s", argv[1], inputPath, outputPath);
}