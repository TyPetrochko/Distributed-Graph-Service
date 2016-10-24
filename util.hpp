#include <stdint.h>

// Magic number used in checksum
#define MAGIC_NUMBER 12345

// Super helpful for debugging
#define DBG true
#define DEBUG(MSG) { \
  cout << __FILE__ << ":" << __LINE__ << " " << MSG << endl; \
}
#define DIE(MSG) { \
  cout << __FILE__ << ":" << __LINE__ << " " << MSG << endl; \
  exit(1); \
}


// File descriptor
extern int fildes;

// Check a block's checksum
bool checksum(unsigned int block);

// Get a block's checksum (excluding first uint64)
uint64_t get_checksum (void *block_data);

// Return a mmapped block
void *get_block(unsigned int block);

// Write a block to disk at a certain index. Does not free the mmapped block!
bool write_block(void *block_data, unsigned int index);

// Free an mmapped block
void free_block(void *block);

// Redo a log entry operation
void redo_operation(log_entry entry);

// Log an op (only do this if VERBOSE == true)
void logop(log_entry entry);

// Log some ops using persistence.cpp
bool log_add_node(uint64_t node);
bool log_add_edge(uint64_t node_a, uint64_t node_b);
bool log_remove_node(uint64_t node);
bool log_remove_edge(uint64_t node_a, uint64_t node_b);

