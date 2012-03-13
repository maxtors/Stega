//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Encrypt.cpp													  |
//----------------------------------------------------------------------------+

//-----------------------------------------------------------------------------
//  INCLUDES AND NAMESPACE
//-----------------------------------------------------------------------------
#include <iostream>					// CIN - COUT
#include <fstream>					// IFSTREAM - OFSTREAM
#include <string.h>					// STRCPY - STRCAT - STRCMP
#include <ctype.h>					// TOUPPER
#include "Image.h"					// MAIN IMAGE CLASS
#include "Encrypt.h"				// DERIVED ENCRYPTION CLASS (ADDS DATA)
#include "Constants.h"				// FILE WITH CONSTANS
using namespace std;				// STD::

// ****************************************************************************
//		PUBLIC FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		CONSTRUCTOR
// ----------------------------------------------------------------------------
Encrypt::Encrypt() {}

Encrypt::~Encrypt() { deallocate(ALL_DYNAMIC_DATA); }

// ----------------------------------------------------------------------------
//		CRYPT
// ----------------------------------------------------------------------------
void Encrypt::cyrpt()
{
	char buffer;							// Create a buffer for input

	if (bValid_image_ && !bCrypt_is_made_)	// Check if the image is valid
	{
		allocate(ENCAPSULATED_STRUCT);

		bMSG_loaded_ = false;				// Set that message isnt loaded
		
											// Ask where date comes from
		cout << "\n\tF - File" << "\n\tI - Input\n";
		buffer = ask("What is the source of input?", 'F', 'I');

		if		(buffer == 'F')	read_data_from_file();		// If file
		else if (buffer == 'I')	read_data_from_input();		// If keyboard

		if (bMSG_loaded_)					// If the message is loaded
		{
			insert_data();					// Insert the data into BMPDATA

			bCrypt_is_made_ = true;			// Set that the "crypt" is made
			bData_is_written_ = false;		// Set that we have not writen yet
		}
		else								// If no message is loaded
		{
			show_error(ERR_0x7);			// Failed adding data

			deallocate(MSG_DATA);
			deallocate(ENCAPSULATED_STRUCT);
		}
	}
	else
	{
		if (!bValid_image_)  show_error(ERR_0x0);	// Image is not valid
		if (bCrypt_is_made_) show_error(ERR_0xD);	// error that crypt is made
	}
}

// ----------------------------------------------------------------------------
//		IS CRYPT CREATED
// ----------------------------------------------------------------------------
void Encrypt::crypt_created()
{
	char buffer;				// For user input

								// If a message has been added to the image
								//data, but not saved to a file
	if (bCrypt_is_made_ && !bData_is_written_)
	{
		show_error(ERR_0x3);	// Show error, and ask if user wants to save
		buffer = ask("Do you want to save your data?", 'Y', 'N');

		if (buffer == 'Y')		fWrite_image_data();	// Save data
		else if (buffer == 'N')	show_error(ERR_0x3);	// No data saved
	}
}


// ****************************************************************************
//		PRIVATE FUNCTIONS
// ****************************************************************************

void Encrypt::set_file_size_to_char()
{
	int i;					// For loop iterator
	char buffer;			// Buffer for data

	/* This function takes an integer and turns it into a 4 byte char array */

	for (i = HEADER_FILE_SIZE - 1; i >= 0; i--)		// Loop throug size of data
	{
		buffer = static_cast<char>(stEncapsulated_->found_msg_size >> (i * 8));
		stEncapsulated_->file_size[(HEADER_FILE_SIZE - 1)-i] = buffer;
	}
}

// ----------------------------------------------------------------------------
//		GET DATA FROM FILE
// ----------------------------------------------------------------------------
void Encrypt::read_data_from_file()
{
	int seek_pos_null = 0;
	int start, end, size;				// For calculating size of file
	char file_to_encapsulate[STRLEN];	// Name of the file we want to insert
	bool bFile_is_ok = false;			// If the data if is good

	cin.ignore();								// Ignore buffered \n
	cout << "Filename: ";						// Ask user for input
	cin.getline(file_to_encapsulate, STRLEN);	// Get filename
	
	ifstream inn(file_to_encapsulate, ios::binary | ios::_Nocreate);
	if (inn)						// If this file exsists
	{
		inn.seekg(seek_pos_null);	// CALCULATE SIZE OF FILE
		start = static_cast<int>(inn.tellg());
		inn.seekg(seek_pos_null, ios::end);
		end = static_cast<int>(inn.tellg());
		size = end - start;
									// IF the file is to big
		if (size > (stImage_numerical_->max_msg_size - MPAD))
		{
			show_error(ERR_0x9);	// Show an error message
		}
		else						// If the file size is good
		{
			bFile_is_ok = true;		// Set the bool to true
		}
	}
	else
	{
		show_error(ERR_0x4);		// Show that we couldnt open the file
	}

	if (bFile_is_ok)
	{
		cout << "Plese enter the file type, ex: txt, pdf (max three letters)\n"
			 << "Filetype: ";					// Get the filetype (ex .txt)
		cin >> stEncapsulated_->file_type;		// Read input

		stEncapsulated_->found_msg_size = size;
		allocate(MSG_DATA, stEncapsulated_->found_msg_size);

												// Seek to start of data
		inn.seekg(seek_pos_null);				// And read all the data
		inn.read(stEncapsulated_->MSG_DATA, stEncapsulated_->found_msg_size);
		inn.close();							// Close the file

		set_file_size_to_char();				// Set the data size
		bMSG_loaded_ = true;					// Set true -> we have read msg
	}
	else
	{
		bMSG_loaded_ = false;
	}
}

// ----------------------------------------------------------------------------
//		GET DATA FROM USER INPUT
// ----------------------------------------------------------------------------
void Encrypt::read_data_from_input()
{
	int i;									// For loop iterator

	allocate(MSG_DATA, stImage_numerical_->max_msg_size - MPAD);

	for (i = 0; i < HEADER_FILETYPE_SIZE; i++)		// Loop through array size
		stEncapsulated_->file_type[i] = 0;			// Set to empty

	cin.ignore();									// Ask for user input
	cout << "\nENTER DATA (Press ENTER to end): ";	// Get the user input
	cin.getline(stEncapsulated_->MSG_DATA, stImage_numerical_->max_msg_size-MPAD);

											// Set the size of user input data
	stEncapsulated_->found_msg_size = strlen(stEncapsulated_->MSG_DATA);

	set_file_size_to_char();				// Set the data size
	bMSG_loaded_ = true;					// Set true that we have read msg
}

// ----------------------------------------------------------------------------
//		INSERT DATA INTO BITMAP PIXEL ARRAY
// ----------------------------------------------------------------------------
void Encrypt::insert_data()
{
	int pos = 0;							// Set position to start (ios::beg)
	show_msg(START, MSG_0x4);				// Show: We are starting to insert

	strcpy(stEncapsulated_->header_moe, "MOE");
	block_insert(stEncapsulated_->header_moe, pos,	// Insert the header "MOE"
				 HEADER_MOE_SIZE);

	block_insert(stEncapsulated_->file_type, pos,	// Insert file type
				 HEADER_FILETYPE_SIZE);

	block_insert(stEncapsulated_->file_size, pos,	// Insert file size
				 HEADER_FILE_SIZE);

	block_insert(stEncapsulated_->MSG_DATA, pos,	// Insert the message data
				 stEncapsulated_->found_msg_size);

	show_msg(DONE, MSG_0x4);				// Show: we are DONE inserting
}

// ----------------------------------------------------------------------------
//		BLOCK INSERT
// ----------------------------------------------------------------------------
void Encrypt::block_insert(char buffer[], int &pos, int size)
{
	int  i;									// For loop iterator
	int  bit;								// N-bit in the byte
	char buffer_msg;						// Buffer for a message byte
	char buffer_data;						// Buffer for a image data byte

	for (i = 0; i < size; i++)				// Loop through the whole message
	{
		buffer_msg = buffer[i];				// Get the byte we want to work with

		for (bit = 7; bit >= 0; bit--)		// Loop through bits of buffer_msg
		{
			buffer_data = stImage_data_->BMPDATA[pos];	// Pull char to buffer

											// If the LSB's dont match
			if (bool((buffer_msg & (1 << bit))) != (buffer_data & 1))
			{
				// IF LSB of image data == 1 THEN -> set LSB to 0
				if (buffer_data & 1)	buffer_data &= (-1 ^ 1);
				
				// IF LSB of image data != 1 THEN -> SET LSB TO 1
				else					buffer_data |= 1;
			}

			stImage_data_->BMPDATA[pos] = buffer_data;	// Set the new value
			pos++;										// Increment position
		}
	}
}