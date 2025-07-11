/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				Shayzexin
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{
	FILE *file = fopen(filename, "r");
	
	if (file == NULL) {
		fprintf(stderr, "Error opening file %s\n", filename);
		exit(EXIT_FAILURE);
	}	

	char format[3];
	fscanf(file, "%2s", format);
	
	if (strcmp(format, "P3") != 0) {
		fprintf(stderr, "Unsupported file format: %s\n", format);
		fclose(file);
		exit(EXIT_FAILURE);
	}
	
	uint32_t cols, rows, maxval;
	fscanf(file, "%" PRIu32 " %" PRIu32 "", &cols, &rows);
	fscanf(file, "%" PRIu32 "", &maxval);
	
	Image *image = malloc(sizeof(Image));
	if (image == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		fclose(file);
		exit(EXIT_FAILURE);	
	}

	image->rows = rows;
	image->cols = cols;	
	image->image = malloc(rows * sizeof(Color *));
	
	for (size_t i = 0; i < rows; i++) {
		image->image[i] = malloc(cols * sizeof(Color));
	}
	
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			uint8_t r, g, b;
			fscanf(file, "%" PRIu8 " %" PRIu8 " %" PRIu8 "", &r, &g, &b);
			image->image[i][j].R = r;
			image->image[i][j].G = g;
			image->image[i][j].B = b;
		}
	}
	
	fclose(file);

	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	if (image == NULL) {
		fprintf(stderr, "The parameter passed in is empty");	
		exit(EXIT_FAILURE);
	}
	
	uint32_t cols = image->cols;
	uint32_t rows = image->rows;
	
	printf("P3\n");
	printf("%" PRIu32 " %" PRIu32 "\n", cols, rows);
	printf("255\n");
	
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			uint8_t r = image->image[i][j].R;
			uint8_t g = image->image[i][j].G;
			uint8_t b = image->image[i][j].B;
			printf("%3" PRIu8 " %3" PRIu8 " %3" PRIu8 "", r, g, b);
			
			if (j < cols - 1) {
				printf("   ");
			}	
		}
		
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{
	if (image == NULL) {
		fprintf(stderr, "The parameter passed in is empty");	
		exit(EXIT_FAILURE);
	}
	
	uint32_t rows = image->rows;
	
	for (size_t i = 0; i < rows; ++i) {
		free(image->image[i]);
	}
	
	free(image->image);
	free(image);
}