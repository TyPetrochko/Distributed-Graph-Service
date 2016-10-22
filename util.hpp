#include <stdint.h>

// Magic number used in checksum
#define MAGIC_NUMBER 12345

// Super helpful for debugging
#define DBG true
#define DEBUG(MSG) (cout << __FILE__ << ":" << __LINE__ << " " << MSG\
    << endl)
#define DIE(MSG) {cout << __FILE__ << ":" << __LINE__ << " " << MSG << endl; \
  exit(1);}


// File descriptor
extern int fildes;

// Check a block's checksum
bool checksum(unsigned int block);

// Get a block's checksum (excluding first uint64)
uint64_t get_checksum (void *block_data);

// Return a mmapped block
void *get_block(unsigned int block);

// Free an mmapped block
void free_block(void *block);

// Redo a log entry operation
void redo_operation(log_entry entry);

// Log an op (only do this if VERBOSE == true)
void logop(log_entry entry);

