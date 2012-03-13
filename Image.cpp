//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Image.cpp													  |
//----------------------------------------------------------------------------+

//-----------------------------------------------------------------------------
//  INCLUDES AND NAMESPACE
//-----------------------------------------------------------------------------
#include <iostream>					// CIN - COUT
#include <fstream>					// IFSTREAM - OFSTREAM
#include <string.h>					// STRCPY - STRCAT - STRCMP
#include <ctype.h>					// TOUPPER
#include "Image.h"					// MAIN IMAGE CLASS
#include "Constants.h"				// FILE WITH CONSTANS
using namespace std;				// STD::

// ****************************************************************************
//		PUBLIC FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		CONSTRUCTOR
// ----------------------------------------------------------------------------
Image::Image()  { set_all_controls_false(); }		// Set all bools false

Image::~Image() { deallocate(ALL_DYNAMIC_DATA); }	// Delete data

// ----------------------------------------------------------------------------
//		UNLOAD AN IMAGE
// ----------------------------------------------------------------------------
void Image::fUnload_image()
{
	deallocate(ALL_DYNAMIC_DATA);	// Delete dynamic data
	set_all_controls_false();		// Set controll bools false

	show_msg(INFO, MSG_0x7);		// Show that image was unloaded
}

// ----------------------------------------------------------------------------
//		LOAD AN IMAGE (WITH PARAMETER INDICATING WHICH FILE)
// ----------------------------------------------------------------------------
void Image::fLoad_image(char f[])
{
	bool regret = false;
	char buffer;

	/* This functions checks if any image is already loaded or that */
	/* maybe a message allready is extracted or any other controll  */
	/* boolean is set to TRUE										*/
	if (bValid_image_ || bPixel_data_is_loaded_ || bCrypt_is_made_ || bMSG_loaded_)
	{
		show_error(ERR_0x8);			// Show that image allready is loaded

										// Ask for a user choice

		buffer = ask("Are you sure you want to do this?", 'Y', 'N');
		
		if (buffer == 'Y')	// If yes continue
		{
			strcpy(file_name, f);		// Set file_name to parameter

			fUnload_image();			// Unload old data
			allocate(IMAGE_STRUCTS);	// Allocate image data structs

			show_msg(START, MSG_0x0);	// Show that we started loading
			bValid_image_ = read_data();// Reads data and returns if success
		}
		else							// If user does not want to continue
		{								// loading a new image
			regret = true;				// They regrett asking for new load
		}
	}
	else					// If no image allready loaded
	{
		strcpy(file_name, f);			// Set file_name to parameter
		show_msg(START, MSG_0x0);		// Show that we started loading image

		allocate(IMAGE_STRUCTS);	// Just incase they are not allocaed
		bValid_image_ = read_data();	// If read data succeded image is valid
	}


	/* Here we check if the image is valid, and or if the user regreted */
	/* invoking a new load												*/

	if (regret)							// If user regrets invoking load
	{
		show_msg(INFO, MSG_0x1);		// Show that the action was aborted
	}
	else								// If they dont regret
	{
		if (bValid_image_)				// If the image is valid
		{
			read_pxarr();				// Reads the main image data
			show_msg(DONE, MSG_0x0);	// Done loading image
		}
		else							// If the image is not valid
		{
			show_error(ERR_0x0);		// Show error, image not valid
			fUnload_image();			// Delete all data and start fresh
		}
	}
}

// ----------------------------------------------------------------------------
//		LOAD AN IMAGE (AND ASKING USER FOR WHAT FILE)
// ----------------------------------------------------------------------------
void Image::fLoad_image()
{
	char buffer[STRLEN];			// Buffer for filename

	cout << "Filename: ";			// Ask user for input
	cin.ignore();					// Ignore buffered \n
	cin.getline(buffer, STRLEN);	// Get filename
	
	// Here we test if the file can be opend, and that it exists
	ifstream inn(buffer, ios::binary | ios::in | ios::_Nocreate);

	if (inn)						// If we can open the file
	{
		inn.close();				// Close the stream
		fLoad_image(buffer);			// Load the desired file
	}
	else							// If we cant open the file
	{
		inn.close();				// Close the stream
		show_error(ERR_0x4);		// Show error, could not open file
	}
}

// ----------------------------------------------------------------------------
//		WRITE ALL THE DATA
// ----------------------------------------------------------------------------
void Image::fWrite_image_data()
{
	int pos = 0;					// Set starting position to 0

	if (!bValid_image_ || !bPixel_data_is_loaded_ || !bCrypt_is_made_)
	{
		if (!bValid_image_)			 show_error(ERR_0x0); // Not valid image
		if (!bPixel_data_is_loaded_) show_error(ERR_0x5); // Data not loaded
		if (!bCrypt_is_made_)		 show_error(ERR_0x6); // No reason for writing
	}
	else
	{
		ofstream out("output.bmp", ios::binary | ios::out);
		if (!out)
		{
			show_error(ERR_0x4); // Could not open file
		}
		else
		{
			show_msg(START, MSG_0x2);

			/* This part is the same as the reading process, but were just */
			/* writing the data out, the read_block and write_block are    */
			/* bassicaly the same, just one for each direction of data flow*/
		
			write_block(&out, pos, stImage_data_->magic_number,	 WORD);
			write_block(&out, pos, stImage_data_->file_size,	DWORD);
			write_block(&out, pos, stImage_data_->empty_data,	DWORD);
			write_block(&out, pos, stImage_data_->data_offset,	DWORD);

			write_block(&out, pos, stImage_data_->header_size,	DWORD);
			write_block(&out, pos, stImage_data_->width,		DWORD);
			write_block(&out, pos, stImage_data_->height,		DWORD);
			write_block(&out, pos, stImage_data_->nplanes,		 WORD);
			write_block(&out, pos, stImage_data_->bitspp,		 WORD);
			write_block(&out, pos, stImage_data_->compression,	DWORD);
			write_block(&out, pos, stImage_data_->data_size,	DWORD);
			write_block(&out, pos, stImage_data_->hres,			DWORD);
			write_block(&out, pos, stImage_data_->vres,			DWORD);
			write_block(&out, pos, stImage_data_->ncolors,		DWORD);
			write_block(&out, pos, stImage_data_->nimpcolors,	DWORD);

			out.write(stImage_data_->BMPDATA, stImage_numerical_->data_size);

			bData_is_written_ = true;
			show_msg(DONE, MSG_0x2);	
		}
	}
}

// ----------------------------------------------------------------------------
//		SHOW SOME INFORMATION ABOUT THE IMAGE
// ----------------------------------------------------------------------------
void Image::fShow_image_info()
{
	float size_fs, size_ms;
	char  type_fs, type_ms;

	if (bValid_image_)
	{
											// Set the sizes to temp floats
		size_fs = static_cast<float>(stImage_numerical_->file_size);
		size_ms = static_cast<float>(stImage_numerical_->max_msg_size - MPAD);

		calculate_type(type_fs, size_fs);	// Get the human readable
		calculate_type(type_ms, size_ms);	// datatype and size


											// Print out the info
		cout << "\n+--------------IMAGE INFO--------------+\n"
			 << "IMAGE NAME\t>\t" << file_name << "\n\n"
			 << "SIZE\t\t>\t" << size_fs << " " << type_fs << "B\n"
			 << "BITS PER PX\t>\t" << stImage_numerical_->bitspp << "\n\n"
			 << "HEIGHT\t\t>\t" << stImage_numerical_->height << "\n"
			 << "WIDTH\t\t>\t" << stImage_numerical_->width << "\n"
			 << "PIXELS\t\t>\t" << stImage_numerical_->totalt_px << "\n\n"
			 << "CAN HIDE\t>\t" << size_ms << " " << type_ms << "B\n"
			 << "+---------------------------------------+\n";
	}
	else
	{
		show_error(ERR_0x1);				// Image not valid
	}
}


// ****************************************************************************
//		PROTECTED FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		ALLOCATER MSG_DATA / ENCAPSULATED STRUCT
// ----------------------------------------------------------------------------
void Image::allocate(TO_ALLOCATE x, int size)
{
	switch (x)
	{
		case MSG_DATA:
			if (bMSG_allocated_)
			{
				delete [] stEncapsulated_->MSG_DATA;
			}
			stEncapsulated_->MSG_DATA = new char [size];
			bMSG_allocated_ = true;
			break;

		case ENCAPSULATED_STRUCT:
			if (!bEncapsulated_loaded_)
			{
				stEncapsulated_ = new Encapsulated;
				bEncapsulated_loaded_ = true;
			}
			break;

		case IMAGE_STRUCTS:
			if (!bNumerical_struct_loaded_)
			{
				stImage_numerical_			= new Numericaldata;
				bNumerical_struct_loaded_	= true;		// Calculations done
			}
			if (!bData_struct_loaded_)
			{
				stImage_data_				= new Data; 
				bData_struct_loaded_		= true;		// Data loaded
			}
			break;

		default: break; // If by some random chance we end up here...
	};
}

// ----------------------------------------------------------------------------
//		DEALLOCATER MSG_DATA / ENCAPSULATED STRUCT
// ----------------------------------------------------------------------------
void Image::deallocate(TO_ALLOCATE x)
{
	switch (x)
	{
		case MSG_DATA:
			if (bMSG_allocated_)
			{
				delete [] stEncapsulated_->MSG_DATA;		// Delete dynamic array
				bMSG_allocated_ = false;
			}
			break;

		case ENCAPSULATED_STRUCT:
			if (bEncapsulated_loaded_)
			{
				delete stEncapsulated_;
				bEncapsulated_loaded_ = false;
			}
			break;

		case ALL_DYNAMIC_DATA:
			if (bPixel_data_is_loaded_)
			{
				delete [] stImage_data_->BMPDATA;
				bPixel_data_is_loaded_ = false;
			}
			if (bMSG_allocated_)
			{
				delete [] stEncapsulated_->MSG_DATA;
				bMSG_allocated_ = false;
			}
			if (bEncapsulated_loaded_)
			{
				delete stEncapsulated_;
				bEncapsulated_loaded_ = false;
			}
			if (bNumerical_struct_loaded_)
			{
				delete stImage_numerical_;
				bNumerical_struct_loaded_ = false;
			}
			if (bData_struct_loaded_)
			{
				delete stImage_data_;
				bData_struct_loaded_ = false;
			}
			break;

		default: break; // If by some random chance we end up here...
	};
}

// ----------------------------------------------------------------------------
//		ASK FOR USER INPUT (Type: Yes / No, etc)
// ----------------------------------------------------------------------------
char Image::ask(char msg[], char x, char y)
{
	char buffer;

	x = toupper(x); y = toupper(y);

	do										// Loops until user says x or y
	{
		cout << msg << " (" << x << "/" << y << "): ";
		cin >> buffer;
		buffer = toupper(buffer);			// Set buffer to uppercase
	} while(buffer != x && buffer != y);

	return buffer;
}

// ----------------------------------------------------------------------------
//		SHOW AN ERROR MESSAGE
// ----------------------------------------------------------------------------
inline void Image::show_error(short e)
{
	if (e < NUM_ERRORS) cout << "\nERROR:\t" << ERROR[e] << '\n';
}

// ----------------------------------------------------------------------------
//		SHOW A GENERAL MESSAGE
// ----------------------------------------------------------------------------
inline void Image::show_msg(STATE x, short e)
{
	if (e < NUM_MSGS)
	{
		switch(x)
		{
			case START: cout << "\nSTART:\t"; break;
			case DONE : cout << "\nDONE :\t"; break;
			case INFO : cout << "\nINFO :\t"; break;
		}
		cout << MSG[e] << '\n';
	}
}


// ****************************************************************************
//		PRIVATE FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		PREFORM AN INITIAL DATA CHECK
// ----------------------------------------------------------------------------
bool Image::init_check()
{
									// Opens the file
	ifstream inn(file_name, ios::binary | ios::in);

	int start;						// Position of file start (0)
	int end;						// Position of file ending
	int size;						// Calculated size (in bytes)
	int i;							// For loop iterator
	int ac = 0;						// Acumulator
	char buffer;					// Buffer for char

	// ---------- THIS PART CALCULATES THE FILE SIZE --------------------------

	inn.seekg(0, ios::beg);					// Seeks to start of file
	start = static_cast<int>(inn.tellg());	// Gets position
	inn.seekg(0, ios::end);					// Seeks to end of file
	end = static_cast<int>(inn.tellg());	// Gets position

	size = end - start;						// Calculates size

	if (size <= W_MIN_BMP_SIZE)				// If size is less than we wanted
	{
		inn.close();
		return false;
	}
	// ------------------------------------------------------------------------

	// ---------- THIS PART CHECKS IF THE FILE STARTS WITH 0x42 0x4D ----------

	for (i = 0; i < 2; i++)			// Loops through the first two bytes
	{
		inn.seekg(i);				// Seeks to position
		inn.read(&buffer, 1);		// Reads byte into buffer
									// Adds to the accumulator
		ac |= static_cast<unsigned char>(buffer) << (i * 8);
	}

	if (ac != W_MAGIC_NUMBER)		// If the read data != 0x42 0x4D
	{
		inn.close();
		return false;
	}
	// ------------------------------------------------------------------------

	inn.close();					// If no errors were found the file is
	return true;					// Closed and returns true
}

// ----------------------------------------------------------------------------
//		READ DATA
// ----------------------------------------------------------------------------
bool Image::read_data()
{
	bool isGood = true;				// Bool to hold result of preformed action

	isGood = init_check();			// Does an initial check of the file
	if (!isGood) return false;

	isGood = read_header();			// Reads the headder into the class
	if (!isGood) return false;

	isGood = set_numbers();			// Sets the numerical values of read data
	if (!isGood) return false;

	return isGood;					// If all went well, return true
}

// ----------------------------------------------------------------------------
//		READ THE HEADER OF THE INPUT FILE
// ----------------------------------------------------------------------------
bool Image::read_header()
{
	int pos = 0;

	ifstream inn(file_name, ios::binary | ios::in);

	if (!inn)
	{
		return false;
	}
	else
	{
		read_block(&inn, pos, stImage_data_->magic_number,	 WORD);
		read_block(&inn, pos, stImage_data_->file_size,		DWORD);
		read_block(&inn, pos, stImage_data_->empty_data,	DWORD);
		read_block(&inn, pos, stImage_data_->data_offset,	DWORD);

		read_block(&inn, pos, stImage_data_->header_size,	DWORD);
		read_block(&inn, pos, stImage_data_->width,			DWORD);
		read_block(&inn, pos, stImage_data_->height,		DWORD);
		read_block(&inn, pos, stImage_data_->nplanes,		 WORD);
		read_block(&inn, pos, stImage_data_->bitspp,		 WORD);
		read_block(&inn, pos, stImage_data_->compression,	DWORD);
		read_block(&inn, pos, stImage_data_->data_size,		DWORD);
		read_block(&inn, pos, stImage_data_->hres,			DWORD);
		read_block(&inn, pos, stImage_data_->vres,			DWORD);
		read_block(&inn, pos, stImage_data_->ncolors,		DWORD);
		read_block(&inn, pos, stImage_data_->nimpcolors,	DWORD);
	}

	inn.close();
	return true;
}

// ----------------------------------------------------------------------------
//		READ THE PIXEL ARRAY IN THE INPUT FILE
// ----------------------------------------------------------------------------
void Image::read_pxarr()
{
	if (bValid_image_)				// If the image has been found to be valid
	{
									// Opens the file (since we know its valid)
		ifstream inn(file_name, ios::binary | ios::in);

									// Set data array to found size
		stImage_data_->BMPDATA = new char[stImage_numerical_->data_size];

									// Seek to start of data
		inn.seekg(stImage_numerical_->data_offset);

									// Read all the data
		inn.read(stImage_data_->BMPDATA, stImage_numerical_->data_size);

		inn.close();					// Close the file
		bPixel_data_is_loaded_ = true;	// Set true: we have loaded the data
	}
}

// ----------------------------------------------------------------------------
//		READ A BLOCK OF DATA
// ----------------------------------------------------------------------------
void Image::read_block(ifstream *inn, int &pos, char block[], int size)
{
	char buffer;					// Holds the char that has been read
	int  i;							// For loop itterator

	for (i = 0; i < size; i++)		// Loops n times (n = bytes)
	{
		(*inn).seekg(pos + i);		// Seeks to wanted position
		(*inn).read(&buffer, 1);	// Reads a byte of data

		block[i] = buffer;			// Sets pos i in block to the buffer
	}

	pos += size;					// Adds the read size to the position
}


// ----------------------------------------------------------------------------
//		CALCULATE THE NUMERICAL VALUES OF READ DATA
// ----------------------------------------------------------------------------
bool Image::set_numbers()
{
	/* Since all the numerical values that are stored in a BMP file are */
	/* stored in little endian form, we have to calculate ("reverse" )  */
	/* each block of data so we can represent it in a human readable    */
	/* form. This is both done to check the values, and to print out    */

	stImage_numerical_->file_size	 = calc(stImage_data_->file_size,	DWORD);
	stImage_numerical_->data_offset  = calc(stImage_data_->data_offset,	DWORD);
	stImage_numerical_->width		 = calc(stImage_data_->width,		DWORD);
	stImage_numerical_->height		 = calc(stImage_data_->height,		DWORD);
	stImage_numerical_->bitspp		 = calc(stImage_data_->bitspp,		 WORD);
	stImage_numerical_->data_size	 = calc(stImage_data_->data_size,	DWORD);
	stImage_numerical_->compression  = calc(stImage_data_->compression,	DWORD);
	stImage_numerical_->ncolors		 = calc(stImage_data_->ncolors,		DWORD);
	stImage_numerical_->nimpcolors	 = calc(stImage_data_->nimpcolors,	DWORD);

	stImage_numerical_->totalt_px	 = stImage_numerical_->width *
									   stImage_numerical_->height;

	stImage_numerical_->max_msg_size = (stImage_numerical_->data_size/8) - MPAD;

	return check_numbers();				// Checks the numbers
}

// ----------------------------------------------------------------------------
//		CHECK IF THE CALCULATETED NUMBERS ARE AS DESIRED
// ----------------------------------------------------------------------------
bool Image::check_numbers()
{
	bool isGood = true;					// By defualt everything is ok

										// Checks if the image is 24 bitspp
	if (stImage_numerical_->bitspp != W_BITSPP)					return false;

										// Checks if data starts at right place
	if (stImage_numerical_->data_offset != W_DATA_OFFSET)		return false;

										// Checks if we can add enough data
	if (stImage_numerical_->max_msg_size < W_MIN_MSG_SIZE)		return false;

										// Checks if sizes match up
	if ((stImage_numerical_->data_size + W_DATA_OFFSET) !=
		 stImage_numerical_->file_size)							return false;

										// Checks if no compression is used
	if (stImage_numerical_->compression != W_COMPRESSION)		return false;

										// Checks if number colors is set
	if (stImage_numerical_->ncolors != W_NCOLORS)				return false;

										// Checks if nonimportant colors is set
	if (stImage_numerical_->nimpcolors != W_NIMPCOLORS)			return false;

	return true;
}

// ----------------------------------------------------------------------------
//		CALCULATE A BLOCK OF DATA
// ----------------------------------------------------------------------------
int Image::calc(char block[], int size)
{
	/*    This block of code is needed since the number values		*/
	/*    that are stored in a BMP file, are stored with little-	*/
	/*    endian form. EX: FFAA00 should be read as, 00AAFF			*/

	char buffer;			// Buffer for current char
	int	i;					// For loop iterator
	int ac = 0;				// Accumulator to hold result

	for (i = size - 1; i >= 0; i--)
	{
		buffer = block[i];	// Read next char into buffer

							// Put value into the accumulator, we use
							// unsigned since we dont want the value to
							// be negative (ex: FF should be 255 not -1)
		ac |= static_cast<unsigned char>(buffer) << (i * 8);
	}

	return ac;
}

// ----------------------------------------------------------------------------
//		CALCULATE DATA SIZE FOR HUMAN READABILITY
// ----------------------------------------------------------------------------
inline void Image::calculate_type(char &t, float &n)
{
	int divisor = 1024;
	t = '\0';						// Set to empty strnig

	if (n >= divisor)				// If bigger than 1024
	{
		n /= divisor; t = 'K';		// Type = Kilo
	}
	if (n >= divisor)				// If still bigger than 1024
	{
		n /= divisor; t = 'M';		// Type = Mega
	}
}

// ----------------------------------------------------------------------------
//		WRITE A BLOCK OF DATA
// ----------------------------------------------------------------------------
inline void Image::write_block(ofstream *out, int &pos, char block[], int size)
{
	(*out).seekp(pos);				// Seeks to the wanted position

	(*out).write(block, size);		// Writes the block of data

									// Adds size of written to position
									// So that we start writing after this
	pos += size;					// Block, the next time
}

// ----------------------------------------------------------------------------
//		SET CONTROL BOOLS FALSE
// ----------------------------------------------------------------------------
inline void Image::set_all_controls_false()
{
	bValid_image_				= false;	// Image to not valid
	bPixel_data_is_loaded_		= false;	// Pixel array not loaded
	bMSG_loaded_				= false;	// Message not loaded
	bCrypt_is_made_				= false;	// "Cryptation" not created
	bData_is_written_			= false;	// Data not written

	bNumerical_struct_loaded_	= false;	// Calculations not done
	bData_struct_loaded_		= false;	// Data not loaded

	bMSG_allocated_				= false;	// Message array is not allocated
	bEncapsulated_loaded_		= false;	// Encapsulated struct not loaded
}