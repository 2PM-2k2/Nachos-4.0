#include "syscall.h"

int
main()
{
    PrintString("---------------DELETE---------------\n");

    CreateFile("d1.txt");
    CreateFile("d2.txt");
    CreateFile("d3.txt");

    Open("d1.txt", 0);
    Delete("d1.txt");

    Open("d2.txt", 0);
    Delete("d2.txt");
    Close(3);
    Delete("d2.txt");

    // Close(2);
    // Delete("d1.txt");

    //Halt();
    return 0;
} 