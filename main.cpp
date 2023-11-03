//#include "List.h"
#include "Log.h"
#include "ListOnPtrs.h"

#include <stdlib.h>

int main(const int argc, const char* argv[])
{
    LogOpen(argv[0]);

    ListType list;
    ListCtor(&list);

    ListElemType* lastPos = nullptr;

    //LIST_TEXT_DUMP(&list);
    //printf("HERE1\n");
    //printf("next free blockAA: %d\n", list.data[list.freeBlockHead].nextPos);

    ListInsert(&list, list.end, 78, &lastPos);

    //LIST_TEXT_DUMP(&list);
    
    //printf("lastPos: %d\n", lastPos);
    //printf("next free blockAB: %d\n", list.data[list.freeBlockHead].nextPos);
    ListElemType* newLastPos = nullptr;
    ListInsert(&list, lastPos, 123, &newLastPos);
    
    LIST_DUMP(&list);
    
    //printf("lastPos: %d\n", lastPos);

    LIST_DUMP(&list);
    
    ListInsert(&list, lastPos, 1488, &newLastPos);

    //printf("lastPos: %d\n", lastPos);
    LIST_DUMP(&list);

    ListErase(&list, newLastPos);
    ListInsert(&list, lastPos, 1111, &lastPos);

    //printf("lastPos: %d\n", lastPos);

    ListInsert(&list, list.end, 9999, &newLastPos);

    //printf("lastPos: %d\n", lastPos);

    LIST_DUMP(&list);
    //LIST_TEXT_DUMP(&list);

    ListInsert(&list, lastPos, 123, &lastPos);

    //printf("lastPos: %d\n", lastPos);

    //ListErase(&list, newLastPos);

    //ListRebuild(&list);
    LIST_DUMP(&list);
    

    ListDtor(&list);
}