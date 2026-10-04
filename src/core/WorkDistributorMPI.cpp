#include "WorkDistributorMPI.hpp"
#include "lacc/core/Mpi.hpp"
#include <string>

#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
  #include <mpi.h>
#endif

namespace lacc::core {

#if defined(LACC_HAS_MPI) && LACC_HAS_MPI

static constexpr int TAG_WORK = 100;
static constexpr int TAG_STOP = 101;
static constexpr int TAG_REQ  = 102;

struct WorkWire {
  std::uint32_t id;
  std::uint64_t offset;
  std::uint64_t length;
};


void master_distribute(const std::vector<WorkItem>& items) {
  std::size_t next = 0; // Index of the next work item to send
  int active_workers = size() - 1; // Total number of workers to manage

  while (active_workers > 0) {
    // TODO: 1. Use MPI_Recv to wait for a request (TAG_REQ) from ANY_SOURCE.
    // Hint: Use MPI_Status to find out which 'worker' rank actually sent the request.
    int signal;
    MPI_Status status;
    MPI_Recv(&signal, 1, MPI_INT, MPI_ANY_SOURCE, TAG_REQ, MPI_COMM_WORLD, &status);
    // TODO: 2. Check if 'next' has reached items.size().
    // IF NO MORE WORK:
    //    - Send a message with TAG_STOP to the requesting worker.
    //    - Decrement active_workers.
    if (next == items.size()) {
      MPI_Send(&signal, 1, MPI_INT, status.MPI_Source, TAG_STOP, MPI_COMM_WORLD);
      active_workers--;
    } else {
      // IF WORK REMAINS:
      //    - Get the WorkItem at items[next].
      //    - Use MPI_Send to send the WorkWire struct (TAG_WORK).
      //    - Use MPI_Send to send the string size, then the string data itself.
      //    - Increment 'next'.
      WorkItem work = items[next];
      WorkWire wire = {work.id, work.offset, work.length};
      // WorkWire struct
      MPI_Send(&wire, sizeof(WorkWire), MPI_BYTE, MPI_ANY_SOURCE, TAG_WORK, MPI_COMM_WOLRD);
      // String data
      std::uint32_t str_size = static_cast<std::uint32_t>(work.source.size());

      MPI_Send(&str_size, 1, MPI_UINT32_T, MPI_ANY_SOURCE, TAG_WORK, MPI_COMM_WORLD);
      MPI_Send(work.source.data(), str_size, MPI_CHAR, MPI_ANY_SOURCE, TAG_WORK, MPI_COMM_WORLD);
      next++;
    }
  }
}

bool worker_get_next(WorkItem& out) {
  // TODO: 1. Send a "request for work" message (TAG_REQ) to Rank 0.
  int req = 0;
  MPI_Send(&req, 1, MPI_INT, 0 /*rank*/, TAG_REQ, MPI_COMM_WORLD);
  // TODO: 2. Use MPI_Probe to look at the incoming message from Rank 0.
  // This allows you to check the TAG before actually receiving the data.
  MPI_Status TAG;
  MPI_Probe(0 /*rank*/, TAG_WORK /*placeholder*/, MPI_COMM_WORLD, &TAG);
  // TODO: 3. Handle the Tag:
  // IF TAG == TAG_STOP:
  //  - Receive the empty message to clear the buffer.
  //  - Return 'false' to tell the caller we are done.
  if (TAG.MPI_TAG == TAG_STOP) {
    int rand_buf;
    MPI_Status status;
    MPI_Recv(&rand_buf, 1, MPI_INT, TAG.MPI_SOURCE, TAG_STOP, MPI_COMM_WORLD, &status);
    return false;
  }
  // IF TAG == TAG_WORK:
  //  - Receive the WorkWire struct into a local variable.
  //  - Receive the string length (uint32_t).
  //  - Resize the 'out.source' string and receive the character data into it.
  //  - Map the received WorkWire values to the 'out' object.
  //  - Return 'true'.
  if (TAG.MPI_TAG == TAG_WORK) {
    WorkWire out;
    MPI_Status status;
    MPI_Recv(&out, sizeof(WorkWire), MPI_BYTE /*placeholder*/, TAG.MPI_SOURCE, TAG.MPI_TAG, MPI_COMM_WORLD, &status);
    std::uint32_t str_size = 0;
    MPI_Recv(&str_size, 1, MPI_UINT32_T, TAG.MPI_SOURCE, TAG.MPI_TAG, MPI_COMM_WORLD, &status);
    out.source.resize(str_size);
    MPI_Recv(out.source.data(), str_size, MPI_CHAR, TAG.MPI_SOURCE, TAG.MPI_TAG, MPI_COMM_WORLD, &status);
    return true;
  }
  return false; // Placeholder
}

#else
// Fallback for non-MPI builds
void master_distribute(const std::vector<WorkItem>&) {}
bool worker_get_next(WorkItem&) { return false; }
#endif

} // namespace lacc::core
