////////////////////////////////////////////////////////////////////////////////
//
//  File           : hdd_file_io.c
//  Description    : 
//
//  Author         : Chuyang Zhang
//  Last Modified  : 2017/11/15
//

// Includes
#include <malloc.h>
#include <string.h>

// Project Includes
#include <hdd_file_io.h>
#include <hdd_driver.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>

// Defines
#define MAX_HDD_FILEDESCR 1024
#define MAX_FILENAME_LENGTH 128
#define CIO_UNIT_TEST_MAX_WRITE_SIZE 1024
#define HDD_IO_UNIT_TEST_ITERATIONS 10240


// Type for UNIT test interface
typedef enum {
	CIO_UNIT_TEST_READ   = 0,
	CIO_UNIT_TEST_WRITE  = 1,
	CIO_UNIT_TEST_APPEND = 2,
	CIO_UNIT_TEST_SEEK   = 3,
} HDD_UNIT_TEST_TYPE;

char *cio_utest_buffer = NULL;  // Unit test buffer

struct HDD_FILE{
	char path[MAX_FILENAME_LENGTH];
        int16_t fh;             //file handle
        uint32_t blockID;       //blockID
        uint32_t cp; // current position
};
struct HDD_FILE file;

struct _fileName{		//global varibale for file
	struct HDD_FILE file[MAX_HDD_FILEDESCR];
	int init;
};
struct _fileName fileName;


//Set funtions for Imeplementation

////////////////////////////////////////////////////////////////////////////////
void setMetaData(){		//set the meta data and make the meta block
	int i = 0;
	printf("Enter for setMetaData\n");
	while( i < MAX_HDD_FILEDESCR){	//loop to initialize all the basic info
		memset(fileName.file[i].path, '\0', sizeof(fileName.file[i].path));
		//fileName.file[i].path = NULL;		
		fileName.file[i].fh = -1;
		fileName.file[i].blockID = 0;
		fileName.file[i].cp = 0;
		++i;
	}
	printf("set metadata successfully\n");
}
////////////////////////////////////////////////////////////////////////////////
HddBitCmd setCmd(uint32_t blockSize, uint8_t Flags, uint8_t R, uint32_t blockID, uint8_t op){	//set the commmand as helper function so that it can be used and called easily
	printf("Enter set command blockSize: %d, flag %d, R: %d, blockID: %d, op: %d",blockSize, Flags, R, blockID, op);
	HddBitCmd command;
	memset(&command, 0, sizeof(HddBitCmd));	
	command = 0;
	command =  (op << 26);
	command = (command | blockSize) << 3;
	command = (command | Flags) << 1;
	command = (command | R) << 32;
	command = command | blockID;
	return command;
}

///////////////////////////////////////////////////////////////////////////////
int checkOpened(int16_t fh){	//check the file
	int i = 0;
	printf("Enter to check the file open status\n");
	while (fileName.file[i].fh != -1){
		if (fileName.file[i].fh == fh){		//when the file has the corrcet file handler
			break;
		}
		++i;
	}
	if (fileName.file[i].fh != -1){		//when the file is opened
		return i;
		printf("Open checked\n");
	}		
	else if (fileName.file[i].fh == -1){     //when the file is not yet opened
        	printf("file is not opened yet\n");
        	return -1;
	}
}

////////////////////////////////////////////////////////////////////////////////
int checkStatus(){		//check the file status
	printf("Enter checkStatus\n");
	if (fileName.init == 0){	
		if (hdd_initialize() == -1){	//when the file is not initialized
			printf("Not yet initialized\n");
			return -1;
		}
		fileName.init = 1;
       		setMetaData();	
		printf("Check Successfully1\n");	//for debug
		return 0;
	}
	printf("Check Successfully\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

//	
// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_format(void)
// Description  : sends a format request to the device delete all blocks
//
// Inputs       : void
// Outputs      : return 0 on success and -1 on failure
//
uint16_t hdd_format(void) {
	uint32_t blockID;
	HddBitCmd command;
	HddBitResp response;
	printf("Enter hdd_format\n");
	char* buffer = malloc(HDD_MAX_BLOCK_SIZE);	//create the buffer
	if (checkStatus() == -1)	return -1;	//check the status
	else{
		command = 0;
		blockID = 0;
		printf("Process1\n");
		command = setCmd(0x0, HDD_FORMAT, 0x0, 0x0, HDD_DEVICE);	
		hdd_data_lane(command, NULL);
		command = setCmd(HDD_MAX_BLOCK_SIZE, HDD_META_BLOCK, 0, blockID, HDD_BLOCK_CREATE);
		memcpy(buffer, &fileName, sizeof(fileName));
		response = hdd_data_lane(command, buffer);
		printf("format successfully\n");
		return 0;
	}	
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_mount(void) 
// Description  : read from the meta block to populate global data.
//
// Inputs       : void
// Outputs      : return 0 on success and -1 on failure
//
uint16_t hdd_mount(void) {
	HddBitCmd command;
	HddBitResp response;
	uint32_t blockID, blockSize;
	printf("Enter hdd_mount\n");	//for debug
	if (checkStatus() == -1)	return -1;
	
	else{
		command= 0;
	        printf("Process3\n");
        	char* buffer = malloc(HDD_MAX_BLOCK_SIZE);
	        printf("Process4\n");
        	blockID = 0;
	        blockSize = hdd_read_block_size(blockID, HDD_META_BLOCK);
        	command = setCmd(blockSize, HDD_META_BLOCK, 0X0, blockID, HDD_BLOCK_READ);
	        response = hdd_data_lane(command, buffer);
        	memcpy(&fileName, buffer, sizeof(fileName));	//copy the buffer to the file name
        	free(buffer);		//free the buffer
        	printf("mount successfully\n");
        	return 0;
	}
	
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_unmount(void)
// Description  : read from the meta block to populate your global data.
//
// Inputs       : void
// Outputs      : return 0 on success and -1 on failure
//
uint16_t hdd_unmount(void) {
	HddBitCmd command;
	HddBitResp response;
	uint32_t blockID, blockSize;
	printf("Enter hdd_unmount\n");
	if (checkStatus() == -1)	return -1;
	else{
		command = 0;			//when the check status is correct
		blockID = 0;
		char* buffer = malloc(HDD_MAX_BLOCK_SIZE);
		printf("Progress6\n");
		blockSize = hdd_read_block_size(blockID, HDD_META_BLOCK);
		printf("Progress7\n");
		command = setCmd(blockSize, HDD_META_BLOCK, 0, blockID, HDD_BLOCK_OVERWRITE);
		printf("Progress8\n");
		memcpy(buffer, &fileName, sizeof(fileName));
		response = hdd_data_lane(command, buffer);
		command = setCmd(0x0, HDD_SAVE_AND_CLOSE, 0, 0X0, HDD_DEVICE);
		hdd_data_lane(command, NULL);
		free(buffer);	//free the buffer
		return 0;
	}
}
////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_open(char*)
// Description  : hdd_open will open a file and return an integer file handle.
//                and it return -1 on failure and integer on sucess.
//
// Inputs       : path  - the given filename
// Outputs      : fileHandle    - the unique integer that needed to be returned.
//
int16_t hdd_open(char *path) {
	int i = 0;
	if (checkStatus() == -1)	return -1;
	while(fileName.file[i].fh != -1){
		if (strcmp(fileName.file[i].path, path) == 0){
			if (fileName.file[i].fh == -99){
				fileName.file[i].fh = i;
				return i;
			}
			printf("file is already opened\n");
			return fileName.file[i].fh;
		}
		++i;
	}

        memset(&fileName.file[i].blockID, 0, sizeof(int16_t));      //memory set the 64bits command 
        if (path == NULL)	return -1;             //case that no file name
        //fileName.file[i].path = malloc(strlen(path));
	else{
		strcpy(fileName.file[i].path, path);
		fileName.file[i].fh = i;
        	printf("Open Sucess\n");                //for debug
       		return fileName.file[i].fh;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_close(int16_t)
// Description  : close the file reference by the file handle
//
// Inputs       : fh    -file handle
// Outputs      : -0 sucess     --1 failure
//

int16_t hdd_close(int16_t fh) {
	int i = 0;
	while (fileName.file[i].fh != -1){
        	//if (fh != fileName.file[i].fh){             //case the file handle is incorrect
                	//printf("file is not correctly opened\n");
                	//return -1;
        //}
        	//else 
		if (fh == fileName.file[i].fh)	break;
		++i;
	}

	if (fileName.file[i].fh == -1){
		printf("file is not correctly opened\n");
		return -1;
	}
        else{
		fileName.file[i].fh = -99;           //set file handle, current position and blockID
        	fileName.file[i].cp = 0;
		printf("File Close sucessfully\n");     //for debug   
		return 0;
	}
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_read(int16_t, void *, int32_t)
// Description  : read a count number and places them into buffer
//
// Inputs       : fh    -file handle    data    -the file content that needed to put in
//                count -count number of bytes from the current position
// Outputs      : --1 failure   -number of bytes read sucess
//
int32_t hdd_read(int16_t fh, void * data, int32_t count) {
        uint32_t blockSize, mid;        char *data1;
        int R;
	int i = checkOpened(fh);		
	if (i == -1)	return -1;
	blockSize = hdd_read_block_size(fileName.file[i].blockID, 0x0);
	
        HddBitCmd command;
        HddBitResp response;
        printf("Enter read\n");         //for debug
        memset(&command, 0, sizeof(HddBitCmd));         //memory set
	printf("Process5\n");        
	memset(&response, 0, sizeof(HddBitResp));
        printf("Process6\n");
	blockSize = hdd_read_block_size(fileName.file[i].blockID, 0x0);  //set the blocksize
        if (fh != fileName.file[i].fh || fileName.file[i].blockID == 0){        //reject the case that file handle is not correct and block is empty
                printf("The file is not correctly opened\n");
                return -1;
        }
        if (fileName.file[i].cp + count > blockSize){
                command = setCmd(blockSize, 0x0, 0x0, fileName.file[i].blockID, 0x1);
                data1 = (char *) malloc(blockSize);             //allocate space for data1
                //printf("blocksize: %d\n", blockSize);           //for debug
                //printf("cp: %d\n", fileName.file[checkOpened(fh)].cp);
                response = hdd_data_lane(command, data1);       //response
                mid = blockSize - fileName.file[i].cp;
                memcpy(data, &data1[fileName.file[i].cp], mid);             //memory copy the data1 to data
                fileName.file[i].cp = blockSize;
		free(data1);            //free data1

                return mid;             // return the count
        }
        command = setCmd(blockSize, 0x0, 0x0, fileName.file[i].blockID, 0x1);
        data1 = (char *) malloc(blockSize);
        response = hdd_data_lane(command, data1);
        memcpy(data, &data1[fileName.file[i].cp], count);
        fileName.file[i].cp += count;               //adding the current position
        R = response >> 32 & 0x01;
        if (R == 1)     return -1;      //reject the case that R = 1
        free(data1);            //free data1
        return count;


}



////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_write(int16_t, void *, int 32_t)
// Description  : write a count number of bytes at the current position, and create new blocks when number of bytes are exceeded.
//
// Inputs       : fh    -file handle    data    - the file content that needed to put in
//                count -count number of bytes from the current position
// Outputs      : --1 if failure -number of written read if sucess
//
int32_t hdd_write(int16_t fh, void *data, int32_t count) {
        HddBitCmd command;
        HddBitResp response;
        char *data1;
        printf("Enter write\n");        // for debug
        memset(&command, 0, sizeof(HddBitCmd));                 //memory set
        memset(&response, 0, sizeof(HddBitResp));
	int i = checkOpened(fh);
	if (i < 0)	return -1;
        if (fileName.file[i].blockID == 0){            //the case that blockID is empty
                command = setCmd(count, 0x0, 0x0, 0x0, 0x0);
                response = hdd_data_lane(command, data);
		fileName.file[i].blockID = (uint64_t) response & 0xFFFFFFFF;     // set to 64 bits
                fileName.file[i].cp = count;
                return count;   //return the count number
        }
        else{
                uint32_t bs = hdd_read_block_size(fileName.file[i].blockID, 0x0);        //set the block size
                if (fileName.file[i].cp + count <= bs){             // the case that current position add count is less than the block size that have been created
                        data1 = (char *) malloc(bs);    // allocate the data
			command = setCmd(bs, 0x0, 0x0, fileName.file[i].blockID, 0x1);
                        response = hdd_data_lane(command, data1);
                        command = 0;            //reset to 0
                        command = setCmd(bs, 0x0, 0x0, fileName.file[i].blockID, 0x2);
                        memcpy(&data1[fileName.file[i].cp], data, count);   //memcpy the data1 to data
                        response = hdd_data_lane(command, data1);
                        fileName.file[i].cp+= count;
                        free(data1);    //free the data1
                        printf("Correctly write when the size is correct\n");   // for debug
                        return count;
                }
                else{           //almost doing the same thing as the if statement, modify because the blocks are too small for the number of the bytes
                        data1 = (char *) malloc(fileName.file[i].cp + count);               
                        command = setCmd(bs, 0x0, 0x0, fileName.file[i].blockID, 0x1);
                        response = hdd_data_lane(command, data1);
                        command = 0;
                        command = setCmd(fileName.file[i].cp + count, 0x0, 0x0, 0x0, 0x0);
                        memcpy(&data1[fileName.file[i].cp], data, count);
                        response = hdd_data_lane(command, data1);
                        hdd_delete_block(fileName.file[i].blockID);
                        fileName.file[i].blockID = (uint64_t) response;
                        fileName.file[i].cp += count;
                        free(data1);
                        printf("Correctly write when the size need to be expanded\n");
                        return count;
                }
	    
        }
        return -1;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_seek(int16_t, uint32_t)
// Description  : changes the current seek position of the file
//
// Inputs       : fh    -file handle    loc     -the location that it wants
// Outputs      : --1 on failure and 0 on sucess
//

int32_t hdd_seek(int16_t fh, uint32_t loc) {
        printf("Enter seek\n");         // for debug

	if (checkOpened(fh) < 0)	return -1;

        if(loc > hdd_read_block_size(fileName.file[checkOpened(fh)].blockID, 0x0))     return -1;      //reject the case that location exceeds the maximum size of block
        fileName.file[checkOpened(fh)].cp = loc;
        printf("Seek\n");       // for debug
        return 0;
}




////////////////////////////////////////////////////////////////////////////////
//
// Function     : hddIOUnitTest
// Description  : Perform a test of the HDD IO implementation
//
// Inputs       : None
// Outputs      : 0 if successful or -1 if failure
int hddIOUnitTest(void) {
	// Local variables
	uint8_t ch;
	int16_t fh, i;
	int32_t cio_utest_length, cio_utest_position, count, bytes, expected;
	char *cio_utest_buffer, *tbuf;
	HDD_UNIT_TEST_TYPE cmd;
	char lstr[1024];

	// Setup some operating buffers, zero out the mirrored file contents
	cio_utest_buffer = malloc(HDD_MAX_BLOCK_SIZE);
	tbuf = malloc(HDD_MAX_BLOCK_SIZE);
	memset(cio_utest_buffer, 0x0, HDD_MAX_BLOCK_SIZE);
	cio_utest_length = 0;
	cio_utest_position = 0;

	// Format and mount the file system
	if (hdd_format() || hdd_mount()) {
		logMessage(LOG_ERROR_LEVEL, "CRUD_IO_UNIT_TEST : Failure on format or mount operation.");
		return(-1);
	}

	// Start by opening a file
	fh = hdd_open("temp_file.txt");
	if (fh == -1) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure open operation.");
		return(-1);
	}

	// Now do a bunch of operations
	for (i=0; i<HDD_IO_UNIT_TEST_ITERATIONS; i++) {

		// Pick a random command
		if (cio_utest_length == 0) {
			cmd = CIO_UNIT_TEST_WRITE;
		} else {
			cmd = getRandomValue(CIO_UNIT_TEST_READ, CIO_UNIT_TEST_SEEK);
		}

		// Execute the command
		switch (cmd) {

		case CIO_UNIT_TEST_READ: // read a random set of data
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : read %d at position %d", bytes, cio_utest_position);
			bytes = hdd_read(fh, tbuf, count);
			if (bytes == -1) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Read failure.");
				return(-1);
			}

			// Compare to what we expected
			if (cio_utest_position+count > cio_utest_length) {
				expected = cio_utest_length-cio_utest_position;
			} else {
				expected = count;
			}
			if (bytes != expected) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : short/long read of [%d!=%d]", bytes, expected);
				return(-1);
			}
			if ( (bytes > 0) && (memcmp(&cio_utest_buffer[cio_utest_position], tbuf, bytes)) ) {

				bufToString((unsigned char *)tbuf, bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST R: %s", lstr);
				bufToString((unsigned char *)&cio_utest_buffer[cio_utest_position], bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST U: %s", lstr);

				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : read data mismatch (%d)", bytes);
				return(-1);
			}
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : read %d match", bytes);


			// update the position pointer
			cio_utest_position += bytes;
			break;

		case CIO_UNIT_TEST_APPEND: // Append data onto the end of the file
			// Create random block, check to make sure that the write is not too large
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			if (cio_utest_length+count >= HDD_MAX_BLOCK_SIZE) {

				// Log, seek to end of file, create random value
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : append of %d bytes [%x]", count, ch);
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : seek to position %d", cio_utest_length);
				if (hdd_seek(fh, cio_utest_length)) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : seek failed [%d].", cio_utest_length);
					return(-1);
				}
				cio_utest_position = cio_utest_length;
				memset(&cio_utest_buffer[cio_utest_position], ch, count);

				// Now write
				bytes = hdd_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes != count) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : append failed [%d].", count);
					return(-1);
				}
				cio_utest_length = cio_utest_position += bytes;
			}
			break;

		case CIO_UNIT_TEST_WRITE: // Write random block to the file
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			// Check to make sure that the write is not too large
			if (cio_utest_length+count < HDD_MAX_BLOCK_SIZE) {
				// Log the write, perform it
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : write of %d bytes [%x]", count, ch);
				memset(&cio_utest_buffer[cio_utest_position], ch, count);
				bytes = hdd_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes!=count) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : write failed [%d].", count);
					return(-1);
				}
				cio_utest_position += bytes;
				if (cio_utest_position > cio_utest_length) {
					cio_utest_length = cio_utest_position;
				}
			}
			break;

		case CIO_UNIT_TEST_SEEK:
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : seek to position %d", count);
			if (hdd_seek(fh, count)) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : seek failed [%d].", count);
				return(-1);
			}
			cio_utest_position = count;
			break;

		default: // This should never happen
			CMPSC_ASSERT0(0, "HDD_IO_UNIT_TEST : illegal test command.");
			break;

		}
	}

	// Close the files and cleanup buffers, assert on failure
	if (hdd_close(fh)) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure read comparison block.", fh);
		return(-1);
	}
	free(cio_utest_buffer);
	free(tbuf);

	// Return successfully
	return(0);
}

































