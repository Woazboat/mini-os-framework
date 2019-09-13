#include "stdint.h"
#include "string.h"

#ifndef OS_NAME
#define OS_NAME "MiniOS"
#endif

#define FRAMEBUFFER_ADDR ((char*)0xB8000)

void entry();
void boot();
void debugprint(const char* p);
void putc(const char c);
void puts(const char *str);


static char* fb_pos = FRAMEBUFFER_ADDR;
void *multiboot_info;
extern char boot_stack_top_address;


__attribute__((naked)) void entry()
{
        asm("movl %[stack], %%esp\n"
            "movl %%esp, %%ebp\n"
            "pushl $0\n"
            "popf\n"
            "pushl %%ebx\n"
            "calll boot\n"
            :[mbootinfo]"=m"(multiboot_info)
            :[stack]"c"(&boot_stack_top_address)
            :"memory", "ebx");
}

void boot(void* multiboot_info_ptr)
{
        multiboot_info = multiboot_info_ptr;
        // Clear framebuffer
        memset(FRAMEBUFFER_ADDR, 0, 80 * 25 * 2);

        debugprint("Hello " OS_NAME "!\n");
        puts("Hello " OS_NAME "!");

        while(1)
                asm("hlt\n");
}





// Print to QEMU/Bochs console
void debugprint(const char* p)
{
        while (*p)
                asm volatile ("outb %b0, %w1\n"
                              ::"a"(*p++),
                                "d"(0xe9));
}

void puts(const char *str)
{
        char c = *str;
        while((c = *str++))
                putc(c);
}

void putc(const char c)
{
        fb_pos[0] = c;
        fb_pos[1] = 0x02;
        fb_pos += 2;
}
