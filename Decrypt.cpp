//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Decrypt.cpp													  |
//----------------------------------------------------------------------------+

//-----------------------------------------------------------------------------
//  INCLUDES AND NAMESPACE
//-----------------------------------------------------------------------------
#include <iostream>					// CIN - COUT
#include <fstream>					// IFSTREAM - OFSTREAM
#include <string.h>					// STRCPY - STRCAT - STRCMP
#include <ctype.h>					// TOUPPER
#include "Image.h"					// MAIN IMAGE CLASS
#include "Decrypt.h"				// DERIVED DECRYPTION CLASS (EXTRACTS DATA)
#include "Constants.h"				// FILE WITH CONSTANS
using namespace std;				// STD::

// ****************************************************************************
//		PUBLIC FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		CONSTRUCTOR
// ----------------------------------------------------------------------------
Decrypt::Decrypt()	{}

Decrypt::~Decrypt() { deallocate(ALL_DYNAMIC_DATA); }

// ----------------------------------------------------------------------------
//		CRYPT
// ----------------------------------------------------------------------------
void Decrypt::cyrpt()
{
	char buffer;

	if (bValid_image_)						// If image is valid
	{
		allocate(ENCAPSULATED_STRUCT);		// Allocate struct for encapsulated
		bMSG_loaded_ = false;				// Set that MSG is not loaded
		extract_data();						// Extract the data

		if (bMSG_loaded_)					// If the MSG was loaded
		{
			// The user is asked where they want to dump the extracted
			// data, either to a file, or to the screen
			cout << "\n\tF - File" << "\n\tS - Screen\n";
			buffer = ask("What is the source of output?", 'F', 'S');

			if		(buffer == 'F')	save_data_to_file();
			else if (buffer == 'S')	save_data_to_screen();

			bCrypt_is_made_ = true;			// Data has been extracted
			bData_is_written_ = false;		// Data has not been saved
		}
		else
		{
			deallocate(MSG_DATA);			// Deallocate the MSG data array
			deallocate(ENCAPSULATED_STRUCT);// Deallocate the encapsulated struct
		}
	}
	else
	{
		show_error(ERR_0x0);				// Error: image not valid
	}
}

// ----------------------------------------------------------------------------
//		IS CRYPT CREATED
// ----------------------------------------------------------------------------
void Decrypt::crypt_created() {} /* Her kan du legge inn noe som spør om
								    brukeren vil lagre den uthentede dataen
								    hvis den ikke har blitt lagret			*/


// ****************************************************************************
//		PRIVATE FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		CHECK THE FILE TYPE (SHOULD ONLY CONTAIN ALPHA NUMERICAL CHARS)
// ----------------------------------------------------------------------------
void Decrypt::check_file_type()
{
	int i;												// For loop iterator
	for (i = 0; i < HEADER_FILETYPE_SIZE; i++)			// Loop through data
	{
		if (!isalnum(stEncapsulated_->file_type[i]))	// If char != alphanum
		{
			strcpy(stEncapsulated_->file_type, "txt");	// Set type to txt
			break;										// break out of loop
		}
	}
}

// ----------------------------------------------------------------------------
//		EXTRACT DATA FROM FILE
// ----------------------------------------------------------------------------
void Decrypt::extract_data()
{
	int i;							// For loop iterator
	int pos;						// Position in data array
	unsigned char size_buffer;		// Buffer for size data

									// Allocate the MSG_DATA array
	allocate(MSG_DATA, stImage_numerical_->max_msg_size - MPAD);

	show_msg(START, MSG_0x3);		// Show that we started extracting

	pos = 0;						// Set the starting position

									// Extract header_moe (3 byte)
	block_extract(stEncapsulated_->header_moe, pos, HEADER_MOE_SIZE);

									// Extract file type (3 bytes)
	block_extract(stEncapsulated_->file_type, pos, HEADER_FILETYPE_SIZE);

									// Extract file size (4 bytes)
	block_extract(stEncapsulated_->file_size, pos, HEADER_FILE_SIZE);


	/* This function converts a 4 byte char array to an integer*/
	stEncapsulated_->found_msg_size = 0;
	for (i = 0; i < 4; i++)
	{
		size_buffer = static_cast<unsigned char>(stEncapsulated_->file_size[i]);
		stEncapsulated_->found_msg_size |= size_buffer << ((3 - i) * 8);
	}

	// IF NO FILENAME IS SET, SET IT TO WRITE TO .TXT
	if (strlen(stEncapsulated_->file_type) == 0)
	{
		strcpy(stEncapsulated_->file_type, "txt");
	}

	// IF THE FOUND SIZE IS NOT > 0
	// OR THE START HEADER != MOE THEN THE FILE IS NOT VALID
	if (stEncapsulated_->found_msg_size <= 0 ||
		strcmp(stEncapsulated_->header_moe, "MOE"))
	{
		bMSG_loaded_ = false;
		show_error(ERR_0xB);
	}
	else
	{
		// If the extracted data header is valid, then
		// we can extract the message data that comes afterwards
		block_extract(stEncapsulated_->MSG_DATA, pos,
					  stEncapsulated_->found_msg_size);

		show_msg(DONE, MSG_0x3);		// Show that we are done extracting
		bMSG_loaded_ = true;			// True: a message is loaded
	}
}

// ----------------------------------------------------------------------------
//		SHOW FOUND DATA TO SCREEN
// ----------------------------------------------------------------------------
void Decrypt::save_data_to_screen()
{
	char buffer;


	// Check how big the output is, if it is over N bytes it asks for a choice
	// but if the file is over M in size it does not alow output
	if (stEncapsulated_->found_msg_size >  LIMIT_FOR_SCREENPRINT &&
		stEncapsulated_->found_msg_size <= LIMIT_FOR_FILEPRINT)
	{
		cout << "This file is over " << LIMIT_FOR_SCREENPRINT << "\n";

										// Ask for user input
		buffer = ask("Are you sure you want to print?", 'Y', 'N');

		if (buffer == 'Y')				// If yes, print to screen
		{
			cout << "\n\n" << stEncapsulated_->MSG_DATA << "\n\n";
		}
		else							// If no, show a message
		{
			show_msg(INFO, MSG_0x6);	// Extracted data not written
		}
	}
	else if (stEncapsulated_->found_msg_size > LIMIT_FOR_FILEPRINT)
	{
		cout << "This file is over " << LIMIT_FOR_FILEPRINT
			 << " bytes, so it will not be printed\n";

										// Ask for user input
		buffer = ask("Would you like to save to file instead?", 'Y', 'N');

		if (buffer == 'Y')				// If yes, save data to file
		{
			save_data_to_file();
		}
		else							// If no, show a message
		{
			show_msg(INFO, MSG_0x6);	// Extracted data not written
		}
	}
	else
	{
										// Write the data to the screen
		cout << "\n\n" << stEncapsulated_->MSG_DATA << "\n\n";
	}
}

// ----------------------------------------------------------------------------
//		SAVE FOUND DATA TO FILE
// ----------------------------------------------------------------------------
void Decrypt::save_data_to_file()
{
	int i;								// For loop iterator
	char buffer;						// Buffer for holding a byte of data

	char buffer_file[STRLEN];

	strcpy(buffer_file, "extract.");	// Set the name of outputfile

	check_file_type();

	strcat(buffer_file, stEncapsulated_->file_type);		// Append the filetype
	
	show_msg(START, MSG_0x5);			// Show that we are starting to write

	ofstream out(buffer_file, ios::binary | ios::out);
	if (out)							// If output file can be created
	{
										// Loop trough the message data
		for (i = 0; i < stEncapsulated_->found_msg_size; i++)
		{
										// Get the next byte of data
			buffer = stEncapsulated_->MSG_DATA[i];

			out.write(&buffer, 1);		// Write it
		}
		out.close();					// When done, close the file
		show_msg(DONE, MSG_0x5);		// Show that we are done writing
	}
	else
	{
		show_error(ERR_0xC);			// Show error message
	}
}

// ----------------------------------------------------------------------------
//		EXTRACT A BLOCK OF DATA
// ----------------------------------------------------------------------------
void Decrypt::block_extract(char buffer[], int &pos, int size)
{
	int i;								// For loop iterator
	int ac;								// Accumulator for data
	int bit;							// Position in byte
	char ch;							// Buffer for data we are working with
	
	for (i = 0; i < size; i++)			// Loop wanted size of extract
	{
		ac = 0;							// Set ac to empty
		for (bit = 7; bit >= 0; bit--)	// Loop through each bit
		{
			ch = stImage_data_->BMPDATA[pos];	// Read the next char

			if (ch & 1)					// If this byte ends with a 1
			{
				ac |= 1 << bit;			// Append 1 LFS according to bit pos
			}

			pos++;						// Increment pos (move to next)
		}
		buffer[i] = ac;
	}
	buffer[i] = '\0';					// Set end of cstring char
}