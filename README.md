# Distributed-Graph-Service
This group consists of Robert Tung and Tyler Petrochko.

CS 426 Assignment #2
The workload was split in the following way:

One partner (Tyler) created the skeleton code for the project (setting up all the different files, makefiles, tests.sh). Tyler also implemented all the functions in util.cpp, added getters to memorygraph.cpp, and wrote the part of persistence.cpp that deals with logging (format_superblock, format_disk, restore_graph, create_lblock, initialize_log, log, log_full). Tyler also did debugging on these log parts of the code in isolation.

The other partner (Robert) wrote all of checkpoint and load_checkpoint (including the API endpoint, all structs and constants required and the refactoring of these functions as well as many other parts of the code). Robert also did substantive debugging which led to edits across functions of persistence.cpp (format_superblock, format_disk, restore_graph) and some changes in util.cpp and memorygraph.cpp.

###########################################
Text from Assignment 1 README below:

CS 426 Assignment #1

The workload was split such that the graph code was written by one partner (Robert Tung) and the networking code was written by the other partner (Tyler Petrochko). Thus, all code in memorygraph.cpp, memorygraph.hpp and all code to test and compile this portion was written by Robert. All code in main.cpp and the code needed to compile and test the networking code was written by Tyler. After each piece was written and tested, both of us tested the code as a whole before submission.
