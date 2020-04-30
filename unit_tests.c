#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "unit_tests.h"
#include "custom_unistd.h"
#include"my_malloc.h"

#define KB 1024
#define MB (1024 * 1024)
//malloc
int unit_test1(void)
{ //test sprawdzajacy alokacje 1 bloku
    TEST_START(1);
    void *ptr = heap_malloc(1);
    test_assert(ptr, 1, "Funkcja heap_malloc zwrociła NULL");
    heap_free(ptr);
    TEST_OK(1);
}
int unit_test2(void)
{ //test sprawdzajacy alokacje 2 blokow
    TEST_START(2);
    void *ptr = heap_malloc(1 * KB);
    test_assert(ptr, 2, "Funkcja heap_malloc zwrociła NULL");
    void *ptr2 = heap_malloc(3 * KB);
    test_assert(ptr2, 2, "Funkcja heap_malloc zwrociła NULL");
   // test_assert(!heap_validate(),5,"Funkcja heap_validate zwrocila -1");
    heap_free(ptr);
    heap_free(ptr2);
    TEST_OK(2);
}
int unit_test3(void)
{ //malloc poiwnien zwrocic NULL, brak pamieci
    TEST_START(3);
    void *ptr = heap_malloc(65 * MB);
    test_assert(ptr == NULL, 3, "Funkcja heap_malloc powinna zwrocic NULL");
    //test_assert(!heap_validate(),5,"Funkcja heap_validate zwrocila -1");
   
    TEST_OK(3);
}
int unit_test4(void)
{ //alokacja w przestrzeni miedzyblokowej-taki sam rozmiar
    TEST_START(4);
    void *ptr1 = heap_malloc(1 * MB);
    void *ptr2 = heap_malloc(10 * MB);
    void *ptr3 = heap_malloc(52 * MB);
    test_assert(ptr1, 4, "Funkcja heap_malloc zwrociła NULL");
    test_assert(ptr2, 4, "Funkcja heap_malloc zwrociła NULL");
    test_assert(ptr3, 4, "Funkcja heap_malloc zwrociła NULL");
    
    heap_free(ptr2);
   
    ptr2 = heap_malloc(10 * MB);
    test_assert(ptr2, 4, "Funkcja heap_malloc zwrociła NULL");
   
    //test_assert(!heap_validate(),4,"Funkcja heap_validate zwrocila -1");
 
    heap_free(ptr1);
    
    
    heap_free(ptr2);
   

    heap_free(ptr3);
     
    TEST_OK(4);
}
int unit_test5(void)
{ //alokacja w przestrzeni miedzyblokowej-mniejszy rozmiar
    TEST_START(5);
   
    void *ptr1 = heap_malloc(1 * MB);
    void *ptr2 = heap_malloc(10 * MB);
    void *ptr3 = heap_malloc(52 * MB);
 
    test_assert(ptr1, 5, "Funkcja heap_malloc zwrociła NULL");
    test_assert(ptr2, 5, "Funkcja heap_malloc zwrociła NULL");
    test_assert(ptr3, 5, "Funkcja heap_malloc zwrociła NULL");
   
    heap_free(ptr2);
  
    ptr2 = heap_malloc(5 * MB);

    void *ptr4 = heap_malloc(3 * MB);
 
    test_assert(ptr2, 5, "Funkcja heap_malloc zwrociła NULL");
    test_assert(ptr4, 5, "Funkcja heap_malloc zwrociła NULL");
    
    heap_free(ptr1);
    
    heap_free(ptr2);
     
    heap_free(ptr3);
  
    heap_free(ptr4);
    
    TEST_OK(5);
}
//calloc
int unit_test6(void)
{ //test sprawdzajacy poprawnosc funkcji calloc-zerowanie pamieci.
    TEST_START(6);
    
    void *ptr = heap_calloc(10, sizeof(char));
    test_assert(ptr, 6, "Funkcja heap_calloc zwrociła NULL");
    char test[10];
    memset(test, 0, 10 * sizeof(char));
    test_assert(!memcmp(ptr, test, 10 * sizeof(char)), 6, "Funkcja heap_calloc niepoprawnie nadpisala pamiec");
    memset(test, 1, 10 * sizeof(char));
    test_assert(memcmp(ptr, test, 10 * sizeof(char)), 6, "Funkcja heap_calloc niepoprawnie nadpisala pamiec");
    heap_free(ptr);
    TEST_OK(6);
}
int unit_test7(void)
{ //test sprawdzajacy odpornosc funkcji heap_calloc na brak pamieci
    TEST_START(7);
   
    void *ptr = heap_calloc(65 * MB, sizeof(char));
    test_assert(ptr == NULL, 7, "Funkcja heap_calloc powinna zwrocic NULL");
    TEST_OK(7);
}
//realloc
int unit_test8(void)
{ //zwiekszanie 1 bloku, sterta pusta
    TEST_START(8);
   
    void *ptr = heap_malloc(8);
    test_assert(ptr, 8, "Funkcja heap_malloc zwrociła NULL");
    void *ptr2 = heap_realloc(ptr, 16);
    test_assert(ptr2, 8, "Funkcja heap_realloc zwrociła NULL");
    heap_free(ptr2);
    TEST_OK(8);
}
int unit_test9(void)
{ //zmniejszanie bloku, nastepny blok jest zajety
    TEST_START(9);

    void *ptr = heap_malloc(10 * MB);
    test_assert(ptr, 9, "Funkcja heap_malloc zwrociła NULL");

    void *ptr3 = heap_malloc(53 * MB);
    test_assert(ptr3, 9, "Funkcja heap_malloc zwrociła NULL");

    void *ptr2 = heap_realloc(ptr, 5 * MB);
    test_assert(ptr2, 9, "Funkcja heap_realloc zwrociła NULL");
    heap_free(ptr2);
    heap_free(ptr3);

    TEST_OK(9);
}
int unit_test10(void)
{ //zmniejszanie bloku, nastepny blok jest wolny
    TEST_START(10);
    
    void *ptr = heap_malloc(10 * MB);
    test_assert(ptr, 10, "Funkcja heap_malloc zwrociła NULL");

    void *ptr2 = heap_malloc(5 * MB);
    test_assert(ptr2, 10, "Funkcja heap_malloc zwrociła NULL");

    void *ptr3 = heap_malloc(48 * MB);
    test_assert(ptr3, 10, "Funkcja heap_malloc zwrociła NULL");
    heap_free(ptr2);
   
    void *ptr4 = heap_realloc(ptr, 6 * MB);
    test_assert(ptr2, 10, "Funkcja heap_realloc zwrociła NULL");
   

     
    heap_free(ptr4);
    heap_free(ptr3);
   

    TEST_OK(10);
}
int unit_test11(void)
{ //zwiekszanie bloku, za malo pamieci
    TEST_START(11);
    
    void *ptr = heap_malloc(1*MB);
    test_assert(ptr, 11, "Funkcja heap_malloc zwrociła NULL");

    void*ptr2=heap_malloc(62*MB);
    test_assert(ptr2, 11, "Funkcja heap_malloc zwrociła NULL");

    void *ptr3 = heap_realloc(ptr, 5*MB);
    test_assert(!ptr3, 11, "Funkcja heap_realloc powinna zwrocic NULL");
    test_assert(ptr, 11, "Wskaznik przed reaolokacja powinien pozostac niezmieniony");

    heap_free(ptr);
    heap_free(ptr2);
    TEST_OK(11);
}

int main()
{
    heap_setup(PAGE_SIZE);
    printf("Sprawdzanie poprawnosci funkcji \033[1mheap_malloc\033[0m oraz \033[1mheap_free\033[0m\n");
    unit_test1();
    unit_test2();
    unit_test3();
    unit_test4();
    unit_test5();
    printf("Sprawdzanie poprawnosci funkcji \033[1mheap_calloc\033[0m oraz \033[1mheap_free\033[0m\n");
    unit_test6();
    unit_test7();
    printf("Sprawdzanie poprawnosci funkcji \033[1mheap_realloc\033[0m oraz \033[1mheap_free\033[0m\n");
    unit_test8();
    unit_test9();
    unit_test10();
    unit_test11();

    //print_heap_info();m
    return 0;
}