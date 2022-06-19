#pragma once
#include <Windows.h>

#include "struct.h"


typedef uintptr_t( __stdcall* CallHook)(PVOID);

//
// Call hook in kernel
//
static
uintptr_t 
call_hook( PVOID arguments ) {
    LoadLibrary( "user32.dll" );

    void* control_function = GetProcAddress( LoadLibrary( "win32u.dll" ), "NtOpenCompositionSurfaceSectionInfo" );
    const CallHook control = ( CallHook )(control_function);

    return control( arguments);
}


//
// Get process handle by process id
//
static
HANDLE 
drv_get_process_handle(DWORD pId) {
    DRIVERCONTROL rqst = { 0 };
    rqst.rqstType = GET_HANDLE;
    rqst.pId      = ( HANDLE )pId;

    call_hook( &rqst );

    return rqst.GetHandle.pHandle;
}

//
// Allocate virtual memory in driver
//
static
PVOID 
drv_allocate_virtual_memory(DWORD pId,size_t size) {
    PVOID base = 0;

    DRIVERCONTROL rqst = { 0 };
    rqst.pId = ( HANDLE )pId;
    rqst.rqstType = ALLOCATE;
    rqst.AllocateVM.size = size;
    rqst.AllocateVM.base = base;
    rqst.AllocateVM.aType = MEM_RESERVE | MEM_COMMIT;
    rqst.AllocateVM.protect = PAGE_READWRITE;

    call_hook( &rqst );

    return rqst.AllocateVM.base;
}

//
// Write Process memory
//
static
void 
drv_write_memory(DWORD pId, PVOID base, PVOID buffer, SIZE_T size ) {
    SIZE_T retSize = 0;

    DRIVERCONTROL rqst = { 0 };
    rqst.rqstType = WRITE;
    rqst.pId = ( HANDLE )pId;
    rqst.Write.dst = buffer;
    rqst.Write.src = base;
    rqst.Write.size = size;
    rqst.Write.retSize = retSize;

    call_hook( &rqst );
}

//
// Read process memory
//
static
PVOID 
drv_read_memory( DWORD pId, PVOID base, size_t size ) {
    PVOID buffer = 0;

    DRIVERCONTROL rqst = { 0 };
    rqst.rqstType = READ;
    rqst.pId = ( HANDLE )pId;
    rqst.Read.base = base;
    rqst.Read.buffer = &buffer;
    rqst.Read.size = size;

    call_hook( &rqst );

    return buffer;
}

//
// Memory basic information
//
static
MEMORY_BASIC_INFORMATION 
drv_memory_basic_information( DWORD pId, PVOID base ) {
    size_t retLen = 0;
    MEMORY_BASIC_INFORMATION mbi = { 0 };

    DRIVERCONTROL rqst = { 0 };
    rqst.rqstType = QUEY_INFO;
    rqst.pId = ( HANDLE )pId;
    rqst.QueryInforamtion.base = base;
    rqst.QueryInforamtion.returnLength = retLen;
    rqst.QueryInforamtion.mbi = mbi;

    call_hook( &rqst );

    return rqst.QueryInforamtion.mbi;
}

//
// Change memory protect
//
static
DWORD 
drv_protect_memory( DWORD pId, PVOID address, size_t size, DWORD protect ) {
    DWORD old = 0;
    DRIVERCONTROL rqst = { 0 };
    rqst.rqstType = CHANGE_PROTECT;
    rqst.pId = ( HANDLE )pId;
    rqst.ChangeProtect.address = address;
    rqst.ChangeProtect.size = size;
    rqst.ChangeProtect.protect = protect;
    rqst.ChangeProtect.oldProtect = old;

    call_hook( &rqst );

    return rqst.ChangeProtect.oldProtect;
}