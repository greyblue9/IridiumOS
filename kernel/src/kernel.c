#include <stdint.h>
#include <stddef.h>
#include <libkern/asm.h>
#include <libkern/log.h>
#include <limine.h>


static void done(void)
{
    while (1)
    {
        HLT;
    }
}


static void init(void)
{
    kprintf(KINFO "Yo yo yo!\n");
    done();
}


int _start(void)
{
    init();
    done();
}
