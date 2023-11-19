# ArenaAllocator
Very basic Arena Allocator implementation in C, inspired by Ryan Fleury's article. Please use arenas instead of malloc and free, thanks.
For more info on arenas and their benefits, see www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

# Usage example
```c
#include <stdio.h>
#include "arena.h"

typedef struct Entity Entity;
struct Entity
{
  int id;
  float x;
  float y;
};

int main(void)
{
  // Create arena with 1 kilobyte of memory
  Arena arena = create_arena(KiB(1));

  Entity *player = {0};
  Entity *enemy1 = {0};
  Entity *enemy2 = {0};

  // Allocate memory
  player = arena_alloc(&arena, sizeof (Entity)); 
  enemy1 = arena_alloc(&arena, sizeof (Entity));
  enemy2 = arena_alloc(&arena, sizeof (Entity));

  // Use the data...

  // Free entire memory block
  clear_arena(&arena);
  
  return 0;
}
```
