#include "synch.h"
#include "ptable.h"

PTable::PTable(int size) {
    if (size < 0)
        return;

    psize = size;
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem",1);

    for (int i = 0; i < size; i++) {
        pcb[i] = 0;
    }


    pcb[0] = new PCB(0);
    pcb[0]->SetFileName("./test/scheduler");
    pcb[0]->parentID = -1;
}

PTable::~PTable() {
    if( bm != 0 )
	    delete bm;

    for (int i = 0; i < psize; i++) {
        if (pcb[i] != 0) delete pcb[i];
    }

    if( bmsem != 0)
		delete bmsem;
}

int PTable::ExecUpdate(char* name) {
    // Gọi mutex->P(); để giúp tránh tình trạng nạp 2 tiến trình cùng 1 lúc.
    bmsem->P();

    // Kiểm tra tính hợp lệ của chương trình “name”.
    // Kiểm tra sự tồn tại của chương trình “name” bằng cách gọi phương thức
    // Open của lớp fileSystem.
    if (name == NULL) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute name is NULL.\n");
        bmsem->V();
        return -1;
    }
    // So sánh tên chương trình và tên của currentThread để chắc chắn rằng
    // chương trình này không gọi thực thi chính nó.
    if (strcmp(name, "./test/scheduler") == 0 || strcmp(name, kernel->currentThread->getName()) == 0) {
        DEBUG(dbgSys, "\nPTable::Exec : Can't not execute itself.\n");
        bmsem->V();
        return -1;
    }

    // Tìm slot trống trong bảng Ptable.
    int index = this->GetFreeSlot();

    // Check if have free slot
    if (index < 0) {
        DEBUG(dbgSys, "\nPTable::Exec :There is no free slot.\n");
        bmsem->V();
        return -1;
    }

    // Nếu có slot trống thì khởi tạo một PCB mới với processID chính là index
    // của slot này
    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);

    // parrentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;

    // Gọi thực thi phương thức Exec của lớp PCB.
    int pid = pcb[index]->Exec(name, index);

    // Gọi bmsem->V()
    bmsem->V();
    // Trả về kết quả thực thi của PCB->Exec.
    cout << "\nFilename: " << name << " - pid: " << pid << endl;
    return pid;
}

int PTable::ExitUpdate(int exitcode) {
    // Nếu tiến trình gọi là main process thì gọi Halt().
    int id = kernel->currentThread->processID;
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
        return 0;
    }

    if (!IsExist(id)) {
        DEBUG(dbgSys, "Process " << id << " is not exist.");
        return -1;
    }

    // Ngược lại gọi SetExitCode để đặt exitcode cho tiến trình gọi.
    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();

    // Gọi JoinRelease để giải phóng tiến trình cha đang đợi nó (nếu có)
    // và ExitWait() để xin tiến trình cha cho phép thoát.
    pcb[id]->JoinRelease();
    pcb[id]->ExitWait();

    Remove(id);
    return exitcode;
}

int PTable::JoinUpdate(int id) {
    // Ta kiểm tra tính hợp lệ của processID id và kiểm tra tiến trình gọi Join
    // có phải là cha của tiến trình có processID là id hay không. Nếu không
    // thỏa, ta báo lỗi hợp lý và trả về -1.
    cout << "\nJoin id: " << id << endl;

    if (id < 0) {
        DEBUG(dbgSys, "\nPTable::Join : Can't not join.\n");
        cout << "\nPTable::Join : Can't not join.\n";
        return -1;
    }

    if (pcb[id]->parentID != kernel->currentThread->processID) {
        DEBUG(dbgSys, "\nPTable::Join : Can't not join in process which is not it's parent process.\n");
        cout << "\nPTable::Join : Can't not join in process which is not it's parent process.\n";
        return -1;
    }

    // Tăng numwait và gọi JoinWait() để chờ tiến trình con thực hiện.
    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();

    // Sau khi tiến trình con thực hiện xong, tiến trình đã được giải phóng

    // Xử lý exitcode.
    int exit_code = pcb[id]->GetExitCode();
    // ExitRelease() để cho phép tiến trình con thoát.
    pcb[id]->ExitRelease();
    cout << "\nExit code : " << exit_code << endl;
    return exit_code;
}

// Find free slot in order to save the new process infom
int PTable::GetFreeSlot() {
    bm->FindAndSet();
}

// Check if Process ID is Exist
bool PTable::IsExist(int pid) { return bm->Test(pid); }

// Remove proccess ID out of table
// When it ends
void PTable::Remove(int pid) {
    bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

char* PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }