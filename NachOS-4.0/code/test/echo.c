#include "syscall.h"
#include "copyright.h"
#define MAX_LENGTH 255


int main()
{
	char buffer[MAX_LENGTH];
	Write("---------------ECHO---------------\n",50,1);
	Write("\n(use ctrl+D to end typing)\n",50,1);
	Write("Enter string: ",20,1);
	Read(buffer,MAX_LENGTH, 0); // nhap chuoi tu console

	Write("\n--->Output: ",30,1);
	Write(buffer,MAX_LENGTH,1); // xuat ra chuoi vua nhap ra console
	Write("\n\n",10,1);

	//Halt();
	return 0;
}
