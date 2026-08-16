#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Structure of the Header of the BMP file
typedef struct __attribute__((packed))
{
    unsigned char type[2];
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offset;
} FileHeader;

// Info Header for BMP files
typedef struct __attribute__((packed))
{
    unsigned int size;
    unsigned int width;
    int height;
    unsigned short planes;
    unsigned short bitsPixel;
} InfoHeader;

// Structure for each pixel
typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} Pixel;


// FILTERS
void grayscale(int height, int width, Pixel **image);


int main(int argc, char *argv[])
{
    // Check for proper Usage layout
    if (argc != 4){
        printf("Usage: ./filter [Filter] ./inputPath.bmp ./outputPath.bmp\n");
        printf("Filters: 'Blur', 'Grayscale', 'Sepia'\n");
        return 1;
    }

    char *filter = argv[1];
    // Lowercase each character of filter
    for (int i = 0; filter[i] != '\0'; i++)
    {
        filter[i] = tolower(filter[i]);
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
    
    // Allocate memory for the row pointers
    Pixel **image = malloc(height * sizeof(Pixel *));
    if (image == NULL)
    {
        return 1;
    }

    // Allocate memory for each pixels
    for (int i = 0; i < height; i++)
    {
        image[i] = malloc(width * sizeof(Pixel));
        if (image[i] == NULL)
        {
            return 1;
        }
    }



    // Copy each pixel to the image array
    unsigned char pixel[3];
    unsigned char padding[3] = {0};
    for (int i = 0; i < height; i++) // EACH ROW INPUT
    {
        for (int j = 0; j < width; j++) // EACH PIXEL INPUT
        {
            // Read BGR value of each pixel
            fread(pixel, 1, 3, inputFile);  

            image[i][j].blue = pixel[0];
            image[i][j].green = pixel[1];
            image[i][j].red = pixel[2];

        }
        // Padding bytes at the end of each row
        fseek(inputFile, rowPadding, SEEK_CUR);
    }



    // MODIFY EACH PIXEL
    if (strcmp(filter, "grayscale") == 0)
    {
        grayscale(height, width, image);
    }




    // Copy each pixel in the image array to the output
    for (int i = 0; i < height; i++) // EACH ROW OUTPUT
    {
        for (int j = 0; j < width; j++) // EACH PIXEL OUTPUT
        {
            // Read BGR value of each pixel
            fwrite(&image[i][j], sizeof(Pixel), 1, outputFile);

        }
        // Padding bytes at the end of each row
        fwrite(padding, 1, rowPadding, outputFile);
    }



    printf("Width: %u Height: %i Bits per Pixel: %hu\n", headerInfo.width, headerInfo.height, headerInfo.bitsPixel);

    // Close Files
    fclose(inputFile);
    fclose(outputFile);

    // Free allocated memory for pixels
    for (int i = 0; i < height; i++)
    {
        free(image[i]);
    }
    free(image);
}






// Convert image to grayscale
void grayscale(int height, int width, Pixel **image)
{
    printf("Grayscale Filter!\n");

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int average = round(
                (image[i][j].red +
                image[i][j].green +
                image[i][j].blue) / 3.0
            );

            image[i][j].red = average;
            image[i][j].green = average;
            image[i][j].blue = average;
        }
    }
}