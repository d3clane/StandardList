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

    ListInsert(&list, list.end, 78, &lastPos);

    ListElemType* newLastPos = nullptr;
    ListInsert(&list, lastPos, 123, &newLastPos);
    
    LIST_DUMP(&list);
    
    LIST_DUMP(&list);
    
    ListInsert(&list, lastPos, 1488, &newLastPos);

    LIST_DUMP(&list);

    ListErase(&list, newLastPos);
    ListInsert(&list, lastPos, 1111, &lastPos);

    ListInsert(&list, list.end, 9999, &newLastPos);

    LIST_DUMP(&list);

    ListInsert(&list, lastPos, 123, &lastPos);

    LIST_DUMP(&list);

    ListDtor(&list);
}