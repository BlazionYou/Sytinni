#include "memory.h"

namespace memory
{
void copy(DWORD address, void* value, int length)
{
	 const PVOID addr = reinterpret_cast<PVOID>(address);
	 DWORD oldProtect;
	 DWORD newProtect;
	 VirtualProtect(addr, length, PAGE_EXECUTE_READWRITE, &oldProtect);
	 memcpy(addr, value, length);
	 VirtualProtect(addr, length, oldProtect, &newProtect);
}

void write(DWORD address, void* value, int length)
{
	 copy(address, value, length);
}

void set(DWORD address, DWORD value, int size)
{
	 const PVOID addr = reinterpret_cast<PVOID>(address);
	 DWORD oldProtect;
	 DWORD newProtect;
	 VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	 memset(addr, value, size);
	 VirtualProtect(addr, size, oldProtect, &newProtect);
}

void patchAddress(DWORD address, DWORD value)
{
	 set(address, value, 4);
}

void nopAddress(DWORD address, int nopCount)
{
	 set(address, 0x90, nopCount); // 0x90 = NOP
}

void createJMP(PBYTE address, DWORD jumpToAddress, DWORD overrideLength)
{
	 DWORD oldProtect;
	 DWORD newProtect;
	 DWORD relativeAddress;
	 VirtualProtect(address, overrideLength, PAGE_EXECUTE_READWRITE, &oldProtect);
	 relativeAddress = (DWORD)(jumpToAddress - (DWORD)address) - 5;
	 *address = 0xE9; // 0xE9 = JMP
	 *((DWORD*)(address + 0x1)) = relativeAddress;
	 for (DWORD x = 0x5; x < overrideLength; x++)
	 {
		  *(address + x) = 0x90; // 0x90 = NOP
	 }
	 VirtualProtect(address, overrideLength, oldProtect, &newProtect);
}

DWORD getAddress(DWORD baseAddress, int ptrDepth)
{
	 DWORD address = *(DWORD*)(baseAddress);
	 for (int i = 0; i < ptrDepth; ++i)
	 {
		  address = *(DWORD*)address;
	 }
	 return address;
}

DWORD getAddress(DWORD baseAddress, std::vector<int>& offsets)
{
	 DWORD address = *(DWORD*)(baseAddress);
	 for (unsigned long offset : offsets)
	 {
		  address = *(DWORD*)(address + offset);
	 }
	 return address;
}



}


