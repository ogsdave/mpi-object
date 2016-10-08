#include <sstream>
#include <stdio.h>
#include <mpi.h>
#include "MpiObject.Item.pb.h"
#include "MpiObject.ObjectArray.pb.h"

using namespace std;
using namespace MpiObject;

int  MPI_Send_Object(Item** list,int size, int dest, int tag, MPI_Comm comm)
{
   ostringstream output;
   ObjectArray* array=new ObjectArray();
   int error;
   int char_size;
   Item* item;
   int my_rank;

   MPI_Comm_rank(comm,&my_rank);
   for(int i=0;i<size;i++){
	item=array->add_items();
        item->CopyFrom(*list[i]);
   }
   if (!array->SerializeToOstream(&output)) { 
       printf("进程%d对象序列化失败\n",my_rank);
       MPI_Abort(comm,-1);
   }
   char_size=output.tellp();
   error=MPI_Send(output.str().data(),char_size,MPI_CHAR,dest,tag,comm); 

   delete array;
   return error;
}

int  MPI_Recv_Object(Item** list,int recv_size,int source,int tag,MPI_Comm comm,MPI_Status* status)
{
   char* recv_buf;
   int error;
   int buf_size[2];
   int char_size;
   int send_size;
   string* str_buf;
   Item* item;
   istringstream input;
   ObjectArray* array=new ObjectArray();
   MPI_Status st;
   int my_rank;

   MPI_Comm_rank(comm,&my_rank);
   MPI_Probe(source, tag, comm, &st);
   MPI_Get_count(&st,MPI_CHAR,&char_size);
   recv_buf=(char*)malloc(char_size*sizeof(char));
   if(!recv_buf){
       printf("进程%d分配recv缓冲区失败\n",my_rank);
       MPI_Abort(comm,-1);
   }
   error=MPI_Recv(recv_buf,char_size,MPI_CHAR,source,tag,comm,status);
   str_buf=new string(recv_buf,char_size);
   input.str(*str_buf);
   if (!array->ParseFromIstream(&input)) {
	  printf("进程%d对象反序列化失败\n",my_rank);
          MPI_Abort(comm,-1);
   }
   send_size=array->items_size();
   if(send_size>recv_size){
	  printf("进程%d发送了%d个对象，但进程%d只能接收%d对象，因此接收失败\n",source,send_size,my_rank,recv_size);
          MPI_Abort(comm,-1);
   }
   for(int i=0;i<send_size;i++){
        item=array->mutable_items(i);
	list[i]=new Item(*item);
   }
   for(int i=send_size;i<recv_size;i++){
	list[i]=NULL;
   }

   delete str_buf;
   delete array;
   return error;
}
int MPI_Bcast_Object(Item** list,int size,int source_proc,MPI_Comm comm){
    ObjectArray* array=new ObjectArray();
    int my_rank;
    int error;
    ostringstream output;
    char* char_buf;
    int char_size;
    string* str_buf;
    istringstream input;
    Item* item;
    int send_size;
    int recv_size;

    MPI_Comm_rank(comm,&my_rank);
    if(my_rank==source_proc){
       send_size=size;
       for(int i=0;i<send_size;i++){
	 item=array->add_items();
         item->CopyFrom(*list[i]);
       }
       if (!array->SerializeToOstream(&output)) { 
         printf("进程%d对象序列化失败\n",my_rank);
         MPI_Abort(comm,-1);
       }
       char_size=output.tellp();
       char_buf=(char*)malloc(char_size*sizeof(char));
       if(!char_buf){
          printf("进程%d分配char_buf缓冲区失败\n",my_rank);
          MPI_Abort(comm,-1);
       }
       output.str().copy(char_buf,char_size);
       error=MPI_Bcast(&char_size,1,MPI_INT,source_proc,comm);
       error=MPI_Bcast(char_buf,char_size,MPI_CHAR,source_proc,comm);
    }else{
       recv_size=size;
       error=MPI_Bcast(&char_size,1,MPI_INT,source_proc,comm);
       char_buf=(char*)malloc(char_size*sizeof(char));
       if(!char_buf){
          printf("进程%d分配char_buf缓冲区失败\n",my_rank);
          MPI_Abort(comm,-1);
       }
       error=MPI_Bcast(char_buf,char_size,MPI_CHAR,source_proc,comm);
       str_buf=new string(char_buf,char_size);
       input.str(*str_buf);
       if (!array->ParseFromIstream(&input)) {
	   printf("进程%d对象反序列化失败\n",my_rank);
           MPI_Abort(comm,-1);
       }
       send_size=array->items_size();
       if(send_size>recv_size){
	  printf("进程%d发送了%d个对象，但进程%d只能接收%d对象，因此接收失败\n",source_proc,send_size,my_rank,recv_size);
          MPI_Abort(comm,-1);
       }
       for(int i=0;i<send_size;i++){
         item=array->mutable_items(i);
	 list[i]=new Item(*item);
       }
       for(int i=send_size;i<recv_size;i++){
	list[i]=NULL;
       }
    }

    if(!str_buf)
      delete str_buf;
    delete array;
    return error;
}
int MPI_Gather_Object(Item** send_list,int send_count,Item** recv_list,int recv_count,int dest_proc,MPI_Comm comm){
    int my_rank;
    int comm_size;
    int char_size=0;
    int* all_char_size;
    char* recv_buf;
    string* str_buf;
    int* displs;
    int error;
    int char_total;
    Item* item;
    Item* item1;
    ostringstream output;
    istringstream input;
    ObjectArray* array=new ObjectArray();
    int send_size;
    int recv_size;

    MPI_Comm_rank(comm,&my_rank);
    MPI_Comm_size(comm,&comm_size);
    if(my_rank!=dest_proc){
       send_size=send_count;
       for(int i=0;i<send_size;i++){
	 item=send_list[i];
         item1=array->add_items();
         item1->CopyFrom(*item);
       }
       if (!array->SerializeToOstream(&output)) { 
           printf("进程%d对象序列化失败\n",my_rank);
	   MPI_Abort(comm,-1);
       }
       char_size=output.tellp();
       //printf("aa%d\n",char_size);
       error=MPI_Gather(&char_size,1,MPI_INT,all_char_size,1,MPI_INT,dest_proc,comm);
       error=MPI_Gatherv(output.str().data(),char_size,MPI_CHAR,recv_buf,all_char_size,displs,MPI_CHAR,dest_proc,comm);
    }else{
       recv_size=recv_count;
       all_char_size=(int*)malloc(comm_size*sizeof(int));
       if(!all_char_size){
           printf("进程%d分配recv_size缓冲区失败\n",my_rank);
	   MPI_Abort(comm,-1);
       }
       displs=(int*)malloc(comm_size*sizeof(int));
       if(!displs){
           printf("进程%d分配displs缓冲区失败\n",my_rank);
           MPI_Abort(comm,-1);
       }
       error=MPI_Gather(&char_size,1,MPI_INT,all_char_size,1,MPI_INT,dest_proc,comm);
       displs[0]=0;
       for(int i=1;i<comm_size;i++){
	   displs[i]=displs[i-1]+all_char_size[i-1];
       }
       char_total=displs[comm_size-1]+all_char_size[comm_size-1];
       recv_buf=(char*)malloc(char_total*sizeof(char));
       if(!recv_buf){
           printf("进程%d分配recv_buf缓冲区失败\n",my_rank);
	   MPI_Abort(comm,-1);
       }
       error=MPI_Gatherv(NULL,0,MPI_CHAR,recv_buf,all_char_size,displs,MPI_CHAR,dest_proc,comm);
       str_buf=new string();
       for(int i=0;i<comm_size;i++){
          if(i!=my_rank){
		str_buf->assign(recv_buf+displs[i],all_char_size[i]);
                input.str(*str_buf);
                if (!array->ParseFromIstream(&input)) { 
                    printf("进程%d发送的对象反序列化失败\n",i);
		    MPI_Abort(comm,-1);
                 }
                 send_size=array->items_size();
          }else{
                send_size=send_count;
          }
          if(send_size>recv_size){
	     printf("进程%d发送%d个对象，但目标进程%d只能接收%d对象，因此接收失败\n",i,send_size,dest_proc,recv_size);
             MPI_Abort(comm,-1);
          }
          for(int j=0;j<send_size;j++){
                item=(i!=my_rank)?array->mutable_items(j):send_list[j];
                recv_list[i*recv_size+j]=new Item(*item);
          }
          for(int k=send_size;k<recv_size;k++){
	       recv_list[i*recv_size+k]=NULL;
          }
          if(i!=my_rank){
                array->clear_items();
                input.clear();
          }
          
       }
     }

   if(!str_buf){
      delete str_buf;
   }
   delete array;
   return error;
}
int MPI_Scatter_Object(Item** send_list,int send_count,Item** recv_list,int recv_count,int src_proc,MPI_Comm comm){
    int my_rank;
    int comm_size;
    int char_size;
    int error;
    int* all_char_size;
    int* displs;
    Item* item;
    Item* item1;
    int begin;
    int end;
    char* recv_buf;
    string* str_buf;
    ostringstream output;
    istringstream input;
    ObjectArray* array=new ObjectArray();
    int send_size=send_count;
    int recv_size=recv_count;

    MPI_Comm_rank(comm,&my_rank);
    MPI_Comm_size(comm,&comm_size);
    if(my_rank==src_proc){
       all_char_size=(int*)malloc(comm_size*sizeof(int));
       if(!all_char_size){
           printf("进程%d分配send_size缓冲区失败\n",my_rank);
           MPI_Abort(comm,-1);
       }
       displs=(int*)malloc(comm_size*sizeof(int));
       if(!displs){
           printf("进程%d分配displs缓冲区失败\n",my_rank);
           MPI_Abort(comm,-1);
       }
       for(int i=0;i<comm_size;i++){
          begin=output.tellp();
          if(i!=my_rank){
              for(int j=0;j<send_size;j++){
                  item=send_list[i*send_size+j];
                  item1=array->add_items();
                  item1->CopyFrom(*item);
              }
              if (!array->SerializeToOstream(&output)) { 
                 printf("进程%d对象序列化失败\n",my_rank);
                 MPI_Abort(comm,-1);
              }
              array->clear_items();
	  }else{
          }
         end=output.tellp();
         //printf("aa%d\n",(end-begin));
         all_char_size[i]=end-begin;
         displs[i]=begin;
       }
       error=MPI_Scatter(all_char_size,1,MPI_INT,&char_size,1,MPI_INT,src_proc,comm);
       error=MPI_Scatterv(output.str().data(),all_char_size,displs,MPI_CHAR,recv_buf,char_size,MPI_CHAR,src_proc,comm);
    }else{
       error=MPI_Scatter(all_char_size,1,MPI_INT,&char_size,1,MPI_INT,src_proc,comm);
       recv_buf=(char*)malloc(char_size*sizeof(char));
       if(!recv_buf){
           printf("进程%d分配recv_buf缓冲区失败\n",my_rank);
	   MPI_Abort(comm,-1);
       }
       error=MPI_Scatterv(NULL,all_char_size,displs,MPI_CHAR,recv_buf,char_size,MPI_CHAR,src_proc,comm);
       str_buf=new string(recv_buf,char_size);
       input.str(*str_buf);
       if (!array->ParseFromIstream(&input)) { 
            printf("进程%d对象反序列化失败\n",my_rank);
            MPI_Abort(comm,-1);
       }
       send_size=array->items_size();
     
    }
    if(send_size>recv_size){
	    printf("进程%d发送%d个对象，但进程%d只能接收%d对象，因此接收失败\n",src_proc,send_size,my_rank,recv_size);
            MPI_Abort(comm,-1);
    }
    for(int i=0;i<send_size;i++){
         item=(my_rank==src_proc)?send_list[my_rank*send_size+i]:array->mutable_items(i);
	 recv_list[i]=new Item(*item);
    }
    for(int i=send_size;i<recv_size;i++){
	 recv_list[i]=NULL;
    }

   if(!str_buf){
      delete str_buf;
   }
   delete array;
   return error;
}

