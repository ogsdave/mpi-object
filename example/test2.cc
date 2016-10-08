#include <stdio.h>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include "mpi_object.h"
#include "Sphere.Atom.pb.h"
#include "Sphere.SurfacePoint.pb.h"
#include "MpiObject.Item.pb.h"

using namespace MpiObject;
using namespace Sphere;
using namespace std;

//MPI_Send_Object and MPI_Recv_Object
int main(int argc,char** argv){
  	int total;
	int my_rank;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&total);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	Item** its=new Item*[2];
	if(my_rank==0){
		Item* it;
		Atom* at;
		at=new Atom();
		at->set_seq(1.23);
		at->set_name("aaa");
		it=new Item();
		it->set_allocated_atom(at);
		its[0]=it;
		at=new Atom();
		at->set_seq(2.12);
		at->set_name("bbb");
		it=new Item();
		it->set_allocated_atom(at);
		its[1]=it;
		for(int i=1;i<total;i++){
		  MPI_Send_Object(its,2,i,0,MPI_COMM_WORLD);
		}
	}else{
		MPI_Recv_Object(its,2,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		Item* it;
		Atom* at;
                for(int i=0;i<2;i++){
	           it=its[i];
		    at=it->mutable_atom();
		    cout<<at->seq()<<" "<<*(at->mutable_name())<<endl;
		}     
	}
	MPI_Finalize();
	return 0;
}
