//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//----------------------------------------------------------------------------+

//----------------------------------------------------------------------------+
//  COPYRIGHT INFORMATION													  |
//----------------------------------------------------------------------------+
//  Copyright (c) 2012 by Andreas Moe										  |
//																			  |
//  Permission is hereby granted, free of charge, to any person obtaining a	  |
//  copy of this software and associated documentation files (the			  |
//  "Software"), to deal in the Software without restriction, including		  |
//  without limitation the rights to use, copy, modify, merge, publish,       |
//  distribute, sublicense, and/or sell copies of the Software, and to permit |
//  persons to whom the Software is furnished to do so, subject to the		  |
//  following conditions:													  |
//																		      |
//  The above copyright notice and this permission notice shall be			  |
//  included in all copies or substantial portions of the Software.			  |
//																			  |
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   |
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF				  |
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.    |
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY      |
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,      |
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE         |
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.					  |
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
#include "Decrypt.h"				// DERIVED DECRYPTION CLASS (EXTRACTS DATA)
#include "Constants.h"				// FILE WITH CONSTANS
using namespace std;				// STD::

//-----------------------------------------------------------------------------
//  GLOBAL VARIABLES AND DATA
//-----------------------------------------------------------------------------
enum Menu {MAIN, ENC, DEC};			// LIST OF MENUS

// ----------------------------------------------------------------------------
//  PROGRAM FUNCTIONS
// ----------------------------------------------------------------------------
void read_choice(char &ch);			// Reads a char input from user
void write_menu(Menu t);			// Writes the wanted menu
void sub_menu(Menu t);				// Shows and handles submenues
void information();					// Shows information about the program

// ----------------------------------------------------------------------------
//  MAIN
// ----------------------------------------------------------------------------
int main()
{
	char command;								// Buffer for user menu choice

	write_menu(MAIN);							// Show the main menu
	read_choice(command);						// Read user choice

	while (command != 'Q')						// As long as not user type Q
	{
		switch(command)
		{
			case 'E': sub_menu(ENC); break;		// Go to submenu: Encryption
			case 'D': sub_menu(DEC); break;		// Go to sumbemu: Decryption
			case 'I': information(); break;		// Show program information

			default: cout << "\nERROR: INVALID INPUT\n"; break;
		}

		write_menu(MAIN);						// Write main menu
		read_choice(command);					// Get user input
	}

	return 0;
}

// ****************************************************************************
//		PROGRAM FUNCTIONS
// ****************************************************************************

// ----------------------------------------------------------------------------
//		READ USER CHOICE INPUT
// ----------------------------------------------------------------------------
void read_choice(char &ch)
{
	cout << "\nINPUT> ";		// Asks for input
	cin >> ch;					// Reads input
	ch = toupper(ch);			// Sets to uppercase
}

// ----------------------------------------------------------------------------
//		WRITE MENU
// ----------------------------------------------------------------------------
void write_menu(Menu t)
{
	if (t == MAIN)				// Displays the main menu
	{
		cout << "\n\tSTEGA " << VERSION << "\n\n"
			 << "\tE - Encrypt\n"
			 << "\tD - Decrypt\n"
			 << "\tI - Information\n"
			 << "\tQ - Quit\n";
	}
	else if (t == ENC)			// Displays the encryption menu
	{
		cout << "\n\tENCRYPT->\n\n"
			 << "\tL - Load image\n"
			 << "\tU - Unload image\n"
			 << "\tI - Show image info\n"
			 << "\tA - Add data to image\n"
			 << "\tS - Save image data\n"
			 << "\tQ - Back to main\n";
	}
	else if (t == DEC)			// Displays the decryption menu
	{
		cout << "\n\tDECRYPT->\n\n"
			 << "\tL - Load image\n"
			 << "\tU - Unload image\n"
			 << "\tI - Show image info\n"
			 << "\tE - Extract data\n"
			 << "\tQ - Back to main\n";
	}
}

// ----------------------------------------------------------------------------
//		SUBMENUS
// ----------------------------------------------------------------------------
void sub_menu(Menu t)
{
	if (t == ENC || t == DEC)					// Submenu should be ENC or DEC
	{
		char command;							// Buffer for user menu choice
	
		Image *img;
	
		Encrypt enc;							// An instance of Encryption
		Decrypt dec;							// An instance of decryption

		if		(t == ENC)  img = &enc;			// Sets img to encryption
		else if (t == DEC)	img = &dec;			// Sets img to decryption

		write_menu(t);							// Write the menu
		read_choice(command);					// Read menu choice


		while (command != 'Q')					// As long as not Q(uit)
		{
			switch(command)
			{
				case 'L':
					img->fLoad_image();			// Load an image file
					break;
				case 'U':
					img->fUnload_image();		// Unload an image file
					break;
				case 'I':
					img->fShow_image_info();	// Show information about image
					break;
				case 'A':
					if (t == ENC) img->cyrpt();	// Add data to image
					else cout << "\nERROR: INVALID INPUT\n";
					break;
				case 'E':
					if (t == DEC) img->cyrpt();	// Extract data from image
					else cout << "\nERROR: INVALID INPUT\n";
					break;
				case 'S':
					img->fWrite_image_data();	// Write modified image data
					break;
				default:
					cout << "\nERROR: INVALID INPUT\n"; break;
			}
			write_menu(t);
			read_choice(command);
			if (command == 'Q' && t == ENC) img->crypt_created();
		}
	}
	else
	{
		cout << "\nERROR: INVALID SUBMENU\n";
	}
}

// ----------------------------------------------------------------------------
//		SHOW INFORMATION ABOUT THE PROGRAM
// ----------------------------------------------------------------------------
void information()
{
	cout << "\nStega " << VERSION << "\n\n"
		 << "Made by:\tAndreas Moe\n"
		 << "Email:\t\tandreas.moe@hig.no\n"
		 << "Lisence:\tMIT\n\n";

	cout << "This is a small program written in C++, which you\n"
		 << "can use to hide files, text, or other data, inside\n"
		 << "a Bitmap picture file (bmp). For further information\n"
		 << "please contact the programmer, or read the README.\n";
}