#include <stdlib.h>
#inclide <assert.h>

#include "common.h"
#include "arena.h"

typedef M_Arena Arena;

Arena m_arena_create(u64 size)
{
  Arena arena;
  arena.memory = malloc(size);
  arena.size = size;
  arena.used = 0;

  return arena;
}

void m_arena_destroy(Arena *arena)
{
  free(arena->memory);
  arena->memory = NULL;
  arena->size = 0;
  arena->used = 0;
}

void *m_arena_alloc(Arena *arena, u64 size)
{
  assert(arena->size >= arena->used + size);

  i8 *allocated = arena->memory + arena->used;
  arena->used += size;
  
  return allocated;
}

void m_arena_free(Arena *arena, u64 size)
{
  assert(arena->used - size >= 0);

  arena->used -= size;
}

void m_arena_clear(M_Arena *arena)
{
  arena->used = 0;
}
