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

};


ListErrors ListCtor  (ListType* list);
ListErrors ListDtor  (ListType* list);
ListErrors ListVerify(ListType* list);

#define LIST_DUMP(list) ListDump((list), __FILE__, __func__, __LINE__)
void ListDump(ListType* list, const char* fileName, 
                              const char* funcName, 
                              const int   line);

#define LIST_TEXT_DUMP(list) ListTextDump((list), __FILE__, __func__, __LINE__)
void ListTextDump(ListType* list, const char* fileName,
                                  const char* funcName,
                                  const int   line);

void ListGraphicDump(ListType* list);

ListErrors ListInsert(ListType* list, ListElemType* anchor, const int value,
                                                            ListElemType** insertedValPtr);
ListErrors ListErase (ListType* list, ListElemType* anchor);

ListElemType* GetListHead(ListType* list);
ListElemType* GetListTail(ListType* list);


#endif