#include <cstdint>
#include <string>

#define BLOCK_SIZE 4096
#define LOG_ENTRIES_PER_BLOCK 170

// In number of blocks:
#define MAX_LOG 524287
#define MAX_CHECKPOINT 2621440
#define LOG_SIZE 10
#define CHECKPOINT_SIZE 100

#define VERBOSE true

using namespace std;

// Represents a completed operation. Do not log if fails.
typedef struct log_entry {
  uint32_t opcode; /* 0-3 maps to add_node, add_edge, rem_node, rem_edge */
  uint64_t node_a;
  uint64_t node_b;
} log_entry;

// Call when starting up. Formats the disk, generation number etc. Return true
// if successful (can only fail if format is false).
bool init(string dev_file, bool format);

// Logs a COMPLETED operation. Do not log failed operations.
void log(log_entry op);

// Checkpoint the graph, clear the log, return true if successful.
bool checkpoint();

// Run tests...
void test();
