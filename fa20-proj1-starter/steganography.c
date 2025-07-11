/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	Color *color = malloc(sizeof(Color));
	if (color == NULL) {
		fprintf(stderr, "Memory allocation error");
		exit(EXIT_FAILURE);
	}

	uint8_t lsb = image->image[row][col].B & 1; // Get the LSB of the B channel

	if (lsb == 0) {
		color->R = 0;
		color->G = 0;
		color->B = 0; // Black
	} else {
		color->R = 255;
		color->G = 255;
		color->B = 255; // White
	}

	return color;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
 	Image *newImage = malloc(sizeof(Image));
	if (newImage == NULL) {
		fprintf(stderr, "Memory allocation error");
		exit(EXIT_FAILURE);
	}

	uint32_t row = image->rows;
	uint32_t col = image->cols;

	newImage->cols = col;
	newImage->rows = row;
	newImage->image = malloc(row * sizeof(Color *));
	if (newImage->image == NULL) {
		fprintf(stderr, "Memory allocation error");
		free(newImage);
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < row; ++i) {
		newImage->image[i] = malloc(col * sizeof(Color));
		if (newImage->image[i] == NULL) {
			fprintf(stderr, "Memory allocation error");
			for (uint32_t j = 0; j < i; ++j) {
				free(newImage->image[j]);
			}
			free(newImage->image);
			free(newImage);
			exit(EXIT_FAILURE);
		}
	}

	for (size_t i = 0; i < row; ++i) {
		for (size_t j = 0; j < col; ++j) {
			Color *tempColor = evaluateOnePixel(image, i, j);
			newImage->image[i][j] = *tempColor;
			free(tempColor);
		}
	}

	return newImage;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	Image *image = readData(argv[1]);
	if (image == NULL) {
		fprintf(stderr, "Failed to read image from file %s\n", argv[1]);
		return EXIT_FAILURE;
	}

	Image *newImage = steganography(image);
	if (newImage == NULL) {
		fprintf(stderr, "Failed to create new image\n");
		free(image);
		return EXIT_FAILURE;
	}

	writeData(newImage);

	// Free memory
	for (size_t i = 0; i < image->rows; ++i) {
		free(image->image[i]);
	}
	free(image->image);
	free(image);

	for (size_t i = 0; i < newImage->rows; ++i) {
		free(newImage->image[i]);
	}
	free(newImage->image);
	free(newImage);

	return EXIT_SUCCESS;
}
