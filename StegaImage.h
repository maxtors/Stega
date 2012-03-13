#ifndef STEGAIMAGE_H
#define STEGAIMAGE_H

#include "Constants.h"

class Image
{
public:
	// ------------------------------------------------------------------------
	//		CONSTRUCTORS / DECONSTRUCOR
	//
	Image();							// Empty constructor
	Image(char f[]);					// Initiates class and reads file
	~Image();							// Delete data and object

	// ------------------------------------------------------------------------
	//		PUBLIC FUNCTIONS
	//
	void set_file_name(char f[]);		// Sets the filename

private:
	// ------------------------------------------------------------------------
	//		GENERAL PRIVATE DATA
	//
	bool valid_image;					// If the image is valid

	char file_name[STRLEN];				// Holds the filename
	char *BMPDATA;						// Holds the image data in memory

	// ------------------------------------------------------------------------
	//		DATA STRUCTS
	//
	struct Numericaldata				// NUMERICAL REPRESENTATION OF DATA
	{

		int file_size;					// Size of the file
		int data_offset;				// Were the data starts
		int width;						// Width  (px)
		int height;						// Height (px)
		int bitspp;						// Bits per pixel
		int data_size;					// Image data size (+0adding)

	} NUMERICALDATA;

	struct Fileheader					// FILE: HEADER INFORMATION
	{

		char magic_number	 [WORD];	// Should be 0x42 0x4D
		char file_size		[DWORD];	// The size of the entire file
		char empty_data		[DWORD];	// 4 empty bytes
		char data_offset	[DWORD];	// Offset of image data

	} BMPFILEHEADER;

	struct Infoheader					// IMAGE: HEADER INFORMATION
	{

		char header_size	[DWORD];	// Size of header (usualy 0x26)
		char width			[DWORD];	// Width  of image (px)
		char height			[DWORD];	// Height of image (px)
		char nplanes		 [WORD];	// Number of colorplanes
		char bitspp			 [WORD];	// Bits per pixel
		char compression	[DWORD];	// If any compression is used
		char data_size		[DWORD];	// Size of image data (+ padding)
		char hres			[DWORD];	// Horisontal resolution
		char vres			[DWORD];	// Vertical	  resolution
		char ncolors		[DWORD];	// Number of colors
		char nimpcolors		[DWORD];	// nonimportant colors

	} BMPINFOHEADER;

	// ------------------------------------------------------------------------
	//		PRIVATE FUCTIONS
	//
	bool load_image();							// Loads the image into class

	void read_data();							// Read all data into class
	char read_byte(ifstream *inn, int &pos);	// Read and return a byte

	void set_numericaldata();					// Sets the numerical form
	int calculate_numericaldata(char block[]);	// Calculates to numerical
};

class Steganography
{
public:
	Steganography();
	~Steganography();
protected:
private:
};

class Encrypt : public Steganography
{
public:
protected:
private:
};

class Decrypt : public Steganography
{
public:
protected:
private:
};

#endif