#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include"custom_unistd.h"
#include"my_malloc.h"
int main()
{
    
    heap_setup(PAGE_SIZE);
  

	void* p1 = heap_malloc(100); // 8MB
	void* p2 = heap_malloc(50); // 8MB
 // printf("%ld",((intptr_t)p1 -32)& (intptr_t)(PAGE_SIZE - 1) == 0);
 printf("%d\n",(((intptr_t)start - (intptr_t)p1 + sizeof(struct meta_data)) &(intptr_t)(PAGE_SIZE - 1))==0);
	//printf("%ld",(intptr_t)start);
  
    return 0;
}