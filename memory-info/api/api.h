#pragma once
#include <Windows.h>
#include <TlHelp32.h>

DWORD  get_pid_by_process_name             ( const char* pName );
HANDLE get_phandle                         ( DWORD pId );

void   write                               ( HANDLE pHandle, PVOID dst, PVOID buffer, SIZE_T size,BOOLEAN drvIs);
PVOID  read                                ( HANDLE pHandle, PVOID dst, SIZE_T size, BOOLEAN drvIs );
DWORD  change_protect_memory               ( HANDLE pHandle, PVOID dst, size_t size, DWORD access );
MEMORY_BASIC_INFORMATION memory_basic_info ( HANDLE pHandle, PVOID dst);