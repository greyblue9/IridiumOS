#include <stdint.h>
#include <stddef.h>
#include <common/asm.h>
#include <common/log.h>
#include <intr/intr.h>
#include <arch/mm/vmm.h>
#include <arch/mm/pmm.h>
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
    pmm_init();
    vmm_init();
    done();
}


int _start(void)
{
    CLI;
    init();
    done();
}
