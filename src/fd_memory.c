/*
 * Internal allocation tracker inspired by LALSuite's memory-debugging API.
 * Adapted for Freedroid to report project-owned leaks with allocation sites.
 *
 * Original idea and API style derived from LALSuite, which is also GPL-licensed.
 */

#define _fd_memory_c

#include "system.h"

#include "defs.h"
#include "proto.h"

typedef struct memory_block {
  void *ptr;
  size_t size;
  const char *file;
  int line;
  struct memory_block *next;
} memory_block;

static memory_block *live_blocks = NULL;
static size_t live_bytes = 0;
static size_t live_count = 0;
static size_t peak_bytes = 0;

static void
MemoryTrackerFatal (const char *func, size_t amount)
{
  fprintf (stderr, "Freedroid: %s(%zu) did not succeed!\n", func, amount);
  exit (ERR);
}

static memory_block **
FindTrackedBlock (void *ptr)
{
  memory_block **block = &live_blocks;

  while (*block != NULL)
    {
      if ((*block)->ptr == ptr)
        return block;
      block = &((*block)->next);
    }

  return NULL;
}

static void *
TrackBlock (void *ptr, size_t size, const char *file, int line)
{
  memory_block *block;

  if (ptr == NULL)
    return NULL;

  block = malloc (sizeof (*block));
  if (block == NULL)
    {
      free (ptr);
      MemoryTrackerFatal ("memory tracker malloc", sizeof (*block));
    }

  block->ptr = ptr;
  block->size = size;
  block->file = file;
  block->line = line;
  block->next = live_blocks;
  live_blocks = block;

  live_count++;
  live_bytes += size;
  if (live_bytes > peak_bytes)
    peak_bytes = live_bytes;

  return ptr;
}

void *
MyMallocLong (long amount, const char *file, int line)
{
  size_t size;
  void *ptr;

  if (amount <= 0)
    amount = 1;

  size = (size_t) amount;
  ptr = calloc (1, size);
  if (ptr == NULL)
    MemoryTrackerFatal ("MyMalloc", size);

  return TrackBlock (ptr, size, file, line);
}

void *
MyCallocLong (size_t count, size_t size, const char *file, int line)
{
  void *ptr;
  size_t total;

  if (count == 0)
    count = 1;
  if (size == 0)
    size = 1;

  total = count * size;
  ptr = calloc (count, size);
  if (ptr == NULL)
    MemoryTrackerFatal ("MyCalloc", total);

  return TrackBlock (ptr, total, file, line);
}

void *
MyReallocLong (void *ptr, size_t size, const char *file, int line)
{
  memory_block **block;
  void *new_ptr;

  if (ptr == NULL)
    return MyCallocLong (1, size, file, line);

  if (size == 0)
    size = 1;

  block = FindTrackedBlock (ptr);
  new_ptr = realloc (ptr, size);
  if (new_ptr == NULL)
    MemoryTrackerFatal ("MyRealloc", size);

  if (block == NULL)
    return TrackBlock (new_ptr, size, file, line);

  live_bytes -= (*block)->size;
  (*block)->ptr = new_ptr;
  (*block)->size = size;
  (*block)->file = file;
  (*block)->line = line;
  live_bytes += size;
  if (live_bytes > peak_bytes)
    peak_bytes = live_bytes;

  return new_ptr;
}

void
MyFreeLong (void *ptr, const char *file, int line)
{
  memory_block **block;

  (void) file;
  (void) line;

  if (ptr == NULL)
    return;

  block = FindTrackedBlock (ptr);
  if (block != NULL)
    {
      memory_block *dead_block = *block;
      *block = dead_block->next;
      live_count--;
      live_bytes -= dead_block->size;
      free (dead_block);
    }

  free (ptr);
}

void
MyCheckMemoryLeaks (void)
{
  memory_block *block;

  if (live_blocks == NULL)
    return;

  fprintf (stderr, "\nFreedroid internal memory tracker: %zu leak(s), %zu byte(s) still live"
           " (peak tracked usage %zu byte(s)).\n",
           live_count, live_bytes, peak_bytes);

  block = live_blocks;
  while (block != NULL)
    {
      memory_block *next = block->next;
      fprintf (stderr, "  leak: %zu byte(s) allocated at %s:%d (%p)\n",
               block->size, block->file, block->line, block->ptr);
      free (block->ptr);
      free (block);
      block = next;
    }

  live_blocks = NULL;
  live_bytes = 0;
  live_count = 0;
}
