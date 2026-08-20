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
    unsigned int compression;
    unsigned int imageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int colorsImportant;
} InfoHeader;

// Structure for each pixel
typedef struct
{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} Pixel;

int over_255(int value);
void freeImage(int height, Pixel **image);

// FILTERS
void grayscale(int height, int width, Pixel **image);
void sepia(int height, int width, Pixel **image);
void blur(int height, int width, Pixel **image);
void fliph(int height, int width, Pixel **image);
void flipv(int height, int width, Pixel **image);

int main(int argc, char *argv[])
{
    // Check for proper Usage layout
    if (argc != 4){
        printf("Usage: ./filter [Filter] ./inputPath.bmp ./outputPath.bmp\n");
        printf("Filters: 'Blur', 'Grayscale', 'Sepia', 'FlipH', 'FlipV'\n");
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
    if (outputFile == NULL)
    {
        printf("Output File can't be created.\n");
        fclose(inputFile);
        return 2;
    }


    // Store the input file FileHeader
    FileHeader header;
    fread(&header, sizeof(header), 1, inputFile);

    // Store the input file InfoHeader
    InfoHeader headerInfo;
    fread(&headerInfo, sizeof(headerInfo), 1, inputFile);


    // CHECK FOR VALIDITY

    // Check if file is a BMP
    if (header.type[0] != 'B' || header.type[1] != 'M')
    {
        printf("Not a BMP file.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    // Check if the DIB header is the one we expect
    if (headerInfo.size != 40)
    {
        printf("Unsupported BMP header.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    // Check if the BMP is 24-bit
    if (headerInfo.bitsPixel != 24)
    {
        printf("Only 24-bit BMP files are supported.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    // Check if the BMP is uncompressed
    if (headerInfo.compression != 0)
    {
        printf("Compressed BMP files are not supported.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }



    // Copy the complete header to the output
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
        // Close Files
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    // Allocate memory for each pixels
    for (int i = 0; i < height; i++)
    {
        image[i] = malloc(width * sizeof(Pixel));
        if (image[i] == NULL)
        {
            // Close Files
            fclose(inputFile);
            fclose(outputFile);
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



    // MODIFY EACH PIXEL ====================================================================================
    if (strcmp(filter, "grayscale") == 0)
    {
        grayscale(height, width, image);
    }
    else if (strcmp(filter, "sepia") == 0)
    {
        sepia(height, width, image);
    }
    else if (strcmp(filter, "blur") == 0)
    {
        blur(height, width, image);
    }
    else if (strcmp(filter, "fliph") == 0)
    {
        fliph(height, width, image);
    }
    else if (strcmp(filter, "flipv") == 0)
    {
        flipv(height, width, image);
    }
    else
    {
        printf("Usage: ./filter [Filter] ./inputPath.bmp ./outputPath.bmp\n");
        printf("Filters: 'Blur', 'Grayscale', 'Sepia', 'FlipH', 'FlipV'\n");
        
        // Close Files
        fclose(inputFile);
        fclose(outputFile);

        // Free allocated memory for pixels
        freeImage(height, image);

        return 1;
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
    freeImage(height, image);
}

// Free allocated memory for pixels
void freeImage(int height, Pixel **image)
{
    for (int i = 0; i < height; i++)
    {
        free(image[i]);
    }
    free(image);
}


// Returns 255 is the value is over 255(FOR PIXEL VALUES)
int over_255(int value)
{
    if (value < 255)
    {
        return value;
    }
    else
    {
        return 255;
    }
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

// Convert image to Sepia
void sepia(int height, int width, Pixel **image)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Get the original value of each RGB values of the pixel
            int originalRed = image[i][j].red;
            int originalGreen = image[i][j].green;
            int originalBlue = image[i][j].blue;

            // Apply the sepia formula to each pixel
            image[i][j].red = over_255(
                round((originalRed * 0.393) + (originalGreen * 0.769) + (originalBlue * 0.189)));
            image[i][j].green = over_255(
                round((originalRed * 0.349) + (originalGreen * 0.686) + (originalBlue * 0.168)));
            image[i][j].blue = over_255(
                round((originalRed * 0.272) + (originalGreen * 0.534) + (originalBlue * 0.131)));
        }
    }
}

// Add Box blur to image
void blur(int height, int width, Pixel **image)
{
    // Create a copy of image
    Pixel copy[height][width];
    for (int a = 0; a < height; a++)
    {
        for (int b = 0; b < width; b++)
        {
            copy[a][b] = image[a][b];
        }
    }

    // Check for each pixel of the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Total Red, Green, and Blue values of the neighbors of the current pixel
            int totalRed = 0;
            int totalGreen = 0;
            int totalBlue = 0;

            // Counter on how many neighbors the pixel have
            float counter = 0;

            // Check for each of the 3x3 neighbor of the current pixel
            for (int column = -1; column <= 1; column++)
            {
                for (int row = -1; row <= 1; row++)
                {
                    // Checks if the current neighbor is valid
                    if ((i + column) >= 0 && (i + column) < height && (j + row) >= 0 &&
                        (j + row) < width)
                    {
                        totalRed += copy[i + column][j + row].red;
                        totalGreen += copy[i + column][j + row].green;
                        totalBlue += copy[i + column][j + row].blue;
                        counter++;
                    }
                }
            }

            image[i][j].red = round(totalRed / counter);
            image[i][j].green = round(totalGreen / counter);
            image[i][j].blue = round(totalBlue / counter);
        }
    }
}


// Flip Horizontal
void fliph(int height, int width, Pixel **image)
{
    for (int i = 0; i < height; i++)
    {
        // Runs for half the image width (Round down if image width is odd)
        for (int j = 0, halfWidth = width / 2; j < halfWidth; j++)
        {
            // Stores the current pixel Pixel to a buffer
            Pixel buffer = image[i][j];

            // Swaps the current pixel and its opposite pixel
            image[i][j] = image[i][(width - 1) - j];
            image[i][(width - 1) - j] = buffer;
        }
    }
}

// Flip Vertical
void flipv(int height, int width, Pixel **image)
{
    for (int i = 0; i < width; i++)
    {
        // Runs for half the image height (Round down if image height is odd)
        for (int j = 0, halfHeight = height / 2; j < halfHeight; j++)
        {
            // Stores the current pixel Pixel to a buffer
            Pixel buffer = image[j][i];

            // Swaps the current pixel and its opposite pixel
            image[j][i] = image[(height - 1) - j][i];
            image[(height - 1) - j][i] = buffer;
        }
    }
}