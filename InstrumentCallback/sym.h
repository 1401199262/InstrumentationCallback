#pragma once
#include "Internal.h"

bool InitSymbol()
{
	SymSetOptions(SYMOPT_UNDNAME);
	if (!SymInitialize((HANDLE)-1, 0, true))
	{
		printf("SymInitialize Failed with %d\n", GetLastError());
		return false;
	}

	return true;
}

std::string GetFunctionInfoByAddress(u64 address, u64* FunctionOffset)
{
    auto buffer = malloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
    if (!buffer)     
        __debugbreak();    

    memset(buffer, 0, sizeof(SYMBOL_INFO) + MAX_SYM_NAME);

    auto symbol_information = (PSYMBOL_INFO)buffer;
    symbol_information->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol_information->MaxNameLen = MAX_SYM_NAME;

    auto result = SymFromAddr((HANDLE)-1, address, FunctionOffset, symbol_information);

    if (!result) {
        free(buffer);
        return "Could_Not_Obtain_Name";
    }

    auto built_string = std::string(symbol_information->Name);

    free(buffer);

    return built_string;
}

void ShowBackTrace(CONTEXT context)
{
	printf("\tStackTrace: \n");

    STACKFRAME64 stack_frame;
    memset(&stack_frame, 0, sizeof(stack_frame));
    stack_frame.AddrPC.Offset = context.Rip;
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrStack.Offset = context.Rsp;
    stack_frame.AddrStack.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Offset = context.Rbp;
    stack_frame.AddrFrame.Mode = AddrModeFlat;

    //DWORD line_displacement;
    //IMAGEHLP_LINE64 line;
    //memset(&line, 0, sizeof(line));
    //line.SizeOfStruct = sizeof(line);

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
    if (!symbol) __debugbreak();
    memset(symbol, 0, sizeof(SYMBOL_INFO) + MAX_SYM_NAME);
    symbol->MaxNameLen = MAX_SYM_NAME;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    int cnt = 0;
    while (StackWalk64(IMAGE_FILE_MACHINE_AMD64, (HANDLE)-1, (HANDLE)-2, &stack_frame, &context,
        NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) 
    {
        //if (SymGetLineFromAddr64((HANDLE)-1, stack_frame.AddrPC.Offset, &line_displacement, &line)) 
        //{
        //    std::cout << line.FileName << ':' << line.LineNumber << '\n';
        //}
        //else
        {
            u64 Displacement = 0;
            if(SymFromAddr((HANDLE)-1, stack_frame.AddrPC.Offset, &Displacement, symbol))
                printf("\t\t[%d] 0x%llx %s+0x%llx\n", cnt, stack_frame.AddrPC.Offset, symbol->Name, Displacement);
            else
                printf("\t\t[%d] 0x%llx %s+0x%llx\n", cnt, stack_frame.AddrPC.Offset, "Could_Not_Obtain", Displacement);

            cnt += 1;
        }
    }
    free(symbol);



	//printf("0x%llx %s+0x%llx\n", BackTrace[i], FunctionName.c_str(), Offset);
	
}
