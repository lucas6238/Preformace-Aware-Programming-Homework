/*Win32_xi.cpp*/

#define local_persist static
#define global_variable static

#include "xi.h"
#include "xi_memory.cpp"
#include "xi_utility.cpp"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "win32_xi.h"

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
    
    if(valueW ==1)
    {
        
        switch(RegOperand1)
        {
            case 0b000:
            {
                regValueA1= 'a';
                regValueA2= 'x';
            }break;
            case 0b001:
            {
                regValueA1= 'c';
                regValueA2= 'x';
            }break;
            case 0b010:
            {
                regValueA1= 'd';
                regValueA2= 'x';
            }break;
            case 0b011:
            {
                regValueA1= 'b';
                regValueA2= 'x';
            }break;
            case 0b100:
            {
                regValueA1= 's';
                regValueA2= 'p';
            }break;
            case 0b101:
            {
                regValueA1= 'b';
                regValueA2= 'p';
            }break;
            case 0b110:
            {
                regValueA1= 's';
                regValueA2= 'i';
            }break;
            case 0b111:
            {
                regValueA1= 'd';
                regValueA2= 'i';
            }break;
        }
    }
    else if(valueW ==0)
    {
        switch(RegOperand1)
        {
            case 0b000:
            {
                regValueA1= 'a';
                regValueA2= 'l';
            }break;
            case 0b001:
            {
                regValueA1= 'c';
                regValueA2= 'l';
            }break;
            case 0b010:
            {
                regValueA1= 'd';
                regValueA2= 'l';
            }break;
            case 0b011:
            {
                regValueA1= 'b';
                regValueA2= 'l';
            }break;
            case 0b100:
            {
                regValueA1= 'a';
                regValueA2= 'h';
            }break;
            case 0b101:
            {
                regValueA1= 'c';
                regValueA2= 'h';
            }break;
            case 0b110:
            {
                regValueA1= 'd';
                regValueA2= 'h';
            }break;
            case 0b111:
            {
                regValueA1= 'b';
                regValueA2= 'h';
            }break;
        }
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
    fileData.FileName = "C:\\projects\\PAP\\Homework\\Mov Encoder\\Listing 38";
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
    
    
    //std::cout << "hello";
    Sleep(2000);
}
