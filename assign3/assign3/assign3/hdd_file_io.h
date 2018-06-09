#ifndef HDD_FILE_IO_INCLUDED
#define HDD_FILE_IO_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : hdd_file_io.h
//  Description    : This is the header file for the standardized IO functions
//                   for used to access the HDD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Tue Sep 16 19:38:42 EDT 2014
//

// Include files
#include <stdint.h>


//
// Management operations
uint16_t hdd_format(void);
	// This function formats the HDD drive, and saves the global data structure.

uint16_t hdd_mount(void);
	// This function mount the current HDD file system and populates values in the global data structure.

uint16_t hdd_unmount(void);
	// This function unmounts the current HDD file system and saves the global data structure.

//
// Interface functions
int16_t hdd_open(char *path);
	// This function opens the file and returns a file handle

int16_t hdd_close(int16_t fd);
	// This function closes the file

int32_t hdd_read(int16_t fd, void *buf, int32_t count);
	// Reads "count" bytes from the file handle "fh" into the buffer  "buf"

int32_t hdd_write(int16_t fd, void *buf, int32_t count);
	// Writes "count" bytes to the file handle "fh" from the buffer  "buf"

int32_t hdd_seek(int16_t fd, uint32_t loc);
	// Seek to specific point in the file

//
// Unit testing for the module
int hddIOUnitTest(void);
	// Perform a test of the CRUD IO implementation

#endif


