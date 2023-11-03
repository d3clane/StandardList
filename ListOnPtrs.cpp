#include <assert.h>

#include "ListOnPtrs.h"
#include "Log.h"

static const int POISON = 0xDEAD; 

static inline ListErrors ListElemDtor(ListElemType* elem);
static inline ListErrors ListElemCtor(ListElemType** elem);
static inline ListErrors ListElemInit(ListElemType* elem, 
                                      const int value,
                                      ListElemType* prev, ListElemType* next);

static inline void CreateNode(FILE* outDotFile,
                              const size_t id, const int value, 
                              const size_t next, const size_t prev);

static void inline CreateImgInLogFile(const size_t imgIndex);

#define LIST_CHECK(list)                                \
do                                                      \
{                                                       \
    ListErrors listVerifyErr = ListVerify((list));      \
                                                        \
    if (listVerifyErr != ListErrors::NO_ERR)            \
        return listVerifyErr;                           \
} while (0)


ListErrors ListCtor(ListType* list)
{
    assert(list);

    ListElemType* fictiousElement = nullptr;
    ListErrors error = ListElemCtor(&fictiousElement);
    
    if (error != ListErrors::NO_ERR)
        return error;

    list->end  = fictiousElement;
    list->size = 0;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

ListErrors ListDtor(ListType* list)
{
    assert(list);

    ListElemType* listBlock = GetListHead(list);
    ListElemType* listTail  = GetListTail(list);

    while (listBlock != listTail)
    {
        ListElemType* blockToErase = listBlock;

        listBlock = listBlock->next;

        ListElemDtor(blockToErase);
    }

    //delete fictious element
    ListElemDtor(list->end);
    list->end = nullptr;

    return ListErrors::NO_ERR;
}

ListErrors ListVerify(ListType* list)
{
    assert(list);
    
    if (list->end == nullptr)
        return ListErrors::INVALID_FICTIOUS_ELEMENT;

    for (ListElemType* listElem = GetListHead(list); listElem != list->end; 
                                                     listElem  = listElem->next)
    {
        if (listElem->next == listElem)
            return ListErrors::INVALID_NEXT_PTR;
        if (listElem->prev == listElem)
            return ListErrors::INVALID_PREV_PTR;

        if (listElem->value == POISON)
            return ListErrors::INVALID_VALUE;
        
        if (listElem == list->end)
            return ListErrors::INVALID_ELEMENT_PTR;
    }

    if (list->end->value != POISON)
        return ListErrors::INVALID_FICTIOUS_ELEMENT;

    return ListErrors::NO_ERR;
}

void ListDump(ListType* list, const char* fileName, 
                              const char* funcName, 
                              const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);

    ListTextDump(list, fileName, funcName, line);

    ListGraphicDump(list);
}

void ListTextDump(ListType* list, const char* fileName,
                                  const char* funcName,
                                  const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    assert(line);

    LogBegin(fileName, funcName, line);

    Log("Head - %p\n", GetListHead(list));
    Log("Tail - %p\n", GetListTail(list));
    Log("End  - %p\n", list->end);
    Log("Size: %zu\n", list->size);

    Log("Fictious element(list->end)[%p] - value: %d, next: %p, prev: %p\n", 
         list->end, list->end->value, list->end->next, list->end->prev);

    size_t elemId = 1;
    for (ListElemType* listElem = GetListHead(list); listElem != list->end; listElem = listElem->next)
    {
        Log("Element[%p] id: %zu, value: %d, next: %p, prev: %p\n",
             listElem, elemId, listElem->value, listElem->next, listElem->prev);
        ++elemId;
    }

    LOG_END();
}

static inline void CreateNode(FILE* outDotFile,
                              const size_t id, const int value, 
                              const size_t next, const size_t prev)
{
    assert(outDotFile);

    fprintf(outDotFile, "node%zu"
                        "[shape=Mrecord, style=filled, fillcolor=\"#7293ba\","
                        "label=\" id: %zu |"
                              "value: %d  |" 
                          "<f0> next: %zu |"
                          "<f1> prev: %zu\","
                        "color = \"#008080\"];\n",
                        id, id,
                        value, 
                        next, prev); 
}

static void inline CreateImgInLogFile(const size_t imgIndex)
{
    static const size_t maxImgNameLength  = 64;
    static char imgName[maxImgNameLength] = "";
    snprintf(imgName, maxImgNameLength, "imgs/img_%zu_time_%s.png", imgIndex, __TIME__);

    static const size_t     maxCommandLength  = 128;
    static char commandName[maxCommandLength] = "";
    snprintf(commandName, maxCommandLength, "dot list.dot -T png -o %s", imgName);

    system(commandName);

    snprintf(commandName, maxCommandLength, "<img src = \"%s\">", imgName);    
    Log(commandName);
}

void ListGraphicDump(ListType* list)
{
    assert(list);

    static const char* tmpDotFileName = "list.dot";
    FILE* outDotFile = fopen(tmpDotFileName, "w");

    fprintf(outDotFile, "digraph G{\nrankdir=LR;\n"
                        "node[shape=rectangle, color=\"red\",fontsize=14];"
                        "\ngraph [bgcolor=\"#31353b\"];\n");

    //---------Fictious element node---------

    CreateNode(outDotFile, 0, list->end->value, 1, list->size); 
    size_t elemId = 1;

    //-----Other elements nodes-----

    ListElemType* listTail = GetListTail(list);
    for (ListElemType* listElem = GetListHead(list); listElem != listTail; listElem = listElem->next)
    {
        CreateNode(outDotFile, elemId, listElem->value, elemId + 1, elemId - 1); 
        elemId++;     
    }  

    //--------Tail element node----------

    CreateNode(outDotFile, elemId, listTail->value, 0, elemId - 1);

    //--------Edges--------

    fprintf(outDotFile, "edge[color=\"#31353b\", weight = 1, fontcolor=\"blue\",fontsize=78];\n");

    static const size_t numberOfWhiteArrows = 10;
    for (size_t whiteArrowId = 0; whiteArrowId < numberOfWhiteArrows; ++whiteArrowId)
    {
        fprintf(outDotFile, "node0");

        for (size_t i = 1; i <= list->size; ++i)
            fprintf(outDotFile, "->node%zu", i);

        fprintf(outDotFile, ";\n");
    }

    fprintf(outDotFile, "edge[color=\"red\", fontsize=12, constraint=false];\n");

    for (size_t i = 0; i < list->size; ++i)
    {
        fprintf(outDotFile, "node%zu->node%zu;\n", i, i + 1);
    }

    fprintf(outDotFile, "node%zu->node%zu;\n", list->size, 0lu);

    fprintf(outDotFile, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(outDotFile, "edge[color = \"lightgreen\"];\n");
    fprintf(outDotFile, "head->node%zu;\n", 1lu);
    fprintf(outDotFile, "tail->node%zu;\n", list->size);
    fprintf(outDotFile, "\"Fictious element\"->node%zu;\n", 0lu);

    fprintf(outDotFile, "}\n");

    fclose(outDotFile);

    static size_t imgIndex = 0;

    CreateImgInLogFile(imgIndex);
    imgIndex++;    
}

ListErrors ListInsert(ListType* list, ListElemType* anchor, const int value,
                                                            ListElemType** insertedValPtr)
{
    assert(list);
    assert(anchor);
    assert(insertedValPtr);

    LIST_CHECK(list);

    ListElemType* elemToInsert = nullptr;
    ListErrors error = ListElemCtor(&elemToInsert);

    if (error != ListErrors::NO_ERR)
        return error;

    *insertedValPtr = elemToInsert;
    //проверить порядок
    ListElemInit(elemToInsert, value, anchor->prev, anchor);
    anchor->prev->next = elemToInsert;
    anchor->prev       = elemToInsert;

    list->size++;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

ListErrors ListErase (ListType* list, ListElemType* anchor)
{
    assert(list);
    assert(anchor);

    LIST_CHECK(list);

    anchor->prev->next = anchor->next;
    anchor->next->prev = anchor->prev;

    ListElemInit(anchor, POISON, nullptr, nullptr);
    ListElemDtor(anchor);

    list->size--;

    LIST_CHECK(list);

    return ListErrors::NO_ERR;
}

static inline ListErrors ListElemDtor(ListElemType* elem)
{
    assert(elem);

    elem->next  = elem->prev = nullptr;
    elem->value = POISON;

    free(elem);

    return ListErrors::NO_ERR;
}

static inline ListErrors ListElemCtor(ListElemType** elem)
{
    assert(elem);

    *elem = (ListElemType*)calloc(1, sizeof(**elem));

    if (elem == nullptr)
        return ListErrors::MEMORY_ERR;

    (*elem)->value = POISON;
    (*elem)->next  = *elem;
    (*elem)->prev  = *elem;

    return ListErrors::NO_ERR;
}

static inline ListErrors ListElemInit(ListElemType* elem, 
                                      const int value,
                                      ListElemType* prev, ListElemType* next)
{
    assert(elem);

    elem->value = value;
    elem->prev  = prev;
    elem->next  = next;

    return ListErrors::NO_ERR;
}

ListElemType* GetListHead(ListType* list)
{
    assert(list);
    assert(list->end);

    return list->end->next;
}

ListElemType* GetListTail(ListType* list)
{
    assert(list);
    assert(list->end);

    return list->end->prev;
}

