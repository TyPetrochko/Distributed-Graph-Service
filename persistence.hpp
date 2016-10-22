#include <cstdint>
#include <string>

using namespace std;

// Represents a completed operation. Do not log if fails.
typedef struct operation {
  enum type {add_node, remove_node, add_edge, remove_edge};
  uint64_t node_a;
  uint64_t node_b;
} operation;

// Call when starting up. Formats the disk, generation number etc. Return true
// if successful (can only fail if format is false).
bool init(string file, bool format);

// Logs a COMPLETED operation. Do not log failed operations.
void log(operation op);

// Checkpoint the graph, clear the log, return true if successful.
bool checkpoint();
