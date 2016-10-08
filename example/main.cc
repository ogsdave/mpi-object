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


//MPI_Scatter_Object
int main(int argc,char** argv){
  	int total;
	int my_rank;
	Item* it;
	Atom* at;
        SurfacePoint* sp;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&total);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	Item** send_its=new Item*[6];
        Item** recv_its=new Item*[3];
	if(my_rank==1){
		at=new Atom();
		at->set_seq(1.23);
		at->set_name("aaa");
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
		at->set_seq(2.12);
		at->set_name("bbb");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[1]=it;

		at=new Atom();
		at->set_seq(1.45);
		at->set_name("ccc");
                sp=at->add_points();
                sp->set_flag("sc");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[2]=it;

		at=new Atom();
		at->set_seq(1.73);
		at->set_name("ddd");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[3]=it;

                at=new Atom();
		at->set_seq(2.23);
		at->set_name("aab");
                sp=at->add_points();
                sp->set_flag("sd");
                sp->set_x(25);
		it=new Item();
		it->set_allocated_atom(at);
		send_its[4]=it;

		at=new Atom();
		at->set_seq(2.12);
		at->set_name("bba");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[5]=it;

		at=new Atom();
		at->set_seq(1.45);
		at->set_name("ccc");
		it=new Item();
		it->set_allocated_atom(at);
		send_its[6]=it;
	}
      
        MPI_Scatter_Object(send_its,1,recv_its,2,1,MPI_COMM_WORLD);
             for(int i=0;i<2;i++){
	           it=recv_its[i];
                   if(it==NULL){
                      out<<"null"<<endl;
                   }else{
		   at=it->mutable_atom();
		   out<<at->seq()<<" "<<*(at->mutable_name())<<endl;
                   for(int i=0;i<at->points_size();i++){
                      sp=at->mutable_points(i);
                      out<<"  "<<sp->flag()<<" "<<sp->x()<<endl;
		   }
                  }
	      }      
	MPI_Finalize();
	return 0;
 }

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
