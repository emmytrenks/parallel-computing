#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define TAG_ECHO 0x1

int main(int argc, char **argv) {
  const int MESSAGE_MAX = 256;

  int forkRank = 0, forkSize = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &forkRank);
  MPI_Comm_size(MPI_COMM_WORLD, &forkSize);

  if (forkRank == 0) {
    puts("[FORK 0] Setting up message receivers.");
    for (int i = 1; i < forkSize; ++i) {
      char recvMessage[MESSAGE_MAX];
      //Message receive is blocking until it receives from a send
      MPI_Recv(&recvMessage, MESSAGE_MAX, MPI_CHAR, i, TAG_ECHO, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      char prefixed[MESSAGE_MAX];
      sprintf(prefixed, "[FORK %d] ", i);
      strncat(prefixed, recvMessage, MESSAGE_MAX);
      puts(prefixed);
    }
  } else {
    char sendMessage[64] = "Hello MPI! We're node ";
    char buffer[64];
    //Be sure to modify the seed sent to the PRNG so results are
    // not the same cross nodes.
    srand(time(NULL) ^ forkRank);
    sprintf(buffer, "%d (salt: %d).", forkRank, rand() & 0xff);
    strncat(sendMessage, buffer, MESSAGE_MAX);
    //Message send is blocking until it is received by a receive.
    MPI_Send(&sendMessage, MESSAGE_MAX, MPI_CHAR, 0, TAG_ECHO, MPI_COMM_WORLD);
  }

  MPI_Finalize();
}
