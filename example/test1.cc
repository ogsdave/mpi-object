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

//MPI_Gather_Object
int main(int argc,char** argv){
  	int total;
	int my_rank;
	Item* it;
	Atom* at;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&total);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	Item** send_its=new Item*[2];
        Item** recv_its=new Item*[8];
        SurfacePoint* sp;
	if(my_rank==0){
		at=new Atom();
		at->set_seq(1.23);
		at->set_name("aaa");
                sp=at->add_points();
                sp->set_flag("sc");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[0]=it;

		at=new Atom();
		at->set_seq(2.23);
		at->set_name("aab");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[1]=it;
        }
        if(my_rank==1){
		at=new Atom();
		at->set_seq(2.12);
		at->set_name("bbb");
                sp=at->add_points();
                sp->set_flag("sd");
                sp->set_x(25);
		it=new Item();
		it->set_allocated_atom(at);
		send_its[0]=it;
 
		at=new Atom();
		at->set_seq(2.24);
		at->set_name("bba");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[1]=it;
        }
        if(my_rank==2){
		at=new Atom();
		at->set_seq(1.45);
		at->set_name("ccc");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[0]=it;

		at=new Atom();
		at->set_seq(2.29);
		at->set_name("ba");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[1]=it;
         }
         if(my_rank==3){
		at=new Atom();
		at->set_seq(1.73);
		at->set_name("ddd");
                sp=at->add_points();
                sp->set_flag("sa");
                sp->set_x(23);
                sp=at->add_points();
                sp->set_flag("sb");
                sp->set_x(24);
		it=new Item();
		it->set_allocated_atom(at);
		send_its[0]=it;

		at=new Atom();
		at->set_seq(2.9);
		at->set_name("aaaa");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[1]=it;
	 }
        
         MPI_Gather_Object(send_its,2,recv_its,2,0,MPI_COMM_WORLD);
        
         if(my_rank==0){
                for(int i=0;i<8;i++){
	           it=recv_its[i];
                     at=it->mutable_atom();
		     cout<<at->seq()<<" "<<*(at->mutable_name())<<endl;
                     for(int i=0;i<at->points_size();i++){
                       sp=at->mutable_points(i);
                       cout<<"  "<<sp->flag()<<" "<<sp->x()<<endl;
		    }
		}     
	  }    
	MPI_Finalize();
	return 0;
 }

