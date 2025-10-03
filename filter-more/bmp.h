#include <stdint.h>

// Basic data types for BMP headers
typedef uint8_t  BYTE;   // 8-bit unsigned
typedef uint16_t WORD;   // 16-bit unsigned
typedef uint32_t DWORD;  // 32-bit unsigned
typedef int32_t  LONG;   // 32-bit signed

// BITMAPFILEHEADER: general info about the BMP file
typedef struct
{
    WORD   bfType;       // File type, must be 'BM'
    DWORD  bfSize;       // File size in bytes
    WORD   bfReserved1;  // Reserved, usually 0
    WORD   bfReserved2;  // Reserved, usually 0
    DWORD  bfOffBits;    // Offset to start of pixel data
} __attribute__((__packed__)) BITMAPFILEHEADER;

// BITMAPINFOHEADER: info about image dimensions and color
typedef struct
{
    DWORD  biSize;          // Size of this header (40 bytes)
    LONG   biWidth;         // Image width in pixels
    LONG   biHeight;        // Image height in pixels
    WORD   biPlanes;        // Number of color planes (must be 1)
    WORD   biBitCount;      // Bits per pixel (e.g., 24 for RGB)
    DWORD  biCompression;   // Compression type (0 = none)
    DWORD  biSizeImage;     // Image size (may be 0 for uncompressed)
    LONG   biXPelsPerMeter; // Horizontal resolution
    LONG   biYPelsPerMeter; // Vertical resolution
    DWORD  biClrUsed;       // Number of colors in palette (0 = all)
    DWORD  biClrImportant;  // Important colors (0 = all)
} __attribute__((__packed__)) BITMAPINFOHEADER;

// RGBTRIPLE: stores one pixel's color
typedef struct
{
    BYTE  rgbtBlue;
    BYTE  rgbtGreen;
    BYTE  rgbtRed;
} __attribute__((__packed__)) RGBTRIPLE;