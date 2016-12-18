namespace cpp rpc

enum Operation {
  ADD_NODE = 0,
  ADD_EDGE = 1,
  REMOVE_NODE = 2,
  REMOVE_EDGE = 3,
  LOCK = 4,
  UNLOCK = 5
}

struct Packet {
	1: Operation op,
 	2: i64 node_a,
 	3: i64 node_b,
}

service GraphEdit {

   bool editGraph(1:Packet p)

}
