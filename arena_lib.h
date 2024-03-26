/*
MIT License

Copyright (c) 2023 Daniel Goldenberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#ifndef ARENA_LIB_H
#define ARENA_LIB_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define KiB(bytes) ((size_t) bytes << 10)
#define MiB(bytes) ((size_t) bytes << 20)
#define GiB(bytes) ((size_t) bytes << 30)

typedef struct Arena Arena;
struct Arena
{
  char *memory;
  size_t size;
  size_t used;
};

Arena create_arena(size_t size)
{
  Arena arena;
  arena.memory = malloc(size);
  arena.size = size;
  arena.used = 0;

  return arena;
}

void destroy_arena(Arena *arena)
{ 
  free(arena->memory);
  arena = (void *) 0;
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

#endif
