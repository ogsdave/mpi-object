#include <mpi.h>
#include "MpiObject.Item.pb.h"

using namespace MpiObject;

int  MPI_Send_Object(Item** its,int size, int dest, int tag, MPI_Comm comm);
int  MPI_Recv_Object(Item** its,int size,int source,int tag,MPI_Comm comm,MPI_Status* st);
int  MPI_Bcast_Object(Item** its,int count,int source_proc,MPI_Comm comm);
int MPI_Scatter_Object(Item** send_its,int send_count,Item** recv_its,int recv_count,int src_proc,MPI_Comm comm);
int MPI_Gather_Object(Item** send_its,int send_count,Item** recv_its,int recv_count,int dest_proc,MPI_Comm comm);
int MPI_Bcast_Object(Item** list,int size,int source_proc,MPI_Comm comm);
