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

#include <arch/mm/vmm.h>
#include <arch/mm/pmm.h>
#include <arch/memory/mem.h>
#include <common/string.h>
#include <common/log.h>
#include <stddef.h>

static uint64_t* pml4;


static inline void invlpg(void* page_base)
{
    __asm__ __volatile__("invlpg (%0)" :: "r" (page_base));
}

static void* vmm_alloc_page(void)
{
    uint64_t frame = (uint64_t)pmm_alloc();

    if (frame == 0)
    {
        return NULL;
    }
    return (void*)((uint64_t)frame & PAGE_ADDR_MASK);
}

void vmm_map_page(uint64_t* pml4, void* logical, uint32_t flags)
{
    uint64_t addr = (uint64_t)logical;
    addr &= PAGE_ADDR_MASK;

    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

    if (!(pml4[pml4_idx] & PAGE_PRESENT))
    {
        uint64_t pdpt = (uint64_t)vmm_alloc_page();
        memzero((void*)pdpt, PAGE_SIZE);
        pml4[pml4_idx] = (pdpt & PAGE_ADDR_MASK) | flags;
        vmm_map_page(pml4, (void*)pdpt, flags);
    }

    uint64_t* pdpt = (uint64_t*)(pml4[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
    {
        uint64_t pd = (uint64_t)vmm_alloc_page();
        memzero((void*)pd, PAGE_SIZE);
        pdpt[pdpt_idx] = (pd & PAGE_ADDR_MASK) | flags;
        vmm_map_page((void*)pd, (void*)pd, flags);
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pd[pd_idx] & PAGE_PRESENT))
    {
        uint64_t pt = (uint64_t)vmm_alloc_page();
        memzero((void*)pt, PAGE_SIZE);
        pd[pd_idx] = (pt & PAGE_ADDR_MASK) | flags;
        vmm_map_page((void*)pt, (void*)pt, flags);
    }

    uint64_t* pt = (uint64_t*)(pd[pd_idx] & PAGE_ADDR_MASK);

    if (!(pt[pt_idx] & PAGE_PRESENT))
    {
        pt[pt_idx] = ((uint64_t)pmm_allocz() & PAGE_ADDR_MASK) | flags;
        invlpg((void*)addr);
    }
    else
    {
        pt[pt_idx] |= flags;
    }
}

static uint64_t* read_cr3(void)
{
    static uint64_t data;
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (data));
    return (uint64_t*)(uint64_t)PHYS_TO_HIGHER_HALF_DATA(data);
}

uint64_t* vmm_mkpml4(void)
{

    uint64_t* new_pml4 = vmm_alloc_page();
    uint64_t* boot_pml4 = read_cr3();
    
    new_pml4[0] = 1;

    for (uint16_t i = 0; i < 512; ++i)
    {
        new_pml4[i]  = boot_pml4[i];
    }

    return (void*)((uint64_t)new_pml4 & PAGE_ADDR_MASK);
}


void vmm_init(void)
{
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (pml4));
    pml4 = vmm_mkpml4();
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (pml4));
    kprintf("<VMM>: Loaded CR3 with PML4.\n");

}
