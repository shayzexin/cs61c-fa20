/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				Shayzexin
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

uint8_t directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},  
    {1, -1},  {1, 0},  {1, 1}
};

bool isAlive(Image *image, uint32_t row, uint32_t col, uint8_t bit, uint8_t channel) {
	uint8_t givenByte;
	
	switch (channel) {
		case 0:
			givenByte = image->image[row][col].R;	
			break;
		case 1:
			givenByte = image->image[row][col].G;
			break;
		case 2:
			givenByte = image->image[row][col].B;
	}
	
	return (givenByte >> bit) & 1;
}

bool evalueateEachBit(Image *image, uint32_t row, uint32_t col, uint32_t rule, uint8_t bit, uint8_t channel) {
	uint8_t CountNeighborsAlive = 0;
	uint32_t num_rows = image->rows;
	uint32_t num_cols = image->cols;

	for (size_t i = 0; i < 8; ++i) {
		uint32_t new_row = row + directions[i][1];
		uint32_t new_col = col + directions[i][0];
		
		new_row = (new_row + num_rows) % num_rows;
		new_col = (new_col + num_cols) % num_cols;

		CountNeighborsAlive += isAlive(image, new_row, new_col, bit, channel);
	}	
	
	bool isOne = isAlive(image, row, col, bit, channel);

	return (isOne) ? ((rule >> 9) >> CountNeighborsAlive) & 1: (rule >> CountNeighborsAlive) & 1;
}

//Determines what color the cell at the given row/col should be. This function allocates space for a new Color.
//Note that you will need to read the eight neighbors of the cell in question. The grid "wraps", so we treat the top row as adjacent to the bottom row
//and the left column as adjacent to the right column.
Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule)
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;

	//R
	for (size_t i = 0; i < 8; ++i) {
		bool nextBitOfRed = evalueateEachBit(image, row, col, rule, i, 0);			
		if (nextBitOfRed) {
			r |= (1 << i);
		}
	}
	
	//G
	for (size_t i = 0; i < 8; ++i) {
		bool nextBitOfGreen = evalueateEachBit(image, row, col, rule, i, 1);
		if (nextBitOfGreen) {
			g |= (1 << i);
		}
	}
	
	//B
	for (size_t i = 0; i < 8; ++i) {
		bool nextBitOfBlue = evalueateEachBit(image, row, col, rule, i, 2);
		if (nextBitOfBlue) {
			b |= (1 << i);
		}
	}
	
	Color *color = malloc(sizeof(Color));
	if (color == NULL) {
		fprintf(stderr, "Memory allocation error");
		exit(EXIT_FAILURE);
	}	

	color->R = r;
	color->G = g;
	color->B = b;
	return color;
}

//The main body of Life; given an image and a rule, computes one iteration of the Game of Life.
//You should be able to copy most of this from steganography.c
Image *life(Image *image, uint32_t rule)
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
			Color *tempColor = evaluateOneCell(image, i, j, rule);	
			newImage->image[i][j] = *tempColor;
			free(tempColor);
		}
	}
	
	return newImage;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this will be a string.
You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/
int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "./gameOfLife usage: ./gameOfLife filename rule    filename is an ASCII PPM file (type P3) with maximum value 255.    rule is a hex number beginning with 0x; Life is 0x1808.\n");
		return EXIT_FAILURE;		
	}
	
	char *filename = argv[1];
	char *rule_str = argv[2];
	
	uint32_t rule = (uint32_t)strtoul(rule_str, NULL, 0);
	
	Image *inputImage = readData(filename);

	if (inputImage == NULL) {
		fprintf(stderr, "Could not load image from %s/n", filename);
		return EXIT_FAILURE;
	}
	
	Image *outputImage = life(inputImage, rule);

	if (outputImage == NULL) {
		fprintf(stderr, "life function failed to produce output image.\n");
		freeImage(inputImage);
		return EXIT_FAILURE;
	}
	
	writeData(outputImage);
	
	freeImage(inputImage);
	freeImage(outputImage);
	
	return EXIT_SUCCESS;
}
