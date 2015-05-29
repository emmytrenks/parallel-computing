# Memory
Memory is (assumed) to be distributed even if it's on the same machine.

# Paradigm
The identical program is ran on each node, with the only differentiating variable being the node's `rank`. Using this, your program is responsible for knowing what work it is meant to process and contribute towards the root node.
