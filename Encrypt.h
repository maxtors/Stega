//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Encrypt.h													  |
//----------------------------------------------------------------------------+

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

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
//		ENCRYPTION CLASS - INHERITS FROM IMAGE
// ----------------------------------------------------------------------------
class Encrypt : public Image
{
public:
	// ------------------------------------------------------------------------
	//		PUBLIC FUNCTIONS
	// ------------------------------------------------------------------------
	Encrypt();
	~Encrypt();
	void cyrpt();							// Inserts data to pixel array
	void crypt_created();					// If a user has made data

private:
	// ------------------------------------------------------------------------
	//		PRIVATE FUNCTIONS
	// ------------------------------------------------------------------------
	void read_data_from_file();					// Reads data file, checks if OK
	void read_data_from_input();				// Gets user input, checks if OK
	void insert_data();							// Insert MESSAGE into BMPDATA
	void block_insert(char buffer[], int &pos, int size);	// Insert a block
	void set_file_size_to_char();				// Convert int to 4byte char[]
};

#endif