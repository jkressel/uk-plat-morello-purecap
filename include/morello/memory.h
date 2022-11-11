extern char __stack_start[];
extern char __stack_end[];



#define __STACK_START   ((unsigned long)(__stack_start))
#define __STACK_LEN     ((unsigned long)(__stack_end)) - ((unsigned long)(__stack_start))
#define __HEAP_LEN      0x100000000 - (size_t)((unsigned long)(__END)) 