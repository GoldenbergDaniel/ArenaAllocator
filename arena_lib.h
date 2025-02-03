#pragma once

#ifndef ARENA_LIB_H
#define ARENA_LIB_H

#include <stdint.h>
#include <assert.h>

#if defined(_WIN32)
#define PLATFORM_WINDOWS
#elif defined (__linux__) || defined(__APPLE__)
#define PLATFORM_POSIX
#endif

#if defined(__GNUC__)
#define COMPILER_CLANG
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#endif

#if defined(PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(PLATFORM_POSIX)
#include <unistd.h>
#include <sys/mman.h>
#endif

#if defined(COMPILER_CLANG)
#define thread_local __thread
#define read_only __attribute__((section(_RO_SECTION_NAME)))
#elif defined(COMPILER_MSVC)
#define thread_local __declspec(thread)
#endif

// OS ////////////////////////////////////////////////////////////////////////////////////

void *_arena_os_reserve_vm(void *addr, uint64_t size)
{
  void *result = NULL;
  
#ifdef PLATFORM_WINDOWS
  result = VirtualAlloc(addr, size, MEM_RESERVE, PAGE_NOACCESS);
#endif

#ifdef PLATFORM_POSIX
  result = mmap(NULL, size, PROT_NONE, MAP_ANON | MAP_PRIVATE, -1, 0);
#endif

  return result;
}

uint8_t _arena_os_commit_vm(void *addr, uint64_t size)
{
  uint8_t result = 1;

#ifdef PLATFORM_WINDOWS
  byte *ptr = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
  if (ptr == NULL)
  {
    result = GetLastError();
  }
#endif

#ifdef PLATFORM_POSIX
  int32_t err = mprotect(addr, size, PROT_READ | PROT_WRITE);
#endif

  return result;
}

int8_t _arena_os_decommit_vm(void *addr, uint64_t size)
{
  int8_t result = 1;

#ifdef PLATFORM_WINDOWS
  result = VirtualFree(addr, size, MEM_DECOMMIT);
#endif

#ifdef PLATFORM_POSIX
  int32_t err = mprotect(addr, size, PROT_NONE);
#endif

  return result;
}

void _arena_os_release_vm(void *ptr, uint64_t size)
{
#ifdef PLATFORM_WINDOWS
  VirtualFree(ptr, size, MEM_RELEASE);
#endif

#ifdef PLATFORM_POSIX
  munmap(ptr, size);
#endif
}

// TODO(dg): This should be cached somewhere
uint64_t _arena_os_get_page_size(void)
{
  uint64_t result = 0;

#ifdef PLATFORM_WINDOWS
  SYSTEM_INFO info = {0};
  GetSystemInfo(&info);
  result = info.dwPageSize;
#endif

#ifdef PLATFORM_POSIX
  result = getpagesize();
#endif

  return result;
}

// @Arena ////////////////////////////////////////////////////////////////////////////////

#define PAGES_PER_COMMIT 2

#ifndef SCRATCH_SIZE
// 8 GiB
#define SCRATCH_SIZE ((uint64_t) 8 << 30)
#endif

typedef struct Arena Arena;
struct Arena
{
  uint64_t size;
  char *memory;
  char *allocated;
  char *committed;
  uint8_t decommit_on_clear;
};

// NOTE(dg): NOT YET IMPLEMENTED
// typedef struct Arena_Temp Arena_Temp;
// struct Arena_Temp
// {
//   Arena *arena;
//   uint64_t used;
// };

#define arena_push(T, count, arena) (T *) _arena_push(arena, size_of(T) * count, align_of(T))

thread_local Arena _scratch_1;
thread_local Arena _scratch_2;

char *_align_ptr(char *ptr, uint32_t align)
{
	uint64_t result = (uint64_t) ptr;
  uint64_t remainder = result % align;
  if (remainder != 0)
  {
    result += align - remainder;
  }

	return (char *) result;
}

Arena arena_create(uint64_t size, uint8_t decommit_on_clear)
{
  Arena arena;
  arena.memory = _arena_os_reserve_vm(NULL, size);;
  arena.allocated = arena.memory;
  arena.committed = arena.memory;
  arena.size = size;
  arena.decommit_on_clear = decommit_on_clear;

  return arena;
}

void arena_destroy(Arena *arena)
{
  _arena_os_release_vm(arena->memory, 0);
  arena->memory = NULL;
  arena->allocated = NULL;
  arena->size = 0;
}

char *_arena_push(Arena *arena, uint64_t size, uint64_t align)
{
  char *ptr = _align_ptr(arena->allocated, align);
  arena->allocated = ptr + size;

  if (arena->committed < arena->allocated)
  {
    uint64_t granularity = _arena_os_get_page_size() * PAGES_PER_COMMIT;
    uint64_t size_to_commit = (uint64_t) (arena->allocated - arena->committed);
    size_to_commit += -size_to_commit & (granularity - 1);

    uint8_t commit_ok = _arena_os_commit_vm(arena->committed, size_to_commit);
    assert(commit_ok);

    arena->committed += size_to_commit;
  }
  
  return ptr;
}

void arena_pop(Arena *arena, uint64_t size)
{
  arena->allocated -= size;
  
  uint64_t start_idx = (uint64_t) (arena->allocated - arena->memory) - 1;
  uint64_t end_idx = start_idx + size;
  for (uint64_t i = start_idx; i < end_idx; i++)
  {
    arena->allocated[i] = 0;
  }
}

void arena_clear(Arena *arena)
{
  for (uint64_t i = 0; i < arena->allocated - arena->memory; i++)
  {
    arena->memory[i] = 0;
  }

  if (arena->decommit_on_clear)
  {
    uint64_t commit_size = arena->committed - arena->memory;
    uint64_t page_size = _arena_os_get_page_size();

    // If committed pages > 16, decommit pages after 16th
    uint64_t page_limit = page_size * 16;
    if (commit_size > page_limit)
    {
      char *start_addr = arena->memory + page_limit;
      _arena_os_decommit_vm(start_addr, commit_size - page_limit);
      arena->committed = start_addr;
    }
  }

  arena->allocated = arena->memory;
}

void arena_init_scratches(void)
{
  _scratch_1 = arena_create(SCRATCH_SIZE, 1);
  _scratch_2 = arena_create(SCRATCH_SIZE, 1);
}

Arena *arena_get_scratch(Arena *conflict)
{
  Arena *result = &_scratch_1;
  
  if (conflict != NULL)
  {
    if (conflict->memory == _scratch_1.memory)
    {
      result = &_scratch_2;
    }
    else if (conflict->memory == _scratch_2.memory)
    {
      result = &_scratch_1;
    }
  }

  return result;
}

// NOTE(dg): NOT YET IMPLEMENTED
// Arena_Temp arena_begin_scratch(Arena *conflict)
// {
//   Arena_Temp result;
//   result.arena = _arena_get_scratch(conflict);
//   result.used = 0;
//   return result;
// }

// void arena_end_scratch(Arena_Temp scratch)
// {
//   arena_pop(scratch.arena, scratch.used);
// }

#endif
