# Distributed-Graph-Service
CS 426 Assignment #3

This group consists of Robert Tung and Tyler Petrochko.

The workload was split in the following way:

Originally and completely separately, both parties started work on the code.
	Robert wrote the Thrift file and a full implementation (correctly compiled but at that point not correctly debugged) of all functionality including processing the packet and editing the graph on replicated nodes' servers, opening up client connections from one node to the next, having the replicated node also propagate the request before editing its own graph, and waiting for an ack and returning the correct response code for successes and failures in the system.

	Meanwhile, Tyler wrote the code to process the arguments and originally had also started writing some server code (there was a bit of redundancy in the work we did).
	
	After this was all done, Tyler also fully debugged the code, which ultimately led to code reorganization in addition to bug fixes (moving the creation of connections into init functions, moving the server code and propagation into a separate file called replication.cpp and cleaning up organization of handle_requests).

	Both partners ensured all code compiled and ran as expected before submission.