#include "persistence.hpp"
#include "util.hpp"
#include "memorygraph.hpp"
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

// Check if a given block (at index 'block') is correctly formatted
bool checksum(unsigned int block){
  uint64_t *block_data = (uint64_t *) get_block(block);

  // copy into local buffer
  if(pread(fildes, block_data, BLOCK_SIZE, BLOCK_SIZE * block) == -1)
    DIE("Pread failed!" << strerror(errno));

  uint64_t chksm = block_data[0] ^ get_checksum(block_data);

  if(munmap(block_data, BLOCK_SIZE) == -1)
    DIE("Couldn't unmap");

  return chksm == 0;
}

// Get a block's checksum, excluding first uint64_t
uint64_t get_checksum(void *block_data){
  uint64_t *data = (uint64_t *) block_data;

  uint64_t chksm = MAGIC_NUMBER;
  for(int i = 1; i < (BLOCK_SIZE / 8); i++){
    chksm ^= data[i];
  }

  return chksm;
}

// Return an mmapped representation of block 'block'. Remember to free later!
void *get_block(unsigned int block){
  void *block_data = (void *) mmap(NULL, BLOCK_SIZE,
      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if(block_data == MAP_FAILED)
    DIE("Couldn't mmap block " + block);

  // copy into local buffer
  pread(fildes, block_data, BLOCK_SIZE, BLOCK_SIZE * block);
  
  return block_data;
}

// Write a block to disk at a certain index. Does not free the mmapped block!
bool write_block(void *block_data, unsigned int index){
  if(pwrite(fildes, block_data, BLOCK_SIZE, BLOCK_SIZE * index) < BLOCK_SIZE)
    return false;

  return true;
}

// Free a mmapped block
void free_block(void *block){
  if(munmap(block, BLOCK_SIZE) == -1)
    DIE("Couldn't unmap block");
}

// Redo a log entry operation
void redo_operation(log_entry entry){
  switch(entry.opcode){
    case 0: /* add node */
      add_node(entry.node_a);
      break;
    case 1: /* add edge */
      add_edge(entry.node_a, entry.node_b);
      break;
    case 2: /* remove node */
      remove_node(entry.node_a);
      break;
    case 3: /* remove edge */
      remove_edge(entry.node_a, entry.node_b);
      break;
    default:
      DIE("Not a valid opcode: " << entry.opcode);
  }

  if(VERBOSE)
    logop(entry);
}

// Debug print an entry
void logop(log_entry entry){
  switch(entry.opcode){
    case 0:
      DEBUG("Adding node " << entry.node_a);
      break;
    case 1:
      DEBUG("Adding edge " << entry.node_a << ", " << entry.node_b);
      break;
    case 2:
      DEBUG("Removing node " << entry.node_a);
      break;
    case 3:
      DEBUG("Removing edge " << entry.node_a << ", " << entry.node_b);
      break;
    default:
      DIE("Not a valid opcode: " << entry.opcode);
  }
}

void log_add_node(uint64_t node){
  log_entry le;

  le.opcode = 0;
  le.node_a = node;

  log(le);
}

void log_add_edge(uint64_t node_a, uint64_t node_b){
  log_entry le;

  le.opcode = 1;
  le.node_a = node_a;
  le.node_b = node_b;

  log(le);
}

void log_remove_node(uint64_t node){
  log_entry le;

  le.opcode = 2;
  le.node_a = node;

  log(le);
}

void log_remove_edge(uint64_t node_a, uint64_t node_b){
  log_entry le;

  le.opcode = 3;
  le.node_a = node_a;
  le.node_b = node_b;

  log(le);
}

