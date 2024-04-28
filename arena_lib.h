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

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef ARENA_ALIGN_SIZE
#define ARENA_ALIGN_SIZE 8
#endif

typedef uint8_t aa_bool;

typedef struct Arena Arena;

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

static inline
char *_arena_align_ptr(char *ptr, int32_t align, int32_t *offset)
{
	uintptr_t result = (uintptr_t) ptr;
	int32_t modulo = result & ((uintptr_t) (align) - 1);
	if (modulo != 0)
  {
    *offset = align - modulo;
		result += *offset;
	}

	return (char *) result;
}

static inline
Arena create_arena(size_t size)
{
  Arena arena;
  arena.memory = malloc(size);
  arena.size = size;
  arena.used = 0;

  return arena;
}

static inline
void destroy_arena(Arena *arena)
{ 
  free(arena->memory);
  arena = (void *) 0;
}

static inline
void *arena_alloc(Arena *arena, size_t size)
{
  assert(arena->size >= arena->used + size + ARENA_ALIGN_SIZE);

  char *allocated = arena->memory + arena->used;
  int32_t offset;
  allocated = _arena_align_ptr(allocated, ARENA_ALIGN_SIZE, &offset);
  arena->used += size + offset;
  
  return allocated;
}

static inline
void arena_free(Arena *arena, size_t size)
{
  assert(arena->used - size >= 0);
  arena->used -= size;
}

static inline
void clear_arena(Arena *arena)
{
  arena->used = 0;
}

#endif
