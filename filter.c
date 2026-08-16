#include <stdio.h>
#include <stdlib.h>


// Structure of the Header of the BMP file (also remove the padding byte)
typedef struct __attribute__((packed))
{
    unsigned char type[2];
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} FileHeader;


int main(int argc, char *argv[])
{
    // Check for proper Usage layout
    if (argc != 4){
        printf("Usage: ./filter [Filter] ./inputPath.bmp ./outputPath.bmp\n");
        printf("Filters: 'Blur', 'Grayscal', 'Sepia'\n");
        return 1;
    }

    char *filter = argv[1];
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

    // Store the input file header
    FileHeader header;
    fread(&header, sizeof(header), 1, inputFile);

    printf("Pixel data starts at %u\n", header.offset);
    printf("Size of FileHeader: %zu\n", sizeof(FileHeader));

    // Close Files
    fclose(inputFile);
    fclose(outputFile);
}