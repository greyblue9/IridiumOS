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


#include <arch/memory/mem.h>
#include <limine.h>
#include <common/log.h>
#include <common/panic.h>
#include <common/string.h>
#include <stddef.h>
#include <stdint.h>

#define PMM_MAGIC 0xC001CA7
#define DATA_START(frame) (((char*) frame) + sizeof(struct Frame))

struct Frame
{
    uint8_t is_free : 1;
    struct Frame* next;
    uint32_t magic;
};

static struct Frame* mem_head = NULL;
static struct Frame* mem_tail = NULL;
static uint64_t start_addr = 0;
static uint64_t mem_left = 0;


volatile struct limine_memmap_request mmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static struct Frame* first_fit(void)
{
    struct Frame* cur_frame = mem_head;

    while (cur_frame != NULL)
    {
        if (cur_frame->is_free)
        {
            return cur_frame;
        }

        cur_frame = cur_frame->next;
    }

    // No more frames left.
    return NULL;
}

void* pmm_alloc(void)
{
    if (mem_left <= 0)
    {
        return NULL;
    }

    struct Frame* frame = first_fit();

    if (frame == NULL)
    {
        char* next = DATA_START(mem_tail + PAGE_SIZE);
        mem_tail->next = (struct Frame*)next;
        frame = mem_tail->next;
        frame->is_free = 0;
        frame->next = NULL;
        frame->magic = PMM_MAGIC;
        mem_tail = frame;
        mem_left -= PAGE_SIZE;
    }

    const void* PHYS = (void*)(uint64_t)HIGHER_HALF_DATA_TO_PHYS((uint64_t)ALIGN_UP((uint64_t)frame, PAGE_SIZE));
    return DATA_START(PHYS);
}


void* pmm_allocz(void)
{
    void* alloc = pmm_alloc();
    memzero(PHYS_TO_HIGHER_HALF_DATA(alloc), PAGE_SIZE);
    return alloc;
}


void pmm_free(void* frame)
{
    struct Frame* cur_frame = mem_head;

    while (cur_frame != NULL && frame != DATA_START(cur_frame))
    {
        cur_frame = cur_frame->next;
    }

    cur_frame->is_free = 1;
}

void pmm_init(void)
{

    // First biggest segment
    struct limine_memmap_entry* biggest_segment = NULL;

    // Find largest segment to hold our bitmap.
    for (uint64_t i = 0; i < mmap_req.response->entry_count; ++i)
    {
        struct limine_memmap_entry* current = mmap_req.response->entries[i];

        // If we haven't set our biggest segment and current->type is usable memory,
        // set biggest segment and continue.
        if (biggest_segment == NULL && current->type == LIMINE_MEMMAP_USABLE)
        {
            biggest_segment = current;
            continue;
        }

        if (current->type == LIMINE_MEMMAP_USABLE && current->length > biggest_segment->length)
        {
            biggest_segment = current;
        }
    }

    if (biggest_segment == NULL)
    {
        kprintf(KERN_PANIC "Not enough memory!\n");
        __asm__ __volatile__("cli; hlt");
    }

    start_addr = biggest_segment->base;
    mem_head = (struct Frame*)PHYS_TO_HIGHER_HALF_DATA(start_addr);
    mem_head->next = NULL;
    mem_head->is_free = 0;
    mem_head->magic = PMM_MAGIC;
    mem_tail = mem_head;
    mem_left = biggest_segment->length;
}
