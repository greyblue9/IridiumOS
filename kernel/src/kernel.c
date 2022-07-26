#include <stdint.h>
#include <stddef.h>
#include <common/asm.h>
#include <common/log.h>
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
    done();
}


int _start(void)
{
    init();
    done();
}
