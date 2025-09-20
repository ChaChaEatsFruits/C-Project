#include "helpers.h"
#include "math.h"

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{   
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Calculate the average of the RGB values
            int red = image[i][j].rgbtRed;
            int green = image[i][j].rgbtGreen;
            int blue = image[i][j].rgbtBlue;
            int average = round((red + green + blue) / 3.0);

            // Set all color channels to the average value
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{   
    for (int i = 0; i < height; i++)
    {
        // Loop only to the middle of the row
        for (int j = 0; j < width / 2; j++)
        {
            // Use a temporary variable to swap pixels
            RGBTRIPLE temp = image[i][j];
            image[i][j] = image[i][width - 1 - j];
            image[i][width - 1 - j] = temp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{   
    // Create a temporary copy of the image
    RGBTRIPLE temp[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j] = image[i][j];
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float totalRed = 0, totalGreen = 0, totalBlue = 0;
            int counter = 0;

            // Iterate over the 3x3 grid around the pixel
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int new_i = i + di;
                    int new_j = j + dj;

                    // Check if the neighbor is within image bounds
                    if (new_i >= 0 && new_i < height && new_j >= 0 && new_j < width)
                    {
                        totalRed += temp[new_i][new_j].rgbtRed;
                        totalGreen += temp[new_i][new_j].rgbtGreen;
                        totalBlue += temp[new_i][new_j].rgbtBlue;
                        counter++;
                    }
                }
            }
            // Set pixel to the average of its neighbors
            image[i][j].rgbtRed = round(totalRed / counter);
            image[i][j].rgbtGreen = round(totalGreen / counter);
            image[i][j].rgbtBlue = round(totalBlue / counter);
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{   
    // Create a temporary copy of the image
    RGBTRIPLE temp[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            temp[i][j] = image[i][j];
        }
    }

    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float gxRed = 0, gyRed = 0;
            float gxGreen = 0, gyGreen = 0;
            float gxBlue = 0, gyBlue = 0;

            // Iterate over the 3x3 grid
            for (int di = -1; di <= 1; di++)
            {
                for (int dj = -1; dj <= 1; dj++)
                {
                    int new_i = i + di;
                    int new_j = j + dj;

                    // Check boundaries
                    if (new_i >= 0 && new_i < height && new_j >= 0 && new_j < width)
                    {
                        gxRed += temp[new_i][new_j].rgbtRed * Gx[di + 1][dj + 1];
                        gyRed += temp[new_i][new_j].rgbtRed * Gy[di + 1][dj + 1];
                        gxGreen += temp[new_i][new_j].rgbtGreen * Gx[di + 1][dj + 1];
                        gyGreen += temp[new_i][new_j].rgbtGreen * Gy[di + 1][dj + 1];
                        gxBlue += temp[new_i][new_j].rgbtBlue * Gx[di + 1][dj + 1];
                        gyBlue += temp[new_i][new_j].rgbtBlue * Gy[di + 1][dj + 1];
                    }
                }
            }

            int finalRed = round(sqrt(gxRed * gxRed + gyRed * gyRed));
            int finalGreen = round(sqrt(gxGreen * gxGreen + gyGreen * gyGreen));
            int finalBlue = round(sqrt(gxBlue * gxBlue + gyBlue * gyBlue));

            image[i][j].rgbtRed = (finalRed > 255) ? 255 : finalRed;
            image[i][j].rgbtGreen = (finalGreen > 255) ? 255 : finalGreen;
            image[i][j].rgbtBlue = (finalBlue > 255) ? 255 : finalBlue;
        }
    }
    return;
}
// Detect edges
void sepia(int height, int width, RGBTRIPLE image[height][width])
{   
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int originalRed = image[i][j].rgbtRed;
            int originalGreen = image[i][j].rgbtGreen;
            int originalBlue = image[i][j].rgbtBlue;

            int sepiaRed = round(.393 * originalRed + .769 * originalGreen + .189 * originalBlue);
            int sepiaGreen = round(.349 * originalRed + .686 * originalGreen + .168 * originalBlue);
            int sepiaBlue = round(.272 * originalRed + .534 * originalGreen + .131 * originalBlue);

            image[i][j].rgbtRed = (sepiaRed > 255) ? 255 : sepiaRed;
            image[i][j].rgbtGreen = (sepiaGreen > 255) ? 255 : sepiaGreen;
            image[i][j].rgbtBlue = (sepiaBlue > 255) ? 255 : sepiaBlue;
        }
    }
    return;
}
// Detect edges
void negative(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = 255 - image[i][j].rgbtRed;
            image[i][j].rgbtGreen = 255 - image[i][j].rgbtGreen;
            image[i][j].rgbtBlue = 255 - image[i][j].rgbtBlue;
        }
    }
    return;
}
