//----------------------------------------------------------------------------+
//  FILE INFORMATION														  |
//----------------------------------------------------------------------------+
//  AUTHOR:		Andreas Moe (andreas.moe@hig.no)							  |
//  PROGRAM:	Stega														  |
//  FILE:		Constants.h													  |
//----------------------------------------------------------------------------+

#ifndef CONSTANTS_H
#define CONSTANTS_H

// ----------------------------------------------------------------------------
//		CONSTANT SIZES
// ----------------------------------------------------------------------------
const int   WORD = 2;						// Two  Bytes
const int  DWORD = 4;						// Four Bytes
const int STRLEN = 40;						// Max 40 chars for cstrings
const int   MPAD = 10;						// Size of header for inserted data

// THIS IS INFORMATION VALUBALE ONLY TO THE BUILDUP OF THE INSERTED DATA HEADER
const int      HEADER_MOE_SIZE = 3;			// 3 Bytes for "MOE"
const int HEADER_FILETYPE_SIZE = 3;			// 3 Bytes for filetype (txt, etc)
const int     HEADER_FILE_SIZE = 4;			// 4 Bytes for file size (int)

const short  NUM_ERRORS = 14;				// The number of errors that exist
const short    NUM_MSGS =  8;				// Number of messages that exist

// ----------------------------------------------------------------------------
//		CONSTANT CSTRINGS
// ----------------------------------------------------------------------------
const char ERROR[NUM_ERRORS][STRLEN] = {"IMAGE NOT VALID",
										"CANNOT SHOW IMAGE INFO",
										"INVALID INPUT",
										"DATA NOT SAVED",
										"COULD NOT OPEN FILE",
										"IMAGE DATA NOT LOADED",
										"NO ACTION PERFORMED ON DATA",
										"ADDING DATA FAILED",
										"IMAGE ALLREADY LOADED",
										"FILE TO BIG",
										"EXTRACTING DATA FAILED",
										"IMAGE DOES NOT CONTAIN DATA",
										"COULD NOT WRITE EXTRACTED DATA",
										"DATA ALLREADY ADDED"};

const char MSG[NUM_MSGS][STRLEN] =	   {"LOADING IMAGE",
										"ACTION ABORTED",
										"WRITING DATA TO OUTPUT.BMP",
										"EXTRACTING DATA FROM IMAGE",
										"ADDING DATA TO IMAGE",
										"WRITING EXTRACTED DATA TO FILE",
										"EXTRACTED DATA NOT WRITTEN",
										"IMAGE DATA UNLOADED"};

// ----------------------------------------------------------------------------
//		ERROR VALUES
// ----------------------------------------------------------------------------
const short ERR_0x0 = 0x0;				// IMAGE NOT VALID
const short ERR_0x1 = 0x1;				// CANNOT SHOW THE IMAGE INFO
const short ERR_0x2 = 0x2;				// USER GAVE INVALID INPUT
const short ERR_0x3 = 0x3;				// IMAGE DATA NOT SAVED
const short ERR_0x4 = 0x4;				// COULD NOT OPEN FILE
const short ERR_0x5 = 0x5;				// IMAGE DATA NOT LOADED
const short ERR_0x6 = 0x6;				// NO ACTION PERFORMED ON DATA
const short ERR_0x7 = 0x7;				// ADDING DATA FAILED
const short ERR_0x8 = 0x8;				// IMAGE ALLREADY LOADED
const short ERR_0x9 = 0x9;				// FILE TO BIG
const short ERR_0xA = 0xA;				// EXTRACTING DATA FAILED
const short ERR_0xB = 0xB;				// IMAGE DOES NOT CONTAIN DATA
const short ERR_0xC = 0xC;				// COULD NOT WRITE EXTRACTED DATA
const short ERR_0xD = 0xD;				// DATA ALLREADY ADDED

// ----------------------------------------------------------------------------
//		MESSAGE VALUES
// ----------------------------------------------------------------------------
const short MSG_0x0 = 0x0;				// LOADING IMAGE
const short MSG_0x1 = 0x1;				// ACTION ABORTED
const short MSG_0x2 = 0x2;				// WRITING DATA TO OUTPUT.BMP
const short MSG_0x3 = 0x3;				// EXTRACTING DATA FROM IMAGE
const short MSG_0x4 = 0x4;				// ADDING DATA TO IMAGE
const short MSG_0x5 = 0x5;				// WRITING EXTRACTED DATA TO FILE
const short MSG_0x6 = 0x6;				// EXTRACTED DATA NOT WRITTEN
const short MSG_0x7 = 0x7;				// IMAGE DATA UNLOADED


// ----------------------------------------------------------------------------
//		MISC. CONSTANTS
// ----------------------------------------------------------------------------
const enum STATE {START, DONE, INFO};	// States of action-messages

const enum TO_ALLOCATE {MSG_DATA,
						ENCAPSULATED_STRUCT,
						ALL_DYNAMIC_DATA,
						IMAGE_STRUCTS};

const char VERSION[STRLEN] = "v0.2-12";	// Version number

// All these constants with a W_ in from of them, indicate that they hold
// a value that is WANTED. We want the bitspp to be 24 in our loaded
// image, så we check if against this W_BITSPP, and so on.
const int W_MAGIC_NUMBER = 19778;		// The "Magic number" (BM)
const int W_MIN_BMP_SIZE = 100;			// Minimum size of data
const int W_BITSPP = 24;				// Bits per pixel
const int W_DATA_OFFSET = 54;			// Were data should start
const int W_MIN_MSG_SIZE = 10;			// Minimum input size
const int W_COMPRESSION = 0;			// NO compression
const int W_NCOLORS = 0;				// NO "number of colors"
const int W_NIMPCOLORS = 0;				// NO non-important colors


const int LIMIT_FOR_SCREENPRINT = 500;	// Limit for asking if screen dump
										// is still wanted

const int LIMIT_FOR_FILEPRINT   = 5000;	// Limit for forceing file dump
										// instead of screen dump
#endif