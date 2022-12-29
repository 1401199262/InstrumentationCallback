#pragma once
#include "sym.h"

extern "C" void InstrCallbackEntry();

extern"C" NTSTATUS NTAPI NtSetInformationProcess(
	HANDLE ProcessHandle,
	ULONG ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength
	);

extern "C" NTSTATUS NtContinue(PCONTEXT, u64);


u32 tls_index = 0;

bool InitInstrumentCallback()
{
	tls_index = TlsAlloc();

	if (tls_index == TLS_OUT_OF_INDEXES)
	{
		printf("Couldn't allocate a TLS index\n");
		return false;
	}


	PROCESS_INSTRUMENTATION_CALLBACK_INFORMATION info;
	info.Version = 0;
	info.Reserved = 0;
	info.Callback = InstrCallbackEntry;

	//NtSetInformationProcess + ProcessInstrumentationCallback 
	auto status = NtSetInformationProcess((HANDLE)-1, 0x28, &info, sizeof(info));
	if (status)
	{
		printf("NtSetInformationProcess Falied with ntstatus %x\n", status);
		return false;
	}
	
	return true;
}

typedef struct _ThreadData
{
	volatile u8 IsThreadHandlingSyscall;
}ThreadData, *PThreadData;

PThreadData GetThreadDataBuffer()
{
	pv thread_data = TlsGetValue(tls_index);
	if (!thread_data)
	{
		thread_data = LocalAlloc(LPTR, sizeof(ThreadData));
		if (!thread_data)
			__debugbreak();

		memset(thread_data, 0, sizeof(ThreadData));

		if (!TlsSetValue(tls_index, thread_data)) 		
			__debugbreak();
		
	}

	return (PThreadData)thread_data;
}

extern "C" void CallbackRoutine(CONTEXT* ctx)
{
	ctx->Rip = __readgsqword(0x02d8);
	ctx->Rsp = __readgsqword(0x02e0);
	ctx->Rcx = ctx->R10;

	PThreadData CurrentThreadData = GetThreadDataBuffer();
	if (CurrentThreadData->IsThreadHandlingSyscall)
		NtContinue(ctx, 0);
	
	CurrentThreadData->IsThreadHandlingSyscall = true;

	u64 SyscallFunctionOffset = 0;
	auto SyscallFunctionName = GetFunctionInfoByAddress(ctx->Rip, &SyscallFunctionOffset);
	printf("syscall function detected : 0x%llx %s+0x%llx\n", ctx->Rip, SyscallFunctionName.c_str(), SyscallFunctionOffset);

	ShowBackTrace(*ctx);

	CurrentThreadData->IsThreadHandlingSyscall = false;


	NtContinue(ctx, 0);	

	__debugbreak();
}