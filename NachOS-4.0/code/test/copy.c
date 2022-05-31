#include "syscall.h"
#define MAX_LENGTH 32 // do dai toi da cho ten file

int main() {
    int length=50;
    int len = 0;
    char buffer[100], source[MAX_LENGTH], dest[MAX_LENGTH]; 

    PrintString("---------------COPY---------------\n");
    PrintString("Enter source file's name: ");
	ReadString(source, MAX_LENGTH); // Goi ham ReadString de doc vao ten file nguon
    PrintString("\nYour source file's name: ");
    PrintString(source);
    PrintString("\n\n");

	PrintString("Enter destination file's name: ");
	ReadString(dest, MAX_LENGTH); // Goi ham ReadString de doc vao ten file dich
    PrintString("\nYour destination file's name: ");
    PrintString(dest);
    PrintString("\n");
    

    Open(source,1); // mo file only read
    Open(dest,0); // mo file 
    
    Read(buffer, 50, 2);
    while (buffer[len] != '\0') ++len;
    Write(buffer, len, 3);


    PrintString("\nCopy successfully\n");   

    //Halt();
	return 0;
}