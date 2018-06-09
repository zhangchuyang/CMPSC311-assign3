#ifndef HDD_DRIVER_INCLUDED
#define HDD_DRIVER_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : hdd_driver.h
//  Description    : This is the header file for the driver implementation
//                   of the HDD storage system.
//
//  Author         : Patrick McDaniel
//

// Includes
#include <stdint.h>

// Defines
#define HDD_MAX_BLOCK_SIZE 0xfffff
#define HDD_NO_BLOCK 0


// These are the op types
typedef enum {
    HDD_BLOCK_CREATE  = 0,     // Create a new block
    HDD_BLOCK_READ    = 1,     // Read an block
    HDD_BLOCK_OVERWRITE  = 2,  // Update the block
    HDD_DEVICE = 3             // Indicates HddBitCmd is directed towards device as a whole, not a block
} HDD_OP_TYPES;

// These are the HDD flags
typedef enum {
    HDD_NULL_FLAG = 0,     // This is the "no flag" flag
    HDD_META_BLOCK = 1,    // Flag indicating that block is the "meta block"
    HDD_FORMAT = 2,        // Flag indicating device should be formatted--used with HDD_DEVICE
    HDD_SAVE_AND_CLOSE = 3 // Flag indicating device info to save in hdd_content.svd and close HDD interface--used with HDD_DEVICE
}   HDD_FLAG_TYPES;

// HDD block ID type (unique to each block)
typedef uint32_t HddBlockID;

// HDD Bit command and response types
typedef uint64_t HddBitCmd;
typedef uint64_t HddBitResp;

/*
 HddBitCmd/HddBitResp Specification

  Bits    Description
  -----   -------------------------------------------------------------
   0-31 - Block - the Block ID (0 if not relevant)
     32 - R - this is the result bit (0 success, 1 is failure)
  33-35 - Flags - these are flags for commands (UNUSED)
  36-61 - Block Size - this is the size of the block in bytes 
  62-63 - Op - the Opcode which controls whether a block is read, overwritten, or created

        6                   5                   4                   3                   2                   1
  3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |Op |                   Block Size                      |Flags|R|                             Block                             |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/


// HDD Interface

// Initalization function to be called once
int32_t hdd_initialize();

// Handles sending/recieving data to the HDD
HddBitResp hdd_data_lane(HddBitCmd command, void * data);

// Retrieves block size (in bytes) of given BID
int32_t hdd_read_block_size(HddBlockID bid, uint8_t flags);

// Deletes block of given BID
int32_t hdd_delete_block(HddBlockID bid);

// Unit testing for the module
int hdd_unit_test( void );


#endif
