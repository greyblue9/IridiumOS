#include <stdint.h>
#include <stddef.h>
#include <common/asm.h>
#include <common/log.h>
#include <intr/intr.h>
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
    intr_init();
    kprintf(KINFO "Booting..\n");
    done();
}


int _start(void)
{
    CLI;
    init();
    done();
}
