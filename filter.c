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

typedef struct __attribute__((packed))
{
    unsigned int size;
    unsigned int width;
    int height;
    unsigned short planes;
    unsigned short bitsPixel;
} InfoHeader;




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

    // Store the input file FileHeader
    FileHeader header;
    fread(&header, sizeof(header), 1, inputFile);

    // Store the input file InfoHeader
    InfoHeader headerInfo;
    fread(&headerInfo, sizeof(headerInfo), 1, inputFile);


    // Copy header and headerInfo to the output file
    fwrite(&header, sizeof(header), 1, outputFile);
    fwrite(&headerInfo, sizeof(headerInfo), 1, outputFile);

    // Account for row padding in case row size (width * bytes per pixel) is not divisible by 4
    int bytesPerPixel = headerInfo.bitsPixel / 8;
    int rowPadding = (4 - ((headerInfo.width * bytesPerPixel) % 4)) % 4;
    

    int height = abs(headerInfo.height);
    int width = headerInfo.width;


    // Loop through each pixel
    unsigned char pixel[3];
    unsigned char padding[3];
    for (int i = 0; i < height; i++) // EACH ROW
    {
        for (int j = 0; j < width; j++) // EACH PIXEL
        {
            // Read BGR value of each pixel
            fread(pixel, 1, 3, inputFile);  
            
            // Modify pixel

            fwrite(pixel, 1, 3, outputFile);
        }
        // Padding bytes at the end of each row
        fread(padding, 1, rowPadding, inputFile);
        fwrite(padding, 1, rowPadding, outputFile);
    }




    printf("Width: %u Height: %i Bits per Pixel: %hu\n", headerInfo.width, headerInfo.height, headerInfo.bitsPixel);

    // Close Files
    fclose(inputFile);
    fclose(outputFile);
}