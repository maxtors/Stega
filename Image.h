//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Image.h														  |
//----------------------------------------------------------------------------+

#ifndef IMAGE_H
#define IMAGE_H

#pragma warning( disable : 4996 4800 )

//-----------------------------------------------------------------------------
//  INCLUDES AND NAMESPACE
//-----------------------------------------------------------------------------
#include <iostream>					// CIN - COUT
#include <fstream>					// IFSTREAM - OFSTREAM
#include <string.h>					// STRCPY - STRCAT - STRCMP
#include <ctype.h>					// TOUPPER
#include "Constants.h"				// FILE WITH CONSTANS

using namespace std;				// STD::

// ----------------------------------------------------------------------------
//		MAIN IMAGE CLASS
// ----------------------------------------------------------------------------
class Image
{
public:
	// ------------------------------------------------------------------------
	//		CONSTRUCTORS / DECONSTRUCOR
	// ------------------------------------------------------------------------
	Image();							// Empty constructor
	~Image();							// Delete data and object

	// ------------------------------------------------------------------------
	//		PUBLIC FUNCTIONS
	// ------------------------------------------------------------------------
	
	void fLoad_image(char f[]);			// Loads a file
	void fLoad_image();					// Loads a file, after getting input
	void fShow_image_info();			// Shows basic information about image
	void fWrite_image_data();			// Write loaded image data to file
	void fUnload_image();				// Deletes data and unload information

	virtual void cyrpt() {}				// Perform action (add / extract) data
	virtual void crypt_created() {}		// If an action has been performed

protected:
	// ------------------------------------------------------------------------
	//		GENERAL PROTECTED DATA
	// ------------------------------------------------------------------------
	
	bool bValid_image_;					// If the image is valid
	bool bPixel_data_is_loaded_;		// If the pixel array is loaded
	bool bMSG_loaded_;					// If a message has been read
	
	bool bMSG_allocated_;

	bool bCrypt_is_made_;				// If a user has made or extracted info
	bool bData_is_written_;				// If the image data has been written

	bool bEncapsulated_loaded_;

	// ------------------------------------------------------------------------
	//		DATA STRUCTS
	// ------------------------------------------------------------------------
	struct Encapsulated		// Holds information about the data hiden in image
	{						// Se the "file structures" file for information
							// about how data is structured
		char header_moe[HEADER_MOE_SIZE      + 1];	// "MOE"
		char file_type [HEADER_FILETYPE_SIZE + 1];	// Type of file hidden
		char file_size [HEADER_FILE_SIZE     + 1];	// Size of file hidden

		int	 found_msg_size;			// Size of message data (bytes)
		char *MSG_DATA;					// Dynamic array for message DATA

	};

	struct Numericaldata				// NUMERICAL REPRESENTATION OF DATA
	{
		int file_size;					// Size of the file
		int data_offset;				// Were the data starts
		int width;						// Width  (px)
		int height;						// Height (px)
		int bitspp;						// Bits per pixel
		int data_size;					// Image data size (+0adding)

		int compression;				// If a compression method is used
		int ncolors;					// The number of colors
		int nimpcolors;					// Non-important colors

		int totalt_px;					// Amount of pixels in image
		int max_msg_size;				// Max size (bytes) of input message

	};

	struct Data
	{
		char magic_number	 [WORD];	// Should be 0x42 0x4D
		char file_size		[DWORD];	// The size of the entire file
		char empty_data		[DWORD];	// 4 empty bytes
		char data_offset	[DWORD];	// Offset of image data

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

		char *BMPDATA;					// Holds the image data in memory

	};

	Encapsulated	*stEncapsulated_;		// Encapsulated data (message)
	Numericaldata	*stImage_numerical_;	// Numerical data (from image)
	Data			*stImage_data_;			// Raw data		  (from image)

	// ------------------------------------------------------------------------
	//		PROTECTED FUCTIONS
	// ------------------------------------------------------------------------
	char ask(char msg[], char x, char y);		// As for user response
	void show_error(short e);					// Show what error ocurred
	void show_msg(STATE x, short e);			// Show a preformed action
	
	void allocate(TO_ALLOCATE x, int size = 0);	// Allocate dynamic data
	void deallocate(TO_ALLOCATE x);				// Deallocate dynamic data

private:
	// ------------------------------------------------------------------------
	//		PRIVATE DATA
	// ------------------------------------------------------------------------
	char file_name[STRLEN];				// Holds the filename
	bool bNumerical_struct_loaded_;
	bool bData_struct_loaded_;

	// ------------------------------------------------------------------------
	//		PRIVATE FUCTIONS
	// ------------------------------------------------------------------------
	bool init_check();						// An initial check of the file
	bool read_data();						// Loads the image into class
	bool read_header();						// Read all data into class
	void read_pxarr();						// Reads the main image data

	// READING AND WRITING DATA
	void read_block(ifstream  *inn, int &pos, char block[], int size);
	void write_block(ofstream *out, int &pos, char block[], int size);

	// Set and calculate numerical values
	bool set_numbers();						// Sets the numerical form
	int  calc(char block[], int size);		// Calculates to numerical
	bool check_numbers();					// Verifies the calculated data
	void calculate_type(char &t, float &n);	// Calculate visual data size

	void set_all_controls_false();			// Sets all control bools false
};
#endif