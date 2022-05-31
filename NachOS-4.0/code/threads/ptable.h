#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"

#define MAX_PROCESSES 10

class PTable {
   private:
    Bitmap* bm;
    PCB* pcb[MAX_PROCESSES];
    int psize;
    Semaphore* bmsem;

   public:
    PTable(int size);
    ~PTable();
    int ExecUpdate(char* name);
    int ExitUpdate(int ec);
    int JoinUpdate(int id);
    int GetFreeSlot();
    bool IsExist(int pid);
    void Remove(int pid);
    char* GetFileName(int id);
};

#endif