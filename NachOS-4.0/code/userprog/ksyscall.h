/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include "sysdep.h"
#include <string.h>
#include <string>

void SysHalt(){  kernel->interrupt->Halt();}


int SysAdd(int op1, int op2){  return op1 + op2;}

int SysSub(int op1, int op2){  return op1 - op2;}

bool SysCreate(char* filename) {
  if (strlen(filename) == 0)
	{
		cout << "\nFile name is not valid!\n";
		DEBUG('a', "\n File name is not valid!\n");
		kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
		return false;
	}
			
	if (filename == NULL)  //Neu khong doc duoc
	{
		cout << "\nNot enough memory in system.\n";
		DEBUG('a', "\n Not enough memory in system.\n");
		kernel->machine->WriteRegister(2, -1); //Return -1 vao thanh ghi R2
		return false;
	}
	DEBUG('a', "\n Finish reading filename.");
			
	if (!kernel->fileSystem->Create(filename)) //Tao file bang ham Create cua fileSystem, tra ve ket qua
	{
		//Tao file that bai
		cout << "\nCan not create file '" << filename << "'\n";
		return false;
	}
	else 
	{
		//Tao file thanh cong
		cout << "\nCreated file '" << filename << "'.\n";
    	return true;
	}
}

int SysOpen(char* fileName, int type) {
    if (type != 0 && type != 1) { //type ko hop le
		cerr << "\nInvalid file type!\n";
		return -1;
	}
    
	if(kernel->fileSystem->Open(fileName, type) != -1){	// open thanh cong
		//cerr << "\nOpened file!\n";
		return 0;
	}
	else{	// open ko thanh cong
		//cerr << "\nCan not open file!\n"; 
		return -1;
	}
}

int SysClose(int id) { return kernel->fileSystem->CloseFile(id); }

int SysRead(char* buffer, int charCount, int fileId) {
    if (fileId == 0) {	//doc tu console
        return kernel->synchConsoleIn->GetString(buffer, charCount);
    }
    return kernel->fileSystem->Read(buffer, charCount, fileId);
}

int SysWrite(char* buffer, int charCount, int fileId) {
    if (fileId == 1) {	//in ra console
        return kernel->synchConsoleOut->PutString(buffer, charCount);
    }
    return kernel->fileSystem->Write(buffer, charCount, fileId);
}

int SysSeek(int seekPos, int fileId) {
    if (fileId <= 1) { //file not found
        DEBUG(dbgSys, "\nCan't seek in console");
        return -1;
    }
    return kernel->fileSystem->Seek(seekPos, fileId);
}

int SysDelete(char* name){
	if(kernel->fileSystem->isOpenFile(name) == true) //Check whether the file's already opened or not
	{
		//cout << "\nif " << name << "\n";
		return -1;
	}
	else{
		//cout << "\nelse " << name << "\n";
		bool result = kernel->fileSystem->Delete(name);
		if (result == true){	// xoa thanh cong
			cout << "\nDeleted file '" << name << "'.\n";
			return 0;
		}
		else{	// file can xoa ko ton tai
			cout << "\nFile '" << name << "' not exists.\n";
			return -1;
		}
		return 0;
	}	
}

char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

void SysPrintChar(char character) {
    kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum() { return random(); }

char* SysReadString(int length) {
    char* buffer = new char[length + 1];
	char temp;
	int i;
    for (i = 0; i < length; i++) {
		temp = SysReadChar();
		if (temp == '\n' || temp == '\r')
			break;
        buffer[i] = temp;
    }
	if (i == length)
    	buffer[length] = '\0';
	else 
		buffer[i] = '\0';
    return buffer;
}

void SysPrintString(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        kernel->synchConsoleOut->PutChar(buffer[i]);
    }
}

int SysExec(char* name) {
    // cerr << "call: `" << name  << "`"<< endl;
    OpenFile* oFile = kernel->fileSystem->Open(name);
    if (oFile == NULL) {	// ko mo dc file
        DEBUG(dbgSys, "\nExec:: Can't open this file.");
		cout << "\nExec:: Can't open this file.\n";
        return -1;
    }

    delete oFile;

    // Return child process id
    return kernel->pTab->ExecUpdate(name);
}

int SysJoin(int id) { return kernel->pTab->JoinUpdate(id); }

int SysExit(int id) { return kernel->pTab->ExitUpdate(id); }

#endif /* ! __USERPROG_KSYSCALL_H__ */
