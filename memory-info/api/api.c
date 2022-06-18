#include "api.h"

#include "defs.h"

#include <stdio.h>

DWORD  get_pid_by_process_name               ( const char* pName ) {
	HANDLE snap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( snap == INVALID_HANDLE_VALUE ) {
		return 0;
	}

	PROCESSENTRY32 pEntry = { 0 };
	pEntry.dwSize = sizeof( pEntry );

	if ( Process32First( snap, &pEntry ) ) {
		if ( !strcmp( pEntry.szExeFile, pName ) ) {
			return pEntry.th32ProcessID;
		}
	}

	while ( Process32Next( snap, &pEntry ) ) {
		if ( !strcmp( pEntry.szExeFile, pName ) ) {
			return pEntry.th32ProcessID;
		}
	}

	return 0;
}
HANDLE get_phandle                           ( DWORD pId ) {
	return OpenProcess(PROCESS_ALL_ACCESS,FALSE,pId);
}


void   write                                 ( HANDLE pHandle, PVOID dst, PVOID buffer, SIZE_T size, BOOLEAN drvIs ) {
	if ( !WriteProcessMemory( pHandle, dst, buffer, size, 0 ) ) {
		printf( "Error write %p in process handle: GetLastError - %p \n", dst, pHandle, GetLastError() );
	}
}
PVOID  read                                  ( HANDLE pHandle, PVOID dst, SIZE_T size, BOOLEAN drvIs ) {
	PVOID buffer = NULL;
	if ( !ReadProcessMemory( pHandle, dst, &buffer, size, 0 ) ) {
		printf( "Error read %p in process handle %p: GetLastError - %p \n", dst, pHandle, GetLastError() );
	}

	return buffer;
}
DWORD  change_protect_memory                 ( HANDLE pHandle, PVOID dst, size_t size, DWORD access ) {
	DWORD old = 0;

	if ( !VirtualProtectEx( pHandle, dst, size, access, &old ) ) {
		err( "Error change_protect_memory %p in process handle: %p GetLastError - %p \n", dst, pHandle, GetLastError() );
	}

	return old;
}

MEMORY_BASIC_INFORMATION memory_basic_info   ( HANDLE pHandle, PVOID dst) {
	MEMORY_BASIC_INFORMATION mbi;
	if ( !VirtualQueryEx( pHandle, dst, &mbi, sizeof( mbi ) ) ) {
		err( "Error: VirtualQueryEx address: %p handle: %p : GetLastError %i \n", dst, pHandle,GetLastError() );
		return mbi;
	}

	return mbi;
}
