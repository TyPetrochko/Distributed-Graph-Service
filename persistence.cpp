#include "persistence.hpp"
#include <string>

#define BLOCK_SIZE 4096
#define GB 1073741824

// In number of blocks:
#define MAX_LOG (((2 * GB)/BLOCK_SIZE) - 1)
#define MAX_CHECKPOINT (((10 * GB)/BLOCK_SIZE))
#define LOG_SIZE 10
#define CHECKPOINT_SIZE 100

using namespace std;

bool checksum(unsigned int block){
  // TODO
  return false;
}

bool init(string file, bool format){
  // TODO
  return false;
}

void log(operation op){
  // TODO
}

