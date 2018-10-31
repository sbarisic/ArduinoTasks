#include "Debug.h"

#include <stdio.h>
#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

HANDLE Process;

void DebugInit() {
	Process = GetCurrentProcess();
	SymInitialize(Process, NULL, TRUE);
}

const char* GetSymbolName(void* Addr) {
	DWORD64  dwDisplacement = 0;

	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
	PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;

	pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	pSymbol->MaxNameLen = MAX_SYM_NAME;

	if (SymFromAddr(Process, (DWORD64)Addr, &dwDisplacement, pSymbol)) {
		return pSymbol->Name;
	} else {
		// SymFromAddr failed
		DWORD error = GetLastError();
		printf("SymFromAddr returned error : %lu\n", error);
	}

	return "";
}