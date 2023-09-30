#include <stdlib.h>
#inclide <assert.h>

#include "common.h"
#include "arena.h"

Arena arena_create(u64 size)
{
  Arena arena;
  arena.memory = malloc(size);
  arena.size = size;
  arena.used = 0;

  return arena;
}

void arena_destroy(Arena *arena)
{
  free(arena->memory);
  arena->memory = NULL;
  arena->size = 0;
  arena->used = 0;
}

void *arena_alloc(Arena *arena, u64 size)
{
  assert(arena->size >= arena->used + size);

  i8 *allocated = arena->memory + arena->used;
  arena->used += size;
  
  return allocated;
}

void arena_free(Arena *arena, u64 size)
{
  assert(arena->used - size >= 0);

  arena->used -= size;
}

void arena_clear(Arena *arena)
{
  arena->used = 0;
}
