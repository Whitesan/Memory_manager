#if !defined(_MY_MALLOC_H_)
#define _MY_MALLOC_H_
#define PAGE_SIZE 4096
#include<stdlib.h>
#include<stdint.h>
enum pointer_type_t
{
    pointer_null,
    pointer_out_of_heap,
    pointer_control_block,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};
struct meta_data
{
    int size;
    void * next;
    void * prev;
    int ctrl_sum;
};
//Standardowe
void* heap_malloc(size_t size);
void* heap_calloc(size_t n,size_t size);
void * heap_realloc(void *ptr,size_t size);
void* heap_setup(size_t size);
void heap_free(void* ptr);
void show_rheap();
//Statystyczne
size_t heap_get_used_space(void);
size_t heap_get_largest_used_block_size(void);
uint64_t heap_get_used_blocks_count(void);
size_t heap_get_free_space(void);
size_t heap_get_largest_free_area(void);
uint64_t heap_get_free_gaps_count(void);
enum pointer_type_t get_pointer_type(const const void* pointer);
void show_heap();
void* start; //Początek stosu

#endif