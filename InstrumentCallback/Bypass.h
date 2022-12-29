#pragma once

//possible detection : impossible to detect :( 
SHORT GetKeyNoob(_In_ int vKey)
{
	printf("[Noob] Prepare to GetKey \n");
	return GetAsyncKeyState(vKey);
}

typedef SHORT(__cdecl* NtUserGetAsyncKeyStatefn)(INT key);
extern"C" NtUserGetAsyncKeyStatefn NtUserGetAsyncKeyState = 0;

//possible detection : normal inline hook would catch this
SHORT GetKeyEasy(_In_ int vKey)
{
	if (!NtUserGetAsyncKeyState)
	{
		NtUserGetAsyncKeyState = (NtUserGetAsyncKeyStatefn)GetProcAddress(
			LoadLibrary(L"win32u.dll"), "NtUserGetAsyncKeyState");
	}
	printf("[Easy] Prepare to GetKey \n");
	return NtUserGetAsyncKeyState(vKey);
}

extern"C" u32 NtUserGetKeySyscallIndex = 0;
extern"C" u64 SyscallTrampoline = 0;
extern"C" SHORT AsmGetKeySkilled(_In_ int vKey);
extern"C" SHORT AsmGetKeyExpert(_In_ int vKey);
extern"C" SHORT AsmGetKeyEvil(_In_ int vKey);

//possible detection : check syscall address is in legal module in Intrument callback 
SHORT GetKeySkilled(_In_ int vKey)
{
	printf("[Skilled] Prepare to GetKey \n");
	if (!NtUserGetKeySyscallIndex)
		NtUserGetKeySyscallIndex = *(u32*)((u64)NtUserGetAsyncKeyState + 0x4);

	return AsmGetKeySkilled(vKey);
}

//possible detection : stackwalk and find caller or compare KTHREAD.SystemCallNumber 
SHORT GetKeyExpert(_In_ int vKey)
{
	printf("[Expert] Prepare to GetKey \n");

	if (!SyscallTrampoline)
	{
		u64 NtCloseAddr = (u64)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtClose");
		SyscallTrampoline = NtCloseAddr + 0x12;
	}

	return AsmGetKeyExpert(vKey);
}

extern"C" u64 JmpRdi = 0;

//possible detection : compare KTHREAD.SystemCallNumber with syscall index in return address 
SHORT GetKeyEvil(_In_ int vKey)
{
	//delete pdb file to get real output 

	printf("[Evil] Prepare to GetKey \n");
	
	if (!JmpRdi)
	{
		JmpRdi = (u64)FindPatternSect(GetModuleHandle(0), ".text", "FF E7");
		if (!JmpRdi)
		{
			__debugbreak();
			//have you delete innocent_function ?
		}
	}

	if (!SyscallTrampoline)
	{
		u64 NtCloseAddr = (u64)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtClose");
		SyscallTrampoline = NtCloseAddr + 0x12;
	}

	return AsmGetKeyEvil(vKey);
}

__declspec(dllexport) int innocent_function()
{
	return 0xE7FF;
}



//GetKeyKernel : gafAsyncKeyStateRecentDown + gafAsyncKeyState 

