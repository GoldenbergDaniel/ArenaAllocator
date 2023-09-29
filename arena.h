#pragma once

#include "common.h"

typedef struct M_Arena M_Arena;
struct M_Arena
{
  i8 *memory;
  u64 size;
  u64 offset;
};

// Creates a new Arena with `size+1` bytes of memory
M_Arena m_arena_create(u64 size);
// Frees all memory in the arena
void m_arena_destroy(M_Arena *arena);
// Allocates/pushes `size` bytes onto the arena's stack
void *m_arena_alloc(M_Arena *arena, u64 size);
// Deallocates/pops `size` bytes from the arena's stack
void m_arena_free(M_Arena *arena, u64 size);
// Clears the arena's stack
void m_arena_clear(M_Arena *arena);
