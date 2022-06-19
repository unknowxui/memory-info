# Memory-info
Program manages memory

# Functional
 - Write Process memory
 - Read Process  memory
 - Allocate virtual memory
 - Memory basic information
 - Change Memory protect
 - Inject dll

# Remarks
If when using the command .useDrv 1 there are functions in which it is possible to use kernel functions, it will use them
Let's say that in the following scenario, the program will use the kernel function to get the process handle:
```
memory-info :>.useDrv 1
memory-info :>.attach 3348
[ Info ] Command attach .attach !
[ Info ] Attached -> 3348
```
