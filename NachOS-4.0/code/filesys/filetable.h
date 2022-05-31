#ifndef FILETABLE_H
#define FILETABLE_H
#include "openfile.h"
#include "sysdep.h"
#include <string.h>

#define FILE_MAX 10
#define CONSOLE_IN 0
#define CONSOLE_OUT 1
#define MODE_READWRITE 0
#define MODE_READ 1
#define MODE_WRITE 2

class FileTable {
   private:
    OpenFile** openFile;
    int* fileOpenMode;
    char** FileName; 

   public:
    FileTable() {
        openFile = new OpenFile*[FILE_MAX];
        FileName = new char*[FILE_MAX];
        fileOpenMode = new int[FILE_MAX];
        fileOpenMode[CONSOLE_IN] = MODE_READ;
        fileOpenMode[CONSOLE_OUT] = MODE_WRITE;
        //2 o dau de xu ly I/O tu console
    }

    int Insert(char* fileName, int openMode) {
        int freeIndex = -1;
        int fileDescriptor = -1;
        for (int i = 2; i < FILE_MAX; i++) {
            if (openFile[i] == NULL) {
                freeIndex = i;
                break;
            }
        }

        if (freeIndex == -1) { //khong du o nho
            return -1;
        }

        if (openMode == MODE_READWRITE){
            //cout << "\nOpen file for read and write.\n";
            fileDescriptor = OpenForReadWrite(fileName, FALSE);
        }
        if (openMode == MODE_READ){
            //cout << "\nOpen file for read.\n";
            fileDescriptor = OpenForRead(fileName, FALSE);
        }

        if (fileDescriptor == -1) {  //khong co file
            cout << "\nFile '" << fileName << "'not found.\n";
            cout << endl << fileDescriptor << endl;
            return -1;
        }
        openFile[freeIndex] = new OpenFile(fileDescriptor);
        fileOpenMode[freeIndex] = openMode;
        FileName[freeIndex] = fileName;
        cout << "\nOpen file '" << fileName << "' at index " << freeIndex << ".\n";
        return freeIndex;
    }

    int Remove(int index) {
        if (index < 2 || index >= FILE_MAX) {
            return -1; //invalid index
        }
        if (openFile[index] != NULL) {
            delete openFile[index];
            openFile[index] = NULL;
            //cout << endl << FileName[index] << endl;
            delete FileName[index];
            FileName[index] = NULL;  
            //FileFlag[index] = false;
            cout << "\nClosed file at id " << index << ".\n";
            return 0;
        }
        else{
            //cerr << "\nFile at " << index << " not exists.\n";
            return -1;
        }
    }

    int Read(char* buffer, int charCount, int index) {
        //cout << "\nIndex: " << index << endl;
        if (index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        int result = openFile[index]->Read(buffer, charCount);
        // if we cannot read enough bytes, we should return -2
        if (result != charCount) return -2;
        return result;
    }

    int Write(char* buffer, int charCount, int index) {
        if (index >= FILE_MAX) return -1;
        if (openFile[index] == NULL || fileOpenMode[index] == MODE_READ)
            return -1;
        return openFile[index]->Write(buffer, charCount);
    }

    int Seek(int pos, int index) {
        if (index <= 1 || index >= FILE_MAX) return -1;
        if (openFile[index] == NULL) return -1;
        // use seek(-1) to move to the end of file
        if (pos == -1) pos = openFile[index]->Length();
        if (pos < 0 || pos > openFile[index]->Length()) return -1;
        return openFile[index]->Seek(pos);
    }

    bool isOpen(char* name){
        for (int i = 2; i < FILE_MAX; i++)
            if (FileName[i] != NULL)
                if (strcmp(name, FileName[i]) == 0){
                    cout << "\nFile '" << name << "' is in use!\n";
                    return true;
                }

        return false;
    }

    ~FileTable() {
        for (int i = 0; i < FILE_MAX; i++) {
            if (openFile[i]) delete openFile[i];
            if (FileName[i]) delete FileName[i];
        }
        delete[] openFile;
        delete[] fileOpenMode;
        delete[] FileName;
        //delete[] FileFlag;
    }
};

#endif