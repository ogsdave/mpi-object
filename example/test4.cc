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

//MPI_Bcast_Object
int main(int argc,char** argv){
  	int total;
	int my_rank;
	Item* it;
	Atom* at;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&total);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	Item** list=new Item*[4];
	if(my_rank==0){
		at=new Atom();
		at->set_seq(1.23);
		at->set_name("aaa");
		it=new Item();
		it->set_allocated_atom(at);
		list[0]=it;
		at=new Atom();
		at->set_seq(2.12);
		at->set_name("bbb");
		it=new Item();
		it->set_allocated_atom(at);
		list[1]=it;
		at=new Atom();
		at->set_seq(1.45);
		at->set_name("ccc");
		it=new Item();
		it->set_allocated_atom(at);
		list[2]=it;
		at=new Atom();
		at->set_seq(1.73);
		at->set_name("ddd");
		it=new Item();
		it->set_allocated_atom(at);
		list[3]=it;
	}

         MPI_Bcast_Object(list,4,0,MPI_COMM_WORLD);

                for(int i=0;i<4;i++){
	           it=list[i];
		   at=it->mutable_atom();
		   cout<<at->seq()<<" "<<*(at->mutable_name())<<" "<<endl;
		}
		
	MPI_Finalize();
	return 0;
 }

