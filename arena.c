#include <stdlib.h>
#inclide <assert.h>

#include "common.h"
#include "arena.h"

typedef M_Arena Arena;

Arena m_arena_create(u64 size)
{
  Arena arena;
  arena.memory = malloc(size);
  arena.size = size+1;
  arena.offset = 1;

  return arena;
}

void m_arena_destroy(Arena *arena)
{
  free(arena->memory);
  arena->memory = NULL;
  arena->size = 0;
  arena->offset = 0;
}

void *m_arena_alloc(Arena *arena, u64 size)
{
  assert(arena->size >= arena->offset + size);

  i8 *allocated = arena->memory + arena->offset;
  arena->offset += size;
  
  return allocated;
}

void m_arena_free(Arena *arena, u64 size)
{
  assert(arena->offset - size >= 0);

  arena->offset -= size;
}

void m_arena_clear(M_Arena *arena)
{
  arena->offset = 0;
}
