#ifndef ARENA_LIB_H
#define ARENA_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Arena Arena;

#define KiB(bytes) ((size_t) bytes << 10)
#define MiB(bytes) ((size_t) bytes << 20)
#define GiB(bytes) ((size_t) bytes << 30)

#ifndef SCRATCH_SIZE
#define SCRATCH_SIZE MiB(16)
#endif

struct Arena
{
  char *memory;
  size_t size;
  size_t used;
};

Arena create_arena(size_t size)
{
  Arena arena = {0};
  arena.memory = malloc(size);
  arena.size = size;

  return arena;
}

void destroy_arena(Arena *arena)
{
  free(arena->memory);
  arena = NULL;
}

void *arena_alloc(Arena *arena, size_t size)
{
  assert(arena->size >= arena->used + size);

  char *allocated = arena->memory + arena->used;
  arena->used += size;
  
  return allocated;
}

void arena_free(Arena *arena, size_t size)
{
  assert(arena->used - size >= 0);
  
  arena->used -= size;
}

void clear_arena(Arena *arena)
{
  arena->used = 0;
}

Arena get_scratch_arena(Arena *conflict)
{
  static __thread Arena scratch_1;
  static __thread Arena scratch_2;
  static __thread bool init = true;

  if (init)
  {
    scratch_1 = create_arena(SCRATCH_SIZE);
    scratch_2 = create_arena(SCRATCH_SIZE);
    init = false;
  }

  Arena scratch = scratch_1;
  
  if (conflict == &scratch_1)
  {
    scratch = scratch_2;
  }
  else if (conflict == &scratch_2)
  {
    scratch = scratch_1;
  }

  return scratch;
}

#endif
