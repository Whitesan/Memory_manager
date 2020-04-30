#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"custom_unistd.h"
#include"my_malloc.h"
void show_heap()
{
    struct meta_data *md = (struct meta_data*)start;

    while(md!=NULL)
    {

        printf("Size: %d Dane: ",md->size);
        for(int i=0;i<abs(md->size);i++)
        {
            //printf("%d ",(*(char*)((void*)md + sizeof(struct meta_data) + i)));
        }
        printf("\n");
        md = (struct meta_data*)md->next;
    }
}
void show_rheap()
{
    struct meta_data *md = (struct meta_data*)start;
    struct meta_data *lmd;
    while(md!=NULL)
    {
      
        lmd = md;
        md = (struct meta_data*)md->next;
    }
    md = lmd;
    while(md!=NULL)
    {
      
        printf("Size: %d Dane: ",md->size);
        for(int i=0;i<abs(md->size);i++)
        {
            //printf("%d ",(*(char*)((void*)md + sizeof(struct meta_data) + i)));
        }
        printf("\n");
        md = (struct meta_data*)md->prev;
    }

}
void * heap_setup(size_t size)
{
    start = custom_sbrk(size);
    struct meta_data *md = (struct meta_data*)start;
    md->size = -(size - sizeof(struct meta_data));
    md->next = NULL;
    md->prev = NULL;
    md->ctrl_sum = 0;
    return NULL;
}
void* heap_malloc(size_t size)
{
    struct meta_data *md = (struct meta_data*)start;


    //Przeszukuje struktury w celu znalezienia przestrzeni o rozmiarze równym rozmiarowi podanemu w argumencie
    while(md!=NULL)
    {
        if(-md->size == size)
        {
            md->size=size;
            return (void*)(md) + sizeof(struct meta_data);
        }
        md = (struct meta_data*)md->next;
    }

    //Przesukuje struktury w celu znalezienia miejsca większego niż potrzebujemy a następnie podzielenie obszaru na 2 struktury
    md = (struct meta_data*)start;
    struct meta_data* last_md;
    while(md!=NULL)
    {
        if(md->size<0)
        {
            if(-md->size >= size + sizeof(struct meta_data))
            {

                int prev_size = -md->size;
                md->size = size;
                void *tp = md->next;
                md->next = (void*)md + md->size + sizeof(struct meta_data);
                struct meta_data *md_temp = (struct meta_data*)md->next;
                md_temp->size = -(prev_size - md->size - sizeof(struct meta_data));
                md_temp->prev = md;
                md_temp->next = tp;
                struct meta_data *nnmd = (struct meta_data*)tp;
                if(nnmd)
                    nnmd->prev = md_temp;
                return (void *)md + sizeof(struct meta_data);

            }
        }
        last_md = md;
        md = (struct meta_data*)md->next;
    }


    //Próba zwiększenia sterty
    md = last_md;
    void *ptr;
    int allocated_pages=0;
    while(1)
    {
        ptr = custom_sbrk(PAGE_SIZE);
        
        if(ptr == (void*) - 1) // Brak pamięci
       {
           custom_sbrk(-PAGE_SIZE * allocated_pages);
           md->size += (PAGE_SIZE * allocated_pages);
           return NULL;
       } 
            
        allocated_pages++;
        md->size -=PAGE_SIZE;
        if(-md->size >= size + sizeof(struct meta_data))
            break;
    }


    int prev_size = -md->size;
    md->size = size;
    void* tp = md->next;
    md->next = (void*)md + md->size + sizeof(struct meta_data);
    struct meta_data *md_temp = (struct meta_data*)md->next;
    md_temp->size = -(prev_size - md->size - sizeof(struct meta_data));
    md_temp->prev = md;
    md_temp->next = tp;
    return (void *)md + sizeof(struct meta_data);


}
void heap_free(void* ptr)
{
    struct meta_data *md = (struct meta_data*)(ptr - sizeof(struct meta_data)); //Obecny blok
    struct meta_data *pmd = (struct meta_data*)md->prev; // Poprzedni blok
    struct meta_data *nmd = (struct meta_data*)md->next; //Następny blok
    
    if(pmd && nmd) // Jeżeli poprzedni i następny blok są wolne to łącze te 3 bloki w jeden
    {
       
        if(pmd->size <= 0 && nmd->size <= 0)
        {
           
            pmd->size = pmd->size - md->size + nmd->size - 2 * sizeof(struct meta_data);
            struct meta_data *nnmd = (struct meta_data*)nmd->next;
            pmd->next = nnmd;
            if(nnmd)
                nnmd->prev = pmd;

            
            if(!nnmd)
            {
                if(pmd->size <= -PAGE_SIZE)  // Jeżeli ilość wolnej przestrzeni jest wystarczająca, zwracam ją do systemu
                {
                   
                    custom_sbrk(-(-pmd->size / PAGE_SIZE) * PAGE_SIZE );
                    pmd->size += (-pmd->size / PAGE_SIZE) * PAGE_SIZE;
                }
                    
            }    
            
            return;    
        }
        
    }
    if(pmd && pmd->size <= 0) // Jeżeli poprzedni blok wolny to ziekszam go o obecny blok
    {
        
        pmd->size -= (md->size + sizeof(struct meta_data));
       
        pmd->next = md->next;
        if(nmd)
            nmd->prev = md->prev;
        return;    
    }
    if(nmd && nmd->size <=0) // Jeżeli natepny blok wolny to ziekszam go o obecny blok
    {
        
        md->size = -md->size - sizeof(struct meta_data) + nmd->size;
        md->next = nmd->next;
        struct meta_data *nnmd = (struct meta_data*)nmd->next;
        if(nnmd)
            nnmd->prev = md;
            
        if(!nnmd)
        {
            if(md->size <= -PAGE_SIZE)  // Jeżeli ilość wolnej przestrzeni jest wystarczająca, zwracam ją do systemu
            { 
                custom_sbrk(-(-pmd->size / PAGE_SIZE) * PAGE_SIZE );
                md->size += (-pmd->size / PAGE_SIZE) * PAGE_SIZE ;
            }
                
        }      
            
        return;    
    }
    if(pmd && nmd) // Jeżeli poprzedni i następny blok są zajęte to zmieniam obecny blok na wolny
    {
        
        if(pmd->size > 0 && nmd->size > 0)
            md->size = -md->size;
        return;    
    }
    if(pmd && !nmd) // obecny blok jest ostatni na stosie, zmieniam go na wolny
    {
        
        md->size = -md->size;
       
        if(md->size <= -PAGE_SIZE) // Jeżeli ilość wolnej przestrzeni jest wystarczająca, zwracam ją do systemu
        {
             custom_sbrk(-(-pmd->size / PAGE_SIZE) * PAGE_SIZE );
             md->size += (-pmd->size / PAGE_SIZE) * PAGE_SIZE ;
        }
        return;
    }
    if(nmd && !pmd) // obecny blok jest na poczatku stosu, zmieniam go na wolny
    {
        
         md->size = -md->size;
        return;
    }
    
   
}
void* heap_calloc(size_t n,size_t size)
{
    void * ptr = heap_malloc(n * size);
    if(!ptr)
        return NULL;
    for(int i=0;i < n * size;i++)
    {
        char *temp  = (char*)(ptr + i);
        *temp = 0; 
    }   
    return ptr; 
}
void * heap_realloc(void *ptr,size_t size)
{
    if(!ptr)
    {
        void* result = heap_malloc(size);
        return result;
    }
    if(size == 0)
        return NULL;

    struct meta_data *md = (struct meta_data*)(ptr - sizeof(struct meta_data)); //Obecny blok
    struct meta_data *nmd = (struct meta_data*)(md->next); //następnyN blok
    if(size == md->size) //Nie ma żadnej zmiany
        return ptr;

    if(size < md->size) // Zmniejszamy blok jeżeli różnica nowego i starego rozmiaru jest znacząca ( >= rozmiar meta danych) To tworze pusty blok obok lub dopisuje size do następnego
    {
        if(nmd->size<=0) //Kolejny blok jest pusty więc dodaję do niego pozostałość pierwszego bloku
        {
            nmd->size += -(md->size - size);
            md->size = size;
            return ptr;  
        }
        if(md->size - size >= sizeof(struct meta_data)) // Tworze obok blok pusty z pozostałością rozmiaru pierwszego
        {
            int prev_size = md->size;
            md->size = size;
            void* tp = md->next;
            md->next = (void*)md + md->size + sizeof(struct meta_data);
            struct meta_data *md_temp = (struct meta_data*)md->next;
            md_temp->size = -(prev_size - size - sizeof(struct meta_data));
            md_temp->prev = md;
            md_temp->next = tp;
            struct meta_data *nnmd = (struct meta_data*)tp;
                if(nnmd)
                    nnmd->prev = md_temp;
            return ptr;
        }
    }
    
    
    if(nmd && nmd->size <= 0 && md->size + -nmd->size + sizeof(struct meta_data) >= size) //Sprawdzamy czy możemy powiększyć obecny blok o następny
    {
        if(md->size + -nmd->size + sizeof(struct meta_data) == size) // Łącze obecny i następny blok w jeden o porządanym rozmiarze
        {
            struct meta_data *nnmd = (struct meta_data*)nmd->next;
            md->next = nnmd;
            if(nnmd)
            {
                 nnmd->prev = md;
            }
            return ptr;
        }
        else if(md->size + -nmd->size >= size)// Zwiększam rozmiar obecnego bloku, a następny przesuwam
        {
                    
            int temp_size = nmd->size;
            void * temp_ptr = nmd->next;
            struct meta_data *nnmd = (struct meta_data*)nmd->next; // Wskaźnik na blok o 2 do przodu od obecnego
            struct meta_data *temp = (struct meta_data*)(md->next + size - md->size);//Przesuwam początek następnego bloku
            temp->next = temp_ptr;
            temp->size = temp_size + (size - md->size) ;
            md->size = size;
            md->next = temp;
                  
            if(nnmd)
            {
                nnmd->prev = temp;
            }
            return ptr;
        }
                
    }    
    //Nie da się zwiększyć bloku nie zmieniając położenia
    
    void * temp =heap_malloc(size);
    if(temp == NULL)
        return NULL;
    memcpy(temp,ptr,md->size);
    heap_free(ptr);    

    return temp;
}
size_t heap_get_used_space(void)
{
    struct meta_data *md = (struct meta_data*)start;
    size_t result = 0;
    while(md!=NULL)
    {
        result += sizeof(struct meta_data);
        if(md->size > 0)
            result += md->size;
        md = (struct meta_data*)md->next;
    }
    return result;
}
size_t heap_get_largest_used_block_size(void)
{
    struct meta_data *md = (struct meta_data*)start;
    int result = 0;
    while(md!=NULL)
    {
        if(md->size > result)
            result = md->size;
        md = (struct meta_data*)md->next;
    }
    return result;
}
uint64_t heap_get_used_blocks_count(void)
{
    struct meta_data *md = (struct meta_data*)start;
    int result = 0;
    while(md!=NULL)
    {
        if(md->size > 0)
            ++result;
        md = (struct meta_data*)md->next;
    }
    return result;
}
size_t heap_get_free_space(void)
{
    struct meta_data *md = (struct meta_data*)start;
    int result = 0;
    while(md!=NULL)
    {
        if(md->size < 0)
            result += -md->size;
        md = (struct meta_data*)md->next;
    }
    return result;
}
size_t heap_get_largest_free_area(void)
{
    struct meta_data *md = (struct meta_data*)start;
    int result = 0;
    if(md->next == NULL)
        return heap_get_free_space();
    while(md!=NULL)
    {
        if(-md->size > result)
            result = -md->size;
        md = (struct meta_data*)md->next;
    }
    return result;
}
uint64_t heap_get_free_gaps_count(void)
{
    struct meta_data *md = (struct meta_data*)start;
    int result = 0;
   
    while(md!=NULL)
    {
        if(-md->size > (int)(sizeof(void*) + 8))
            ++result;
        md = (struct meta_data*)md->next;
    }
    return result;
}
enum pointer_type_t get_pointer_type(const const void* pointer)
{
    if(pointer == NULL)
        return pointer_null;
    struct meta_data *md = (struct meta_data*)start;
    struct meta_data* end;
    if((uint64_t)md > (uint64_t) pointer)
        return pointer_out_of_heap;
    while(md!=NULL)
    {
        
        if((uint64_t)pointer >= (uint64_t)md && (uint64_t)pointer < ((uint64_t)md + sizeof(struct meta_data)))
            return pointer_control_block;
            
        if(md->size < 0 && (uint64_t)pointer >= ((uint64_t)md + sizeof(struct meta_data)) && (uint64_t)pointer < ((uint64_t)md + sizeof(struct meta_data) - md->size))
            return pointer_unallocated;
        if(md->size > 0 && (uint64_t)pointer > ((uint64_t)md + sizeof(struct meta_data)) && (uint64_t)pointer < ((uint64_t)md + sizeof(struct meta_data) + md->size))
            return pointer_inside_data_block;
    
        if(md->size > 0 && (uint64_t)pointer == ((uint64_t)md + sizeof(struct meta_data)))
            return pointer_valid; 
            
       
        end = md;
        md = (struct meta_data*)md->next;
    }
    if((uint64_t)pointer >= (uint64_t)end + abs(end->size) + sizeof(struct meta_data))
        return pointer_out_of_heap;
    return pointer_null;
}