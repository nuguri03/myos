#ifndef SELFTEST_H
#define SELFTEST_H

#include "types.h"

// heap
void test_heap();
void test_heap_overlap();

// paging
void test_paging();
void test_page_fault();

// standard library
void test_kmemset();
void test_kmemcpy();
void test_kmemmove();
void test_kstrcmp();

// print kernel
void test_kprintf();

#endif