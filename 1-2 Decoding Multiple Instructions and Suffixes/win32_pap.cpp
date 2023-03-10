/*
win32_pap.cpp 
 Perforamce Aware Programming 
 Homework #2 - Decoding Multiple Instructions and Suffixes
 Lucas Chacon 
 03/09/2023
*/

#include <iostream>
#include <stdio.h>
#include <windows.h>


void *ProgramMemory;
uint64_t TotalMemorySize;

struct debug_read_file_data
{
    uint32_t FileSize;
    char *FileName;
    void *MemoryLocation;
};

//reg[16][2] gave me an overflow error...

char reg[16][4] = 
{
    {"al"},
    {"cl"},
    {"dl"},
    {"bl"},
    {"ah"},
    {"ch"},
    {"dh"},
    {"bh"},
    {"ax"},
    {"cx"},
    {"dx"},
    {"bx"},
    {"sp"},
    {"bp"},
    {"si"},
    {"di"}
};

char nodisp[8][10] = 
{
    {"bx + si"},
    {"bx + di"},
    {"bp + si"},
    {"bp + di"},
    {"si"},
    {"di"},
    {"bp"},
    {"bx"}
    
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


int main(int argc, char *argv[] )
{
    
    TotalMemorySize = Kilobytes(1);
    ProgramMemory = VirtualAlloc(0,TotalMemorySize,
                                 MEM_COMMIT|MEM_RESERVE,
                                 PAGE_READWRITE);
    
    debug_read_file_data fileData = {};
    fileData.MemoryLocation = ProgramMemory;
    fileData.FileName = "C:\\projects\\PAP\\Listings\\Listing 39";
    Win32ReadFile(&fileData);
    
    char *cursor = (char *)ProgramMemory;
    uint32_t byteCounter = 0;
    
    while(byteCounter < fileData.FileSize)
    {
        //determine what opcode to use
        char byte1  = *((char *)cursor);
        char byte2 = *(((char *)cursor)+1);
        char byte3 = *(((char *)cursor)+2);
        char *byte3ptr = ((char *)cursor)+2;
        
        //4 bit opcode
        char fourbitopcode = (byte1 >> 4) & 0b00001111 ;
        //char movImmediateToRegister = 0b1011 ; //MOV Immediate to register
        
        if(fourbitopcode == 0b1011)
        {
            //process move immediate to Register
            //need to get the register
            char RegOperand = (byte1) & 0b00000111;
            
            char valueW = (byte1 >> 3) & 0b00000001;
            
            reg_code _RegOperandData = {};
            char *RegOperandData = {};
            char offset = 8*valueW;
            RegOperandData  = reg[RegOperand + offset];
            
            
            char *byte2 = (((char *)cursor)+1);
            char outputA[32] = {};
            //8bit immediate to register
            if(valueW == 0)
            {
                char *byte2 = (((char *)cursor)+1);
                char byte2char = *byte2;
                sprintf_s(outputA,"%i",byte2char);
                std::cout << "mov";
                std::cout << " ";
                std::cout << RegOperandData;
                std::cout << ", ";
                std::cout << outputA;
                cursor +=2;
                byteCounter +=2;
                
            }
            //16 bit immediate to register
            else if(valueW == 1)
            {
                short byte2short = *((short *)byte2);
                sprintf_s(outputA,"%i",byte2short);
                std::cout << "mov";
                std::cout << " ";
                std::cout << RegOperandData;
                std::cout << ", ";
                std::cout << outputA;
                cursor +=3;
                byteCounter +=3;
                
            }
            std::cout << "\n";
        }
        //process 6 bit opcode
        else
        {
            char sixbitopcode = byte1 >> 2;
            char sixbitopcodeMask = 0b00111111;
            sixbitopcode = sixbitopcode & sixbitopcodeMask;
            
            char valueW = byte1 & 0b00000001;
            char valueD = (byte1 >> 1) & 0b00000001;
            
            char MODFieldEncoding = (byte2 >> 6) & 0b00000011;
            char RegOperand = (byte2 >> 3) & 0b00000111;
            char RMOperand = byte2 & 0b00000111;
            
            char *RegOperandData = {};
            char *RMOperandData = {};
            
            char offset = 8*valueW;
            RegOperandData= reg[RegOperand + offset];
            RMOperandData = reg[RMOperand + offset];
            
            switch(sixbitopcode)
            {
                //MOV Register/memory to/from register
                case 0b100010:
                {
                    std::cout << "mov ";
                    //register to register
                    if(MODFieldEncoding == 0b11)
                    {
                        //std::cout << "register to register";
                        if(valueD == 0)
                        {
                            //RegOperand is the source
                            //RMOperand is the destination
                            std::cout << RMOperandData;
                            std::cout << ", ";
                            std::cout << RegOperandData;
                        }
                        else if(valueD == 1)
                        {
                            //RegOperand is the destination
                            //RegOperand is the source
                            std::cout << RegOperandData;
                            std::cout << ", ";
                            std::cout << RMOperandData;
                        }
                        cursor +=2;
                        byteCounter +=2;
                    }
                    //Memory Mode - no displacement
                    else if(MODFieldEncoding == 0b00)
                    {
                        //RMOperand
                        if(RMOperand == 0b110)
                        {
                            //Direct Address, use 16 bit displacement
                            cursor +=4;
                            byteCounter +=4;
                        }
                        else
                        {
                            //RegOperand is the source
                            //RMOperand is the destination
                            if(valueD == 0)
                            {
                                std::cout << "[";
                                std::cout << nodisp[RMOperand];
                                std::cout << "]";
                                std::cout << ", ";
                                std::cout << RegOperandData;
                                
                            }
                            else if(valueD == 1)
                            {
                                std::cout << RegOperandData ;
                                std::cout <<  ", ";
                                std::cout << "[";
                                std::cout << nodisp[RMOperand];
                                std::cout << "]";
                            }
                            cursor +=2;
                            byteCounter +=2;
                        }
                    }
                    //Memory Mode - 8 bit displacement
                    else if(MODFieldEncoding == 0b01)
                    {
                        char outputBuffer[16] = {};
                        if(byte3 > 0)
                        {
                            sprintf_s(outputBuffer,"%s, [%s + %i]",&RegOperandData,&nodisp[RMOperand],byte3);
                            //std::cout << 
                            //std::cout << ", ";
                            //std::cout << "[";
                            std::cout << outputBuffer;
                            //std::cout << "]";
                        }
                        else
                        {
                            if(valueD == 0)
                            {
                                std::cout << "[";
                                std::cout << nodisp[RMOperand];
                                std::cout << "]";
                                std::cout << ", ";
                                std::cout << RegOperandData;
                            }
                            else
                            {
                                std::cout << RegOperandData;
                                std::cout << ", ";
                                std::cout << "[";
                                std::cout << nodisp[RMOperand];
                                std::cout << "]";
                            }
                        }
                        cursor +=3;
                        byteCounter +=3;
                    }
                    //Memory Mode - 16 bit displacement
                    else if(MODFieldEncoding == 0b10)
                    {
                        char outputBuffer[16] = {};
                        short byte3short = *((short *)byte3ptr);
                        if(byte3short > 0)
                        {
                            sprintf_s(outputBuffer,"%s + %i",&nodisp[RMOperand],byte3short);
                            std::cout << RegOperandData;
                            std::cout << ", ";
                            std::cout << "[";
                            std::cout << outputBuffer;
                            std::cout << "]";
                        }
                        else
                        {
                            std::cout << RegOperandData;
                            std::cout << ", ";
                            std::cout << "[";
                            std::cout << nodisp[RMOperand];
                            std::cout << "]";
                        }
                        cursor +=4;
                        byteCounter +=4;
                    }
                    std::cout << "\n";
                }break;
                
            }
            
            
        }
    }
}
