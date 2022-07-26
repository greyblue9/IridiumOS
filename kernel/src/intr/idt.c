/*
 * MIT License
 *
 * Copyright (c) 2022 IridiumProject
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <intr/idt.h>

static struct IDTR idtr;
static struct InterruptGateDescriptor idt[256];


void idt_set_desc(uint8_t vec, void* isr, uint32_t flags)
{
    uint64_t addr = (uint64_t)isr;
    struct InterruptGateDescriptor* vector = &idt[vec];

    vector->isr_low16 = addr & 0xFFFF;
    vector->isr_mid16 = (addr >> 16) & 0xFFFF;
    vector->isr_high32 = (addr >> 32);
    vector->ist = 0;
    vector->zero = 0;
    vector->zero1 = 0;
    vector->zero2 = 0;
    vector->reserved;
    vector->p = 1;
    vector->dpl = 3;
    vector->cs = 0x28;
    vector->attr = flags;
}

void idt_install(void)
{
    idtr.limit = sizeof(struct InterruptGateDescriptor) * 256 - 1;
    idtr.base = (uint64_t)&idt;
    __asm__ __volatile__("lidt %0" :: "m" (idtr));
}
