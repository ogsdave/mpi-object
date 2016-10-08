The basic MPI is unable to pass C++ object. We have extended basic MPI and developed five functions-MPI_Send_Object, MPI_Recv_Object, MPI_Gather_Object, MPI_Scatter_Object, MPI_Bcast_Object through which C++ object can be passed easily as the simple data type of int, double, char.
To use these functions right,the Protobuf(https://github.com/google/protobuf/) will have to
be installed first. The property in the file MpiObject.Item.proto is modified to your property.

