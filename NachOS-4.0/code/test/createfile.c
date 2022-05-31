#include "syscall.h"
// #include <string.h>

int main() {
    char fileName[80];
    int length=50;

    Write("---------------CREATE FILE---------------\n",100,1);

    Write("\n(use ctrl+D to end typing)\n",50,1);
    Write("Enter file's name: ",50,1);
    Read(fileName, length,0); // doc ten file tu console

    if (fileName[0]=='\0'){ // tao file voi ten mac dinh "newfile.txt"
        if (CreateFile("newfile.txt") == 0) {
            Write("\nFile ",10,1);
            Write("newfile.txt",20,1);
            Write(" created successfully!\n\n",50,1);
        } else
            Write("Create file failed\n\n",30,1);
    }

    else if (CreateFile(fileName) == 0) { // tao file voi ten vua nhap
        Write("\nFile ",10,1);
        Write(fileName,length,1);
        Write(" created successfully!\n\n",50,1);
    } else
        Write("Create file failed\n\n",30,1);

    //Halt();
    return 0;
}

