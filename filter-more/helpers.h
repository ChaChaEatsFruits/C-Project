#include "bmp.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width]);

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width]);

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width]);

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width]);

// Negative image
void negative(int height, int width,RGBTRIPLE image[height][width]);

// Sepia filter
void sepia(int height, int width,RGBTRIPLE image[height][width]);

// sharpen filter
void sharpen(int height, int width, RGBTRIPLE image[height][width]);

// emboss filter
void emboss(int height, int width, RGBTRIPLE image[height][width]);