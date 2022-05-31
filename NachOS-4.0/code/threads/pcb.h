#ifndef PCB_H
#define PCB_H

class PCB {
   private:
    Semaphore* joinsem;         // semaphore cho quá trình join
    Semaphore* exitsem;         // semaphore cho quá trình exit
    Semaphore* multex;          // semaphore cho quá trình truy xuất đọc quyền

    int exitcode;
    int numwait;                // số tiến trình đã join

    Thread *thread;             // Tien trinh cua chuong trinh
    char filename[128];         // Ten cua tien trinh

   public:
    int parentID;               // ID cua tien trinh cha
    //int processID;
    char boolBG;                // Kiem tra neu la tien trinh nen

    PCB(int = 0);
    ~PCB();

    int Exec(char *filename, int pid);  // Tao mot thread moi
    int GetID();                // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();           // Trả về số lượng tiến trình chờ

    void JoinWait();            // Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait();            // Tiến trình con kết thúc

    void JoinRelease();         // Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();         // Cho phép tiến trình con kết thúc

    void IncNumWait();          // Tăng số tiến trình chờ
    void DecNumWait();          // Giảm số tiến trình chờ

    void SetExitCode(int ec);   // Đặt exitcode của tiến trình
    int GetExitCode();          // Trả về exitcode

    void SetFileName(char *fn); // Set ten tien trinh
    char *GetFileName();        // Tra ve ten tien trinh
};

#endif