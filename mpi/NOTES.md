# Memory
Memory is (assumed) to be distributed even if it's on the same machine.

# Paradigm
The identical program is ran on each node, with the only differentiating variable being the node's `rank`. Using this, your program is responsible for knowing what work it is meant to process and contribute towards the root node.

# MPI Components
## MPI_Init
Tells MPI to do all the necessary setup

## MPI_Finalize
Tells MPI we are done, and to clean up anything allocated for this program

## MPI_Comm_size
Reports the number of processes

## MPI_Comm_rank
Reports the rank, a number between - and size -1, identifying the calling process

## MPI_SEND
MPI_SEND(start, count, datatype, dest, tag, comm)

-> Start, count, and datatype is the message buffer

-> Target process is specified by dest (destination), which is the rank of the target process in the communicator
  specified by comm

-> When this function returns, the data has been delivered to the system and the buffer can be reused

## MPI_RECV
MPI_RECV(start, count, datatype, source, tag, comm, status)

-> waits until a matching (both source and tag) message is received from the system

-> source is rank in communicator specified by comm, or *MPI_ANY_SOURCE*

-> tag is a tag to be matched on or *MPI_ANY_TAG*

# Datatypes
Types | * *
--- | ---
MPI_CHAR | MPI_SHORT
MPI_INT | MPI_LONG
MPI_LONG_LONG | MPI_UNSIGNED CHAR
MPI_UNSIGNED_SHORT | MPI_UNSIGNED
MPI_UNSIGNED_LONG | MPI_FLOAT
MPI_DOUBLE | MPI_LONG_DOUBLE
MPI_BYTE | MPI_PACKED
