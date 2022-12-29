#pragma once
#include <iostream>
#include <map>
#include <Windows.h>
#include <winternl.h>
#include <DbgHelp.h>

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "imagehlp.lib")

#define noinl __declspec(noinline)
#define naked __declspec(naked)
#define inl __forceinline

using u64 = unsigned long long;
using u32 = unsigned long;
using u16 = unsigned short;
using u8 = unsigned char;
using pv64 = void*;
using pv = void*;
using i64 = __int64;
using u64 = unsigned __int64;
using DWORD = unsigned long;
using WORD = unsigned short;
using BYTE = unsigned char;
using size_t = SIZE_T;
using UINT = unsigned int;

typedef struct _PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION
{
	ULONG Version;
	ULONG Reserved;
	PVOID Callback;
} PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION, * PPROCESS_INSTRUMENTATION_CALLBACK_INFORMATION;

#define IMAGE_FIRST_SECTION(NtHeader) (PIMAGE_SECTION_HEADER)(NtHeader + 1)
#define NT_HEADER(ModBase) (PIMAGE_NT_HEADERS)((ULONG64)(ModBase) + ((PIMAGE_DOS_HEADER)(ModBase))->e_lfanew)
#define SizeAlign(Size) ((Size + 0xFFF) & 0xFFFFFFFFFFFFF000)
#define ConstStrLen(Str) ((sizeof(Str) - sizeof(Str[0])) / sizeof(Str[0]))
#define ToLower(Char) ((Char >= 'A' && Char <= 'Z') ? (Char + 32) : Char)
#define ToUpper(Char) ((Char >= 'a' && Char <= 'z') ? (Char - 'a') : Char)

template <typename StrType, typename StrType2>
__forceinline bool StrICmp(StrType Str, StrType2 InStr, bool CompareFull) {
	if (!Str || !InStr) return false;
	wchar_t c1, c2; do {
		c1 = *Str++; c2 = *InStr++;
		c1 = ToLower(c1); c2 = ToLower(c2);
		if (!c1 && (CompareFull ? !c2 : 1))
			return true;
	} while (c1 == c2);

	return false;
}

PVOID FindSection(PVOID ModBase, const char* Name, PULONG SectSize)
{
	//get & enum sections
	PIMAGE_NT_HEADERS NT_Header = NT_HEADER(ModBase);
	PIMAGE_SECTION_HEADER Sect = IMAGE_FIRST_SECTION(NT_Header);

	for (PIMAGE_SECTION_HEADER pSect = Sect; pSect < Sect + NT_Header->FileHeader.NumberOfSections; pSect++)
	{
		//copy section name
		char SectName[9]; SectName[8] = 0;
		*(ULONG64*)&SectName[0] = *(ULONG64*)&pSect->Name[0];

		//check name
		if (StrICmp(Name, SectName, true))
		{
			//save size
			if (SectSize) {
				ULONG SSize = SizeAlign(max(pSect->Misc.VirtualSize, pSect->SizeOfRawData));
				*SectSize = SSize;
			}

			//ret full sect ptr
			return (PVOID)((ULONG64)ModBase + pSect->VirtualAddress);
		}
	}

	//no section
	return nullptr;
}

inl bool readByte(PVOID addr, UCHAR* ret)
{
	//__try
	*ret = *(volatile char*)addr;
	return true;
}

PUCHAR FindPatternSect(PVOID ModBase, const char* SectName, const char* Pattern)
{
	//find pattern utils
#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((UCHAR)(GetBits(x[0]) << 4 | GetBits(x[1])))
	if (!ModBase) return nullptr;

	//get sect range
	ULONG SectSize;
	PUCHAR ModuleStart = (PUCHAR)FindSection(ModBase, SectName, &SectSize);
	PUCHAR ModuleEnd = ModuleStart + SectSize;

	if (!ModuleStart) return nullptr;

	//scan pattern main
	PUCHAR FirstMatch = nullptr;
	const char* CurPatt = Pattern;
	if (*Pattern == '\0')
		CurPatt++;

	for (; ModuleStart < ModuleEnd; ++ModuleStart)
	{
		bool SkipByte = (*CurPatt == '\?');

		//hp(ModuleStart);
		UCHAR byte1;
		if (!readByte(ModuleStart, &byte1)) {
			auto addr2 = (u64)ModuleStart;
			addr2 &= 0xFFFFFFFFFFFFF000;
			addr2 += 0xFFF;
			ModuleStart = (PUCHAR)addr2;
			//sp("123");
			goto Skip;
		}

		if (SkipByte || byte1 == GetByte(CurPatt)) {
			if (!FirstMatch) FirstMatch = ModuleStart;
			if (SkipByte)
				CurPatt += 2;
			else
				CurPatt += 3;
			if (CurPatt[-1] == 0) return FirstMatch;
		}

		else if (FirstMatch) {
			ModuleStart = FirstMatch;
		Skip:
			FirstMatch = nullptr;
			CurPatt = Pattern;
		}
	}

	//failed
	return nullptr;
}

