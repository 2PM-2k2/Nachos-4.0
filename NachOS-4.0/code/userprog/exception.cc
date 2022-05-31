// exception.cc 

/////////////////////////////////////
//  DH KHTN - DHQG TPHCM           //
// 20127038 - Nguyen Phuoc Gia Huy //
// 20127063 - Phan Minh Phuc       //
// 20127188 - Nguyen Quoc Huy      //
/////////////////////////////////////

//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define MaxFileNameLength 32 // Do dai quy uoc cho file name

void IncreaseProgramCounter(){
	// Similar to: register[PrevPCReg] = register[PCReg].
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	// Similar to: register[PCReg] = register[NextPCReg].
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(NextPCReg));

	// Similar to: register[NextPCReg] = PCReg right after.
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

// Input: Khong gian dia chi User(int) - gioi han cua buffer(int)
// Output: Bo nho dem Buffer(char*)
// Chuc nang: Sao chep vung nho User sang vung nho System
char* User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; 
	if (kernelBuf == NULL)
		return kernelBuf;
		
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

// Input: Khong gian vung nho User(int) - gioi han cua buffer(int) - bo nho dem buffer(char*)
// Output: So byte da sao chep(int)
// Chuc nang: Sao chep vung nho System sang vung nho User
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}

void HandleHalt(){
	DEBUG(dbgSys, "\n Shutdown, initiated by user program.\n");
	SysHalt();
	ASSERTNOTREACHED();
}

void HandleAdd(){
	DEBUG(dbgSys, "\n Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/*op1*/(int)kernel->machine->ReadRegister(4), /*op2*/(int)kernel->machine->ReadRegister(5));
	DEBUG(dbgSys, "\n Add returning with " << result << "\n");

	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	cout << "Add: " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << " = " << result << "\n";
	
	return IncreaseProgramCounter();
}

void HandleSub(){
	DEBUG(dbgSys, "\n Sub " << kernel->machine->ReadRegister(4) << " - " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysSub Systemcall*/
	int result;
	result = SysSub(/*op1*/(int)kernel->machine->ReadRegister(4), /*op2*/(int)kernel->machine->ReadRegister(5));
	DEBUG(dbgSys, "\n Add returning with " << result << "\n");
					
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	cout << "Sub: " << kernel->machine->ReadRegister(4) << " - " << kernel->machine->ReadRegister(5) << " = " << result << "\n";

	return IncreaseProgramCounter();
}

void HandleCreate(){
	// Input: Dia chi tu vung nho user cua ten file
	// Output: -1 = Loi, 0 = Thanh cong
	// Chuc nang: Tao ra file voi tham so la ten file
	int virtAddr;
	char* filename;
	DEBUG('a', "\n SC_CreateFile call ...");
	DEBUG('a', "\n Reading virtual address of filename");

	virtAddr = kernel->machine->ReadRegister(4); //Doc dia chi cua ten file tu thanh ghi R4
	DEBUG('a', "\n Reading filename.");
			
	//Sao chep khong gian bo nho User sang System, voi do dang toi da la (32 + 1) bytes
	filename = User2System(virtAddr, MaxFileNameLength + 1);
	
	if(SysCreate(filename))
		kernel->machine->WriteRegister(2, 0);
	else
		kernel->machine->WriteRegister(2, -1);

	delete filename;
	return IncreaseProgramCounter();
}

void HandleDelete(){
	int virtAddr = kernel->machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
	char* filename = User2System(virtAddr, MaxFileNameLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength

	kernel->machine->WriteRegister(2, SysDelete(filename));	

	return IncreaseProgramCounter();
}

void HandleOpen(){
	int virtAddr = kernel->machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
	int type = kernel->machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
	char* filename = User2System(virtAddr, MaxFileNameLength); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
	
	kernel->machine->WriteRegister(2, SysOpen(filename, type));
			
	delete[] filename;
	return IncreaseProgramCounter();
}

void HandleClose(){
	//Input id cua file(OpenFileID)
	// Output: 0: thanh cong, -1 that bai
	int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysClose(id));

    return IncreaseProgramCounter();
}

void HandleRead(){
	int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    DEBUG(dbgFile, "Read " << charCount << " chars from file " << fileId << "\n");

    kernel->machine->WriteRegister(2, SysRead(buffer, charCount, fileId));
    System2User(virtAddr, charCount, buffer);

    delete[] buffer;
    return IncreaseProgramCounter();
}

void HandleWrite(){
	int virtAddr = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char* buffer = User2System(virtAddr, charCount);
    int fileId = kernel->machine->ReadRegister(6);

    DEBUG(dbgFile,
          "Write " << charCount << " chars to file " << fileId << "\n");

    kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));
	System2User(virtAddr, charCount, buffer);

    delete[] buffer;
    return IncreaseProgramCounter();
}

void HandleSeek(){
	int seekPos = kernel->machine->ReadRegister(4);
    int fileId = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));

    return IncreaseProgramCounter();
}

void HandleReadChar() {
    char result = SysReadChar();
    kernel->machine->WriteRegister(2, (int)result);
    return IncreaseProgramCounter();
}

void HandlePrintChar() {
    char character = (char)kernel->machine->ReadRegister(4);
    SysPrintChar(character);
    return IncreaseProgramCounter();
}

// void HandlePrintNum() {
//     int character = kernel->machine->ReadRegister(4);
//     SysPrintNum(character);
//     return IncreaseProgramCounter();
// }

// void HandleRandomNum() {
//     int result;
//     result = SysRandomNum();
//     kernel->machine->WriteRegister(2, result);
//     return IncreaseProgramCounter();
// }

#define MAX_STRING_LENGTH 255
void HandleReadString() {
    int virAddr = kernel->machine->ReadRegister(4);  // read address of C-string
    int length = kernel->machine->ReadRegister(5);  // read length of C-string
    if (length > MAX_STRING_LENGTH) {  // avoid allocating large memory
        DEBUG(dbgSys, "String length exceeds " << MAX_STRING_LENGTH);
        SysHalt();
    }
    char* buffer = SysReadString(length);
    System2User(virAddr, 255, buffer);
    delete[] buffer;
    return IncreaseProgramCounter();
}

void HandlePrintString() {
    int virAddr = kernel->machine->ReadRegister(4);  // read address of C-string
    char* buffer = User2System(virAddr, 255);

    SysPrintString(buffer, strlen(buffer));
    delete[] buffer;
    return IncreaseProgramCounter();
}

void HandleExec() {
    int virAddr = kernel->machine->ReadRegister(4);  // doc dia chi ten chuong trinh tu thanh ghi r4
    char* name = User2System(virAddr, 255);  // Lay ten chuong trinh, nap vao kernel
    if (name == NULL) {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
		cout << "\n Not enough memory in System\n";
        kernel->machine->WriteRegister(2, -1);
        return IncreaseProgramCounter();
    }

    kernel->machine->WriteRegister(2, SysExec(name));
    // DO NOT DELETE NAME, THE THEARD WILL DELETE IT LATER
    // delete[] name;

    return IncreaseProgramCounter();
}

/**
 * @brief handle System Call Join
 * @param id: thread id (get from R4)
 * @return -1 if failed to join, otherwise return exit code of
 * the thread. (write result to R2)
 */
void HandleJoin() {
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysJoin(id));
    return IncreaseProgramCounter();
}

void HandleExit() {
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysExit(id));
    return IncreaseProgramCounter();
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "\n Received Exception " << which << " type: " << type << "\n");

    switch (which) {
		case NoException: 			// Nothing wrong. Return control to kernel
			kernel->interrupt->setStatus(SystemMode);
			DEBUG(dbgSys, "\n Return control to kernel.\n");
			break;

		case PageFaultException:	// No valid translation found
			DEBUG('a', "\n No valid translation found.\n");
			cerr << "\n No valid translation found.\n";
			SysHalt();
			break;

		case ReadOnlyException:		// Write attempted to page marked "read-only"
			DEBUG('a', "\n Write attempted to page marked read-only.\n");
			cerr << "\n Write attempted to page marked read-only.\n";
			SysHalt();
			break;

		case BusErrorException:		// Translation resulted in an invalid physical address
			DEBUG('a', "\n Translation resulted invalid physical address.\n");
			cerr << "\n Translation resulted invalid physical address.\n";
			SysHalt();
			break;

		case AddressErrorException:	// Unaligned reference or one that was beyond the end of the address space
			DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space.\n");
			cerr << "\n Unaligned reference or one that was beyond the end of the address space.\n";
			SysHalt();
			break;

		case OverflowException:		// Integer overflow in add or sub.
			DEBUG('a', "\n Integer overflow in add or sub.\n");
			cerr << "\n Integer overflow in add or sub.\n";
			SysHalt();
			break;

		case IllegalInstrException:	// Unimplemented or reserved instr.
			DEBUG('a', "\n Unimplemented or reserved instr.\n");
			cerr << "\n Unimplemented or reserved instr.\n";
			SysHalt();
			break;

		case NumExceptionTypes:
			DEBUG('a', "\n Number exception types.\n");
			cerr << "\n Number exception types.\n";
			SysHalt();
			break;

    	case SyscallException:
    		switch(type) {
    			case SC_Halt:
					HandleHalt();
					break;

      			case SC_Add:
					HandleAdd();
					break;

				case SC_Sub:
					HandleSub();
					break;
			
				case SC_CreateFile:
					HandleCreate();
					break;
				case SC_Delete:
					HandleDelete();			
					break;

				case SC_Open:
					HandleOpen();				
					break;

				case SC_Read:
					HandleRead();			
					break;

				case SC_Write:
					HandleWrite();			
					break;

				case SC_Seek:
					HandleSeek();			
					break;

				case SC_Close:
					HandleClose();
					break;
				
				case SC_ReadChar:
					HandleReadChar();
					break;

				case SC_PrintChar:
					HandlePrintChar();
					break;

				case SC_ReadString:
					HandleReadString();
					break;

				case SC_PrintString:
					HandlePrintString();
					break;

				case SC_Exec:
					HandleExec();
					break;

				case SC_Join:
					HandleJoin();
					break;
				
				case SC_Exit:
					HandleExit();
					break;

				// case SC_PrintNum:
				// 	HandlePrintNum();
				// 	break;

      			default:
					cerr << "\nUnexpected system call " << type << "\n";
				break;
      		}
      		break;
    	default:
      		cerr << "\nUnexpected user mode exception" << (int)which << "\n";
      		break;
    }
}
