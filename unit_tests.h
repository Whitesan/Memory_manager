#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__
#define __ITERATOR_TO_NUMBER(i) i

#define TEST_OK(test_number)                                                \
    printf("\033[32m\tTEST %d zakonczony pomyslnie\033[0m\n", test_number); \
    return 0;
#define TEST_ERROR(test_number)                                  \
    printf("\033[31m\tTEST %d przerwany\033[0m\n", test_number); \
    exit(test_number);
#define TEST_START(test_number) printf("\033[1m\t### TEST %d\n\033[0m", test_number);

#define test_assert(__bool, __test_number, __err) \
    if (!(__bool))                                  \
    {                                             \
        printf("%s\n", __err);                    \
        TEST_ERROR(__test_number)                 \
    }

#endif