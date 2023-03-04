/*
win32_pap.cpp 
 Perforamce Aware Programming 
 Homework #1
 Lucas Chacon 
 03/04/2023
*/

#include <iostream>
#include <stdio.h>
#include <windows.h>
//#include "win32_xi.h"

void *ProgramMemory;
uint64_t TotalMemorySize;

struct debug_read_file_data
{
    uint32_t FileSize;
    char *FileName;
    void *MemoryLocation;
};

struct reg_code
{
    char A;
    char B;
};


#define Assert(Expression) if(!(Expression)){*(int*)0=0;}
#define Kilobytes(Value)  ((Value) *1024LL)

inline
uint32_t SafeTruncateUInt64(uint64_t Value)
{
    Assert(Value <= 0xFFFFFFFF);
    uint32_t  Result = (uint32_t)Value;
    return(Result);
}


bool Win32ReadFile(debug_read_file_data *fileData)
{
    
    bool result =false;
    uint32_t fileSize32 =0;
    HANDLE fileHandle = CreateFileA(fileData->FileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,0);
    if(fileHandle)
    {
        //get file size
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle,&fileSize))
        {
            fileSize32=  SafeTruncateUInt64(fileSize.QuadPart);
            //VirtualAlloc(0,FileSize32,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
            if(fileData->MemoryLocation)
            {
                DWORD BytesRead;
                if(ReadFile(fileHandle,fileData->MemoryLocation,fileSize32,&BytesRead,0)
                   && (BytesRead == fileSize32))
                {
                    //read file success
                    fileData->FileSize = fileSize32;
                    result =true;
                    
                }
                else
                {
                    Assert(!"Read Failed");
                }
            }
            else
            {
            }
        }
        else
        {
        }
        CloseHandle(fileHandle);
    }
    else
    {
    }
    return result;
}

reg_code DetermineRegCode(char valueW,char RegOperand1)
{
    char regValueA1 = {};
    char regValueA2 = {};
    
    char regA1[8] = {'a','c','d','b','s','b','s','d'};
    char regA2[8] = {'x','x','x','x','p','p','i','i'};
    
    char regB1[8] = {'a','c','d','b','a','c','d','b'};
    char regB2[8] = {'l','l','l','l','h','h','h','h'};
    
    if(valueW ==1)
    {
        regValueA1 = regA1[RegOperand1];
        regValueA2 = regA2[RegOperand1];
        
    }
    else if(valueW ==0)
    {
        regValueA1 = regB1[RegOperand1];
        regValueA2 = regB2[RegOperand1];
        
    }
    reg_code result = {};
    result.A = regValueA1;
    result.B = regValueA2;
    return result;
}


int main(int argc, char *argv[] )
{
    
    TotalMemorySize = Kilobytes(1);
    //entire primary screen including taskbar
    ProgramMemory = VirtualAlloc(0,TotalMemorySize,
                                 MEM_COMMIT|MEM_RESERVE,
                                 PAGE_READWRITE);
    
    debug_read_file_data fileData = {};
    fileData.MemoryLocation = ProgramMemory;
    fileData.FileName = "../Listing 38";
    Win32ReadFile(&fileData);
    
    char *cursor = (char *)ProgramMemory;
    uint32_t steps = fileData.FileSize / 2;
    
    for(uint32_t x = 0; x<steps;x++)
    {
        char byte1  = *((char *)cursor);
        char opcodeCMD = byte1 >> 2;
        char opcodeMask = 0b00111111;
        opcodeCMD = opcodeCMD & opcodeMask;
        char movCMD = 0b100010;
        
        char valueW = byte1 & 0b00000001;
        char valueD = (byte1 >> 1) & 0b00000001;
        
        //what registers are involved
        char byte2 = *(((char *)cursor)+1);
        
        char MODFieldEncoding = byte2 >> 6;
        char MODFieldMask = 0b00000011;
        MODFieldEncoding = MODFieldEncoding & MODFieldMask ;
        
        char RegOperand = byte2 >> 3;
        char RegOperandMask = 0b00000111;
        RegOperand = RegOperand & RegOperandMask;
        char regValueA1 = {};
        char regValueA2 = {};
        
        char RMOperand = byte2;
        RMOperand = RMOperand & RegOperandMask;
        char regValueB1 = {};
        char regValueB2 = {};
        
        
        reg_code RegOperandData = {};
        reg_code RMOperandData = {};
        
        RegOperandData = DetermineRegCode(valueW,RegOperand);
        RMOperandData = DetermineRegCode(valueW,RMOperand);
        
        if(opcodeCMD == movCMD)
        {
            std::cout << "mov";
            if(MODFieldEncoding == 0b11)
            {
                //std::cout << "register to register";
                if(valueD == 0)
                {
                    //RegOperand is the source
                    //RMOperand is the destination
                    std::cout << " ";
                    std::cout << RMOperandData.A;
                    std::cout << RMOperandData.B;
                    std::cout << ", ";
                    std::cout << RegOperandData.A;
                    std::cout << RegOperandData.B;
                }
                else if(valueD == 1)
                {
                    //RegOperand is the destination
                    //RegOperand is the source
                    std::cout << " ";
                    std::cout << RegOperandData.A;
                    std::cout << RegOperandData.B;
                    std::cout << ", ";
                    std::cout << RMOperandData.A;
                    std::cout << RMOperandData.B;
                }
            }
            std::cout << "\n";
        }
        cursor = cursor +2;
        
    }
    Sleep(5000);
}
