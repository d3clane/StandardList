#ifndef LIST_ON_PTRS_H
#define LIST_ON_PTRS_H

#include <stdio.h>

struct ListElemType
{
    int value;

    ListElemType* next;
    ListElemType* prev;
};

struct ListType
{
    size_t size;

    ListElemType* end;
};

enum class ListErrors
{
    NO_ERR,

    MEMORY_ERR,

    INVALID_NEXT_PTR,
    INVALID_PREV_PTR,
    
    INVALID_VALUE,
    INVALID_ELEMENT_PTR,
    
    INVALID_FICTIOUS_ELEMENT,
};


ListErrors ListCtor  (ListType* list);
ListErrors ListDtor  (ListType* list);
ListErrors ListVerify(ListType* list);

#define LIST_DUMP(list) ListDump((list), __FILE__, __func__, __LINE__)
void ListDump(ListType* list, const char* fileName, 
                              const char* funcName, 
                              const int   line);

#define LIST_TEXT_DUMP(list) ListTextDump((list), __FILE__, __func__, __LINE__)
void ListTextDump(const ListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line);

void ListGraphicDump(const ListType* list);

ListErrors ListInsert(ListType* list, ListElemType* anchor, const int value,
                                                            ListElemType** insertedValPtr);
ListErrors ListErase (ListType* list, ListElemType* anchor);

ListElemType* ListGetHead(const ListType* list);
ListElemType* ListGetTail(const ListType* list);


#endif