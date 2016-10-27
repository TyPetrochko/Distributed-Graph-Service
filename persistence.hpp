#include <cstdint>
#include <string>

#define BLOCK_SIZE 4096
#define NODES_PER_BLOCK 510
#define EDGES_PER_BLOCK 205
#define LOG_ENTRIES_PER_BLOCK 170

// In number of blocks:
#define MAX_LOG 524287
#define MAX_CHECKPOINT 2621440
#define LOG_SIZE MAX_LOG
#define CHECKPOINT_SIZE MAX_CHECKPOINT

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

// Logs a COMPLETED operation. Ideally, don't log failed operations...
// In reality, as long as it ALSO fails on restore_graph() it's ok.
void log(log_entry op);

// Is the log currently full?
bool log_full();

// Checkpoint the graph, set the log tail to 0, and increment the generation number, return true if successful.
bool checkpoint();

// Run tests...
void test();
