#include <assert.h>

#include "ListOnPtrs.h"
#include "Log.h"

static const int POISON = 0xDEAD; 

static inline ListErrors ListElemDtor(ListElemType* elem);
static inline ListErrors ListElemCtor(ListElemType** elem);
static inline ListErrors ListElemInit(ListElemType* elem, 
                                      const int value,
                                      ListElemType* prev, ListElemType* next);

//------Graphic dump funcs-------
static inline void CreateMainNodeDotFile(FILE* outDotFile,
                                         const size_t id, const int value, 
                                         const size_t next, const size_t prev);

static void CreateMainNodesDotFile      (FILE* outDotFile, const ListType* list);
static void CreateMainEdgesDotFile      (FILE* outDotFile, const ListType* list);
static void CreateAuxiliaryInfoDotFile (FILE* outDotFile, const ListType* list);
static void CreateFictiousEdgesDotFile  (FILE* outDotFile, const ListType* list);

static inline void CreateImgInLogFile(const size_t imgIndex);
static inline void BeginDotFile(FILE* outDotFile);
static inline void EndDotFile  (FILE* outDotFile);


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

    ListElemType* listBlock = ListGetHead(list);
    ListElemType* listTail  = ListGetTail(list);

    while (listBlock != listTail)
    {
        ListElemType* blockToErase = listBlock;

        listBlock = listBlock->next;

        ListElemDtor(blockToErase);
    }

    ListElemDtor(list->end);
    list->end = nullptr;

    return ListErrors::NO_ERR;
}

ListErrors ListVerify(ListType* list)
{
    assert(list);
    
    if (list->end == nullptr)
        return ListErrors::INVALID_FICTIOUS_ELEMENT;

    for (ListElemType* listElem = ListGetHead(list); listElem != list->end; 
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

void ListTextDump(const ListType* list, const char* fileName,
                                        const char* funcName,
                                        const int   line)
{
    assert(list);
    assert(fileName);
    assert(funcName);
    assert(line);

    LogBegin(fileName, funcName, line);

    Log("Head - %p\n", ListGetHead(list));
    Log("Tail - %p\n", ListGetTail(list));
    Log("End  - %p\n", list->end);
    Log("Size: %zu\n", list->size);

    Log("Fictious element(list->end)[%p] - value: %d, next: %p, prev: %p\n", 
         list->end, list->end->value, list->end->next, list->end->prev);

    size_t elemId = 1;
    for (ListElemType* listElem = ListGetHead(list); listElem != list->end; listElem = listElem->next)
    {
        Log("Element[%p] id: %zu, value: %d, next: %p, prev: %p\n",
             listElem, elemId, listElem->value, listElem->next, listElem->prev);
        ++elemId;
    }

    LOG_END();
}

static inline void CreateMainNodeDotFile(FILE* outDotFile,
                                     const size_t id, const int value, 
                                     const size_t next, const size_t prev)
{
    assert(outDotFile);

    fprintf(outDotFile, "node%zu"
                        "[shape=Mrecord, style=filled, fillcolor=\"#7293ba\","
                        "label=\" id: %zu  |"
                              "value: %d   |" 
                          "<f0> next: %zu  |"
                          "<f1> prev: %zu\","
                        "color = \"#008080\"];\n",
                        id, id,
                        value, 
                        next, prev); 
}

static void CreateMainNodesDotFile(FILE* outDotFile, const ListType* list)
{
    assert(outDotFile);
    assert(list);

    //---------Fictious element node---------

    CreateMainNodeDotFile(outDotFile, 0, list->end->value, 1, list->size); 
    size_t elemId = 1;

    //-----Other elements nodes-----

    ListElemType* listTail = ListGetTail(list);
    for (ListElemType* listElem = ListGetHead(list); listElem != listTail; listElem = listElem->next)
    {
        CreateMainNodeDotFile(outDotFile, elemId, listElem->value, elemId + 1, elemId - 1); 
        elemId++;     
    }  

    //--------Tail element node----------

    CreateMainNodeDotFile(outDotFile, elemId, listTail->value, 0, elemId - 1);   
}

static void CreateFictiousEdgesDotFile(FILE* outDotFile, const ListType* list)
{
    fprintf(outDotFile, "\nnode0");
    for (size_t i = 1; i <= list->size; ++i)
        fprintf(outDotFile, "->node%zu", i);
    fprintf(outDotFile, "[color=\"#31353b\", weight = 1, fontcolor=\"blue\",fontsize=78];\n");
}

static void CreateMainEdgesDotFile(FILE* outDotFile, const ListType* list)
{
    fprintf(outDotFile, "edge[color=\"red\", fontsize=12, constraint=false];\n");
    for (size_t i = 0; i < list->size; ++i)
    {
        fprintf(outDotFile, "node%zu->node%zu;\n", i, i + 1);
    }
    fprintf(outDotFile, "node%zu->node%zu;\n", list->size, 0lu);
}

static void CreateAuxiliaryInfoDotFile(FILE* outDotFile, const ListType* list)
{
    fprintf(outDotFile, "node[shape = octagon, style = \"filled\", fillcolor = \"lightgray\"];\n");
    fprintf(outDotFile, "edge[color = \"lightgreen\"];\n");
    fprintf(outDotFile, "head->node%zu;\n", 1lu);
    fprintf(outDotFile, "tail->node%zu;\n", list->size);
    fprintf(outDotFile, "\"Fictious element\"->node%zu;\n", 0lu);
}

static inline void CreateImgInLogFile(const size_t imgIndex)
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

static inline void BeginDotFile(FILE* outDotFile)
{
    fprintf(outDotFile, "digraph G{\nrankdir=LR;\ngraph [bgcolor=\"#31353b\"];\n");
}

static inline void EndDotFile(FILE* outDotFile)
{
    fprintf(outDotFile, "}\n");
}


void ListGraphicDump(const ListType* list)
{
    assert(list);

    static const char* dotFileName = "list.dot";
    FILE* outDotFile = fopen(dotFileName, "w");

    if (outDotFile == nullptr)
        return;
    
    BeginDotFile(outDotFile);

    CreateMainNodesDotFile(outDotFile, list);
    CreateFictiousEdgesDotFile(outDotFile, list);
    CreateMainEdgesDotFile(outDotFile, list);
    CreateAuxiliaryInfoDotFile(outDotFile, list);

    EndDotFile(outDotFile);

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

    ListElemType* insertedElement = nullptr;

    ListErrors error = ListElemCtor(&insertedElement);

    if (error != ListErrors::NO_ERR)
        return error;

    *insertedValPtr = insertedElement;

    ListElemInit(insertedElement, value, anchor->prev, anchor);
    anchor->prev->next = insertedElement;
    anchor->prev       = insertedElement;

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

ListElemType* ListGetHead(const ListType* list)
{
    assert(list);
    assert(list->end);

    return list->end->next;
}

ListElemType* ListGetTail(const ListType* list)
{
    assert(list);
    assert(list->end);

    return list->end->prev;
}

