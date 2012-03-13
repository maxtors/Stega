//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Decrypt.h													  |
//----------------------------------------------------------------------------+

#ifndef DECRYPTION_H
#define DECRYPTION_H

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

// ----------------------------------------------------------------------------
//		DECRYPTION CLASS - INHERITS FROM IMAGE
// ----------------------------------------------------------------------------
class Decrypt : public Image
{
public:
	// ------------------------------------------------------------------------
	//		PUBLIC FUNCTIONS
	// ------------------------------------------------------------------------
	Decrypt();
	~Decrypt();
	void cyrpt();							// Extracts data from pixel array
	void crypt_created();					// If a user has extracted data

private:
	// ------------------------------------------------------------------------
	//		PRIVATE FUNCTIONS
	// ------------------------------------------------------------------------
	void save_data_to_screen();					// Viser funnet data på skjermen
	void save_data_to_file();					// Lagrer funnet data til en fil
	void extract_data();						// Extract data from image
	void block_extract(char buffer[], int &pos, int size);	// Extract a block
	void check_file_type();						// Check the bytes indicating
};												// the data filetype
#endif