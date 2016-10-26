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

using namespace std;

int fildes;
static uint64_t generation;
static unsigned int log_start;
static unsigned int log_size;

// Format the superblock to match above metadata
void format_superblock();

// Format a disk
void format_disk(bool);

// Restore the graph from disk
void restore_graph();

// Create an sblock
void create_sblock();

// Create an lblock
void create_lblock(unsigned int block);

// Initialize log from disk
void initialize_log();

// Load the checkpoint into the current graph
void load_checkpoint();

// Struct representing the superblock
typedef struct s_block {
  uint64_t checksum;
  uint32_t generation;
  uint32_t log_start;
  uint32_t log_size; /* in blocks */
  char fluff[4076];
} s_block;

// Struct representing a log block
typedef struct l_block {
  uint64_t checksum;
  uint32_t generation;
  uint32_t num_entries;
  log_entry entries[LOG_ENTRIES_PER_BLOCK];
} l_block;

// Struct representing a log block
typedef struct edge {
  uint64_t node_a;
  uint64_t node_b;
} edge;

// Call when starting up. Formats the disk, generation number etc. Return true
// if successful (can only fail if format is false).
bool init(string dev_file, bool format) {
  if (VERBOSE) {
    cout << "Initializing disk... " << endl;
    cout << "\tLog (blocks): " << LOG_SIZE << endl;
    cout << "\tCheckpoint (blocks): " << CHECKPOINT_SIZE << endl;
    cout << "\tDev file: " << dev_file << endl;
    cout << "\tFormat?: " << format << endl;
    cout << "\tPage size: " << sysconf(_SC_PAGE_SIZE) << endl;
    cout << "\tLblock size: " << sizeof(l_block) << endl;
    cout << "\tSblock size: " << sizeof(s_block) << endl;
    cout << "\tLog Entry size: " << sizeof(log_entry) << endl;
    print_nodes();
    print_graph();
    cout << "nodes and graph in init start\n";
  }

  // open the dev file
  fildes = open(dev_file.c_str(), O_RDWR | O_DIRECT);
  if(fildes < 0)
    DIE("Could not open " + dev_file);
  
  if (!format) {
    if (!checksum(0))
      DIE("Dev file not formatted and no -f flag! Aborting...");
    initialize_log();
    restore_graph();
  } else {
    if (!checksum(0)) {
      if(VERBOSE) {
        cout << "formatting disk for the first time\n";
      }
      format_disk(false);
    } else {
      if(VERBOSE) {
        cout << "formatting disk not for the first time\n";
      }
      format_disk(true);
    }
  }
  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph in end of init\n";
  }
  
  return false;
}

// Update the superblock on disk to represent in-memory metadata
void format_superblock() {
  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph in format superblock\n";
  }
  s_block *super = (s_block*) get_block(0);

  super->generation = generation;
  super->log_start = log_start;
  super->log_size = log_size;
 
  super->checksum = get_checksum(super);

  if (!write_block(super, 0))
    DIE("Couldn't write to block zero");

  free_block(super);
}

// Format the disk and start the log! If initialized is false it is the first time.
// Otherwise a format is called on a pre-existing memory space.
void format_disk(bool initialized) {
  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph in format disk\n";
  }
  if (initialized) {
    generation++;
    log_start = 1;
    log_size = 1;
    format_superblock();
  } else {
    create_sblock();
  }

  // start log
  create_lblock(1);
}

// Log a graph entry and write block back to disk
void log(log_entry entry) {
  if (VERBOSE) {
    cout << "logop in log\n";
    logop(entry);
  }

  l_block *l = (l_block*) get_block(log_size);

  // last log block full - make another and update superblock!
  if (l->num_entries == LOG_ENTRIES_PER_BLOCK) {
    if(VERBOSE) DEBUG("Making a new block for logs!");
    log_size++;
    format_superblock();
    free_block(l);

    create_lblock(log_size);
    l = (l_block*) get_block(log_size);
  }
  
  l->entries[l->num_entries] = entry;
  l->num_entries++;
  l->checksum = get_checksum(l);

  if (!write_block(l, log_size))
    DIE("Couldn't write to block " << log_size);
  
  free_block(l);
}

bool log_full() {
  if (log_size < LOG_SIZE)
    return false;
  else if (log_size > LOG_SIZE)
    DIE("Somehow log size " << log_size << " exceeds the max?");

  bool is_full;

  l_block *l = (l_block*) get_block(log_size);

  is_full = (l->num_entries == LOG_ENTRIES_PER_BLOCK);

  free_block(l);

  return is_full;
}

// Read log metadata from disk to memory
void initialize_log() {
  s_block *super = (s_block *) get_block(0);

  generation = super->generation;
  log_start = super->log_start;
  log_size = super->log_size;

  if (VERBOSE) {
    cout << "Reading in log..." << endl;
    cout << "\tGeneration: " << generation << endl;
    cout << "\tLog start: " << log_start << endl;
    cout << "\tLog size: " << log_size << endl;
  }

  free_block(super);
}

// Format block as a log entry block
void create_lblock(unsigned int block) {
  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph in start of create lblock\n";
  }
  l_block *l = (l_block*) get_block(block);

  l->generation = generation;
  l->num_entries = 0;

  l->checksum = get_checksum(l);

  if (!write_block(l, block))
    DIE("Couldn't write to block " + block);

  free_block(l);

  if (!checksum(block))
    DIE("Could not format log block " << block);

  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph in end of create lblock\n";
  }
}

// Create initialized superblock
void create_sblock() {
  s_block *s = (s_block*) get_block(0);

  s->generation = 0;
  s->log_start = 1;
  s->log_size = 1;

  s->checksum = get_checksum(s);

  if (!write_block(s, 0))
    DIE("Couldn't write to block " + 0);

  free_block(s);

  if (!checksum(0))
    DIE("Could not format log block " << 0);
}

void restore_graph() {

  load_checkpoint();

  // iterate over every log block
  for (unsigned int i = 1; i <= log_size; i++) {

    if(VERBOSE) DEBUG("On block " << i);

    if (!checksum(i))
      DEBUG("Block " << i << " corrupted!");

    l_block *l = (l_block*) get_block(i);

    // stop if we hit an older generation
    if (l->generation != generation)
      break;

    for (unsigned int j = 0; j < l->num_entries; j++)
      redo_operation(l->entries[j]);

    free_block(l);
  }

  if(VERBOSE) {
    print_nodes();
    print_graph();
    cout << "nodes and graph after restore\n";
  }
}

void load_checkpoint() {
  clear_adjacency_list_and_nodes();

  uint64_t *num_nodes = (uint64_t *) malloc(sizeof(uint64_t));
  uint64_t *n = (uint64_t *) malloc(sizeof(uint64_t));

  uint64_t *num_edges = (uint64_t *) malloc(sizeof(uint64_t));
  edge *e = (edge *) malloc(sizeof(edge));

  unsigned int offset = 0;

  uint64_t *generationRead = (uint64_t *) malloc(sizeof(uint64_t));

  ssize_t size_read = pread(fildes, (void *) generationRead, sizeof(uint64_t), LOG_SIZE*BLOCK_SIZE);
  if ((*generationRead) != generation) {
    return;
  }
  if (size_read <= 0) {
    free(generationRead);
    free(num_nodes);
    free(n);
    free(num_edges);
    free(e);
    return;
  }
  offset++;

  size_read = pread(fildes, (void *) num_nodes, sizeof(uint64_t), LOG_SIZE*BLOCK_SIZE);
  if (size_read <= 0) {
    free(generationRead);
    free(num_nodes);
    free(n);
    free(num_edges);
    free(e);
    return;
  }
  offset++;

  for (int i = 0; i < size_read; i++) {
    size_read = pread(fildes, (void *) n, sizeof(uint64_t), LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
    if (size_read <= 0) {
      free(generationRead);
      free(num_nodes);
      free(n);
      free(num_edges);
      free(e);
      return;
    }
    add_node(*n);
    offset++;
  }

  size_read = pread(fildes, (void *) num_edges, sizeof(uint64_t), LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
  if (size_read <= 0) {
    free(generationRead);
    free(num_nodes);
    free(n);
    free(num_edges);
    free(e);
    return;
  }
  offset++;

  for (int i = 0; i < size_read; i++) {
    size_read = pread(fildes, (void *) e, sizeof(edge), LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
    if (size_read <= 0) {
      free(generationRead);
      free(num_nodes);
      free(n);
      free(num_edges);
      free(e);
      return;
    }
    add_edge(e->node_a,e->node_b);
    offset += sizeof(edge);
  }

  free(num_nodes);
  free(n);
  free(num_edges);
  free(e);
}

bool checkpoint(){
  unordered_set<uint64_t> nodes = *(get_nodes());
  uint64_t num_nodes = (uint64_t) nodes.size();

  // Number of edges where a < b, since each edge is stored twice
  uint64_t num_unique_edges = (uint64_t) get_num_edges() / 2;

  edge *e = (edge *) malloc(sizeof(edge));
  unsigned int offset = 0;

  ssize_t bytes_written = pwrite(fildes,
                                (void *) &generation,
                                sizeof(uint64_t),
                                LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
  if (bytes_written <= 0) {
    free(e);
    return false;
  }

  offset++;

  bytes_written = pwrite(fildes,
                          (void *) &num_nodes,
                          sizeof(uint64_t),
                          LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
  if (bytes_written <= 0) {
    free(e);
    return false;
  }

  offset++;

  for (uint64_t node : nodes) {
    bytes_written = pwrite(fildes,
                            (void *) &node,
                            sizeof(uint64_t),
                            LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
    if (bytes_written <= 0) {
      free(e);
      return false;
    }
    offset++;
  }

  bytes_written = pwrite(fildes,
                          (void *) &num_unique_edges,
                          sizeof(uint64_t),
                          LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
  if (bytes_written <= 0) {
    free(e);
    return false;
  }
  offset++;

  for (uint64_t node : nodes) {
    list<uint64_t> neighbors = get_neighbors(node).neighbors;
    for (uint64_t neighbor : neighbors) {
      if (node < neighbor) {
        e->node_a = node;
        e->node_b = neighbor;
        bytes_written = pwrite(fildes,
                                (void *) e,
                                sizeof(edge),
                                LOG_SIZE*BLOCK_SIZE + offset*sizeof(uint64_t));
        if (bytes_written <= 0) {
          free(e);
          return false;
        }
        offset += sizeof(edge);
      }
    }
  }
  free(e);

  format_superblock();
  return true;
}

// to run AFTER init
void test(){

  DEBUG("Testing...");

  // make nodes 0 - 999
  for(unsigned int i = 0; i < 1000; i++){
    log_entry le;

    le.opcode = 0;
    le.node_a = i;

    log(le);
  }

   // chain nodes 0-99 to successor
  for(int i = 0; i < 100; i++){
    log_entry le;

    le.opcode = 1;
    le.node_a = i;
    le.node_b = i + 1;

    log(le);
  }

  // delete the first ten edges
  for(int i = 0; i < 10; i++){
    log_entry le;

    le.opcode = 3;
    le.node_a = i;
    le.node_b = i + 1;

    log(le);
  }

  // delete the last ten nodes
  for(int i = 990; i < 1000; i++){
    log_entry le;

    le.opcode = 2;
    le.node_a = i;

    log(le);
  }

  // hopefully this works!
  DEBUG("Restoring graph...");
  restore_graph();
  DEBUG("Done restoring graph...");

  for(int i = 0; i < 100; i++){
    if(!get_node(i).in_graph)
      DIE("Error: node " << i << " not in graph!");
  }
  
  for(int i = 990; i < 1000; i++){
    if(get_node(i).in_graph)
      DIE("Error: node " << i << " in graph!");
  }

  for(int i = 0; i < 10; i++){
    if(get_edge(i, i + 1).in_graph)
      DIE("Error: edge " << i << ", " << i + 1 << " in graph!");
  }
 
  // Recall that the 990'th node was deleted...
  for(int i = 10; i < 99; i++){
    if(!get_edge(i, i + 1).in_graph)
      DIE("Error: edge " << i << ", " << i + 1 << " not in graph!");
  }

  DEBUG("All tests passed!");
}

