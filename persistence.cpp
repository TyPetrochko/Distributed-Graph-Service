#include "persistence.hpp"
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// Super helpful for debugging
#define DBG true
#define DEBUG(MSG) (cout << __FILE__ << ":" << __LINE__ << " " << MSG\
    << endl)
#define DIE(MSG) {cout << __FILE__ << ":" << __LINE__ << " " << MSG << endl; \
  exit(1);}

#define MAGIC_NUMBER 12345
#define BLOCK_SIZE 4096

// In number of blocks:
#define MAX_LOG 524287
#define MAX_CHECKPOINT 2621440
#define LOG_SIZE 10
#define CHECKPOINT_SIZE 100

#define VERBOSE true

using namespace std;

static int fildes;

// Check a block's checksum
bool checksum(unsigned int block);

// Format a disk
void format_disk();

// Restore the graph from disk
void restore_graph();

// Struct representing a log entry (to be serialized)
typedef struct log_entry {
  uint32_t opcode;
  uint64_t node_a;
  uint64_t node_b;
} log_entry;

// Struct representing the superblock
typedef struct s_block {
  uint64_t checksum;
  uint32_t current_generation;
  uint32_t log_start;
  uint32_t log_size; /* in blocks */
  char fluff[4076];
} s_block;

// Struct representing a log block
typedef struct l_block {
  uint64_t checksum;
  uint32_t generation_number;
  uint32_t num_entries;
  log_entry entries[204];
} l_block;


// Check if a given block (at index 'block') is correctly formatted
bool checksum(unsigned int block){
  uint64_t *block_data = (uint64_t *) mmap(NULL, BLOCK_SIZE,
      PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if(block_data == MAP_FAILED)
    DIE("Couldn't mmap");

  // copy into local buffer
  pread(fildes, block_data, BLOCK_SIZE / 8, BLOCK_SIZE * block);

  uint64_t chksm = MAGIC_NUMBER;
  for(int i = 0; i < (BLOCK_SIZE / 8); i++){
     chksm ^= block_data[i];
  }

  if(munmap(block_data, BLOCK_SIZE) == -1)
    DIE("Couldn't unmap");

  return chksm == 0;
}


// Call when starting up. Formats the disk, generation number etc. Return true
// if successful (can only fail if format is false).
bool init(string dev_file, bool format){
  if(VERBOSE){
    cout << "Initializing disk... " << endl;
    cout << "\tLog (blocks): " << LOG_SIZE << endl;
    cout << "\tCheckpoint (blocks): " << CHECKPOINT_SIZE << endl;
    cout << "\tDev file: " << dev_file << endl;
    cout << "\tFormat?: " << format << endl;
    cout << "\tPage size: " << sysconf(_SC_PAGE_SIZE) << endl;
  }

  // open the dev file
  fildes = open(dev_file.c_str(), O_RDWR | O_DIRECT);
  if(fildes < 0)
    DIE("Could not open " + dev_file);
  
  if(checksum(0)){
    if (VERBOSE) cout << "Block zero formatted correctly!" << endl;

    restore_graph();
  }else{
    if (VERBOSE) cout << "Block zero not formatted correctly!" << endl;
    if (!format)
      DIE("Dev file not formatted and no -f flag! Aborting...");

    format_disk();

    if(!checksum(0))
      DIE("Could not format superblock!");
  }
  return false;
}

void format_disk(){
  s_block *super = (s_block*) mmap(NULL, BLOCK_SIZE, PROT_READ | PROT_WRITE,
      MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  
  if(super == MAP_FAILED)
    DIE("Mmap failed");

  super->current_generation = 0;
  super->log_start = 1;
  super->log_size = 0;

  uint64_t *block_data = (uint64_t*) super;
  uint64_t chksm = MAGIC_NUMBER;
  for(int i = 1; i < (BLOCK_SIZE / 8); i++){
     chksm ^= block_data[i];
  }

  super->checksum = chksm;

  if(pwrite(fildes, super, BLOCK_SIZE, 0) < BLOCK_SIZE)
    DIE("Couldn't write to block zero");
  
  if(munmap(super, BLOCK_SIZE) == -1)
    DIE("Couldn't unmap");
}

void restore_graph(){
  // TODO
}

void log(operation op){
  // TODO
}

