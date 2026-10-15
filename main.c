#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>

void* dlsym(void*, const char*);
void* memcpy(void * __restrict, const void * __restrict, size_t);

static int publish_hypercore_state(int ready)
{
    mkdir("/tmp", 0777);
    int fd = open("/tmp/kstuff_ready", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0)
        return -1;
    const char value = '1';
    ssize_t written = write(fd, &value, sizeof(value));
    int close_result = close(fd);
    return written == (ssize_t)sizeof(value) && close_result == 0 ? 0 : -1;
}

void notify(const char* s)
{
    struct
    {
        char pad1[0x10];
        int f1;
        char pad2[0x19];
        char msg[0xc03];
    } notification = {.f1 = -1};
    char* d = notification.msg;
    while((*d++ = *s++));
    int fd = open("/dev/notification0", 1);
    write(fd, &notification, 0xc30);
    close(fd);
}

void die(int line)
{
    notify("problem encountered on main.c");
    publish_hypercore_state(0);
    asm volatile("ud2");
}

#define die() die(__LINE__)

// Global variable tracking the base kernel address layout
uint64_t kdata_base = 0;

static int strcmp(const char* a, const char* b)
{
    while(*a && *a == *b)
    {
        a++;
        b++;
    }
    return *a - *b;
}

// 13.60 Kernel Privilege Verification Patch Entry Point
void init_1360_fpkg_bypass(void) {
    // If the base memory coordinate is initialized, apply the bypass inline
    if (kdata_base != 0) {
        // Calculate the absolute destination virtual address from your verified offset map
        uint64_t patch_address = kdata_base + 0x001e0e0a7;

        // Pure Inline Assembly: Clears CR0 Write-Protect bit, injects success bytes, restores CR0
        __asm__ volatile(
            "mov %%cr0, %%rax\n"
            "mov %%rax, %%rdx\n"
            "and $0xfffffffffffeffff, %%rax\n"
            "mov %%rax, %%cr0\n"
            "movb $0x31, (%0)\n"
            "movb $0xC0, 1(%0)\n"
            "movb $0xC3, 2(%0)\n"
            "mov %%rdx, %%cr0\n"
            :
            : "r"(patch_address)
            : "rax", "rdx", "memory"
        );
    }
}
