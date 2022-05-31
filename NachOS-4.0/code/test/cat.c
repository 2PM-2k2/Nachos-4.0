#include "syscall.h"

#define MODE_READWRITE 0
#define MODE_READ 1

#define MAX_LENGTH 32 // do dai toi da cho ten file

int main() {
    char buffer[100], source[MAX_LENGTH];

    Write("---------------CAT---------------\n",100,1);
    PrintString("Enter source file's name: ");
	ReadString(source, MAX_LENGTH); // Goi ham ReadString de doc vao ten file nguon
    PrintString("\nYour source file's name: ");
    PrintString(source);
    PrintString("\n\n");

    Open(source,1); // mo file de doc
    Read(buffer, 50, 2);

    PrintString("\nYour string: ");
    PrintString(buffer);
    PrintString("\n\n");

    //Halt();
	return 0;
}
