#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/driver.h"

#include "api/defs.h"
#include "api/api.h"

#pragma warning( disable : 4996)
#pragma warning( disable : 6054)
#pragma warning( disable : 6031)
#pragma warning( disable : 6328)
#pragma warning( disable : 6066)

//
// If this is null some command not run
// 
DWORD  attachedProcessPid   = 0;

//
// If this is null some command not run
// 
HANDLE attachProcessHandle  = 0;

//
// If this is 1 when used driver functino 
//
int isUseDriver = 0;





int main(int argc,char* argv[]) {
    printf_s( "Entry ! \n" );
   
    //
    // If exitApp is true application exit
    //
    BOOLEAN exitApp = 0;

    if ( argc != 1 ) {
        if ( !strcmp( argv[1], "-help") ) {
            printf( "Commands: \n" );
            printf( ".attach        <pId>        ; Attach process by process id \n " );
            printf( ".deattach      <pId>        ; Deattach process if he attached \n " );
            printf( ".clear         <any number> ; Clear console \n" );
            printf( ".protect       <any number> ; Depending on the selection shows protection, region size, base allocate \n" );
            printf( ".changeProtect <any number  ; Change address protect and show old protect > \n" );
        }
    }

    //
    // Main loop
    //
    while ( !exitApp ) {
        printf( "memory-info :>" );

        char  line[256];
        DWORD pId;

        memset( line, 0, 256 );
        
        //
        // User command input
        //
        scanf( "%s %i", line, &pId);

        //
        // Attach process by pId
        //
        if ( !strcmp( line, ".attach" ) ) {
#ifdef _DEBUG
            info( "Command attach %s ! \n", line );
#endif
            if ( pId ) {
                HANDLE pHandle = 0;
                if ( isUseDriver ) {

                    // 
                    // If driver loaded when return process handle
                    //
                    pHandle = drv_get_process_handle( pId );

                } else {

                    pHandle = get_phandle( pId );

                }

                if ( pHandle == INVALID_HANDLE_VALUE || !pHandle ) {
                    err( "Error: process not open ! %i \n", GetLastError() );
                    continue;
                }

                attachedProcessPid  = pId;
                attachProcessHandle = pHandle;

                info( "Attached -> %i \n", pId );
            }
#ifdef _DEBUG
            else {
                err( "Error: pId for attach 0 ! \n" );
            }
#endif
        }

        //
        // Use driver function
        //
        else if ( !strcmp( line, ".useDrv" ) ) {
            isUseDriver = pId;
        }

        //
        // Inject dll
        //
        else if ( !strcmp( line, ".inject" ) ) {
            if ( !attachedProcessPid ) {
                err( "Pls use .attach for get pid ! \n" );
                continue;
            }

            char path[256];

            printf( "\n Dll path>:" );
            scanf( "%s", path );

            inject( attachedProcessPid, path );
        }

        //
        // Deattach 
        //
        else if ( !strcmp( line, ".deattach" ) ) {
            if ( attachedProcessPid == pId ) {
                info( "Deattach -> %i \n", attachedProcessPid );
                attachedProcessPid ^= attachedProcessPid;
            }
        }

        // 
        // Change memory protect used VirtualProtect
        //
        else if ( !strcmp( line, ".changeProtect" ) ) {
            if ( !attachProcessHandle ) {
                err( "Error: attachProcessHandle is null pls use .attach <processId> \n" );
                continue;
            }

            info( "Enter pls address and new protect, if you need out protect new protect will be null \n" );
            
            DWORD  newProtect;
            PVOID  address;
            int    size;

            info( "Protect->Address->Size: ");
            scanf( "%p %p %i",&newProtect, &address,&size );

            DWORD old = 0;

            if ( isUseDriver ) {
                info( "drv_protect_memory is use driver ! \n" );
                old = drv_protect_memory( attachedProcessPid, address, size, newProtect );

            } else {
                old = change_protect_memory( attachProcessHandle, address, size, newProtect );
                if ( old ) {
                    info( "Change protect Success ! \n" );
                    info( "change_protect_memory: old protect %i \n", old );
                }
            }
        }

        //
        // Use system("cls")
        //
        else if ( !strcmp( line, ".clear" ) ) {
            system( "cls" );
        }

        // 
        // For print memory info
        //
        else if ( !strcmp( line, ".protect" ) ) {
            if ( !attachProcessHandle ) {
                err( "Error: attachProcessHandle is null pls use .attach <processId> \n" );
                continue;
            }

            info( "Enter what if you need ! \n" );

            info( "Address: " );
            PVOID address;

            scanf( "%p", &address );
            if ( !address ) {
                err( "Address is null ! \n" );
                continue;
            }

            MEMORY_BASIC_INFORMATION memInfo;
            if(isUseDriver ){
                memInfo = drv_memory_basic_information( attachedProcessPid, address );
            } else {
                memInfo = memory_basic_info( attachProcessHandle, address );
            }

            if ( !memInfo.BaseAddress )
                continue;

            info( "Protect: 1 BaseAddress: 2 RegionSize: 3 Exit: 4\n" );
        AgainIf:
            int type;
            printf( "type:> " );
            scanf( "%i", &type );
            switch ( type ) {
                case 1:
                {
                    info( "Protect: %p \n", memInfo.AllocationProtect );
                    break;
                }
                case 2:
                {
                    info( "BaseAddress: %p \n", memInfo.BaseAddress );
                    break;
                }
                case 3:
                {
                    info( "RegionSize: %i \n", memInfo.RegionSize );
                    break;
                }
                case 4:
                {
                    continue;
                }

                default:
                    break;
            }
            goto AgainIf;
        }

        //
        // Write memory
        // 
        else if ( !strcmp( line, ".write" ) ) {
            if ( !attachedProcessPid ) {
                err( "Pls use .attach for open handle process ! \n" );
                continue;
            }

            printf( "Address->Size->Type: " );

            PVOID address;
            int   size;
            char  type[10];

            char        dataStr[10];
            int         dataInt;
            float       dataFloat;

            scanf( "%p %i %s", &address, &size, type );
            if ( !strcmp( type, "int" ) ) {
                printf( "\t Data: " );
                scanf( "%i", &dataInt );

                drv_write_memory( attachedProcessPid, address, &dataInt, size );
            }
            else if ( !strcmp( type, "float" ) ) {
                printf( "\t Data: " );
                scanf( "%f", &dataFloat );

                drv_write_memory( attachedProcessPid, address, &dataFloat, size );
            } else if ( !strcmp( type, "str" ) ) {
                printf( "\t Data: " );
                scanf( "%s", &dataStr );

                drv_write_memory( attachedProcessPid, address, &dataStr, size );
            }

        }

        // For exit program
        //
        else if ( !strcmp( line, ".exit" ) ) {
            exitApp = 1;
        }
        else {
            err( "Command not found ! \n" );
        }
        printf( "\n\n" );
    }

    printf_s( "End ! \n" );
    return 0;
}

