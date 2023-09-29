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
  i32 id;
  f32 x;
  f32 y;
};

int main(void)
{
  // Create arena with 1 kilobyte of memorY
  M_Arena arena = m_arena_create(1024); 

  Entity *player = {0};
  Entity *enemy1 = {0};
  Entity *enemy2 = {0};

  // Push appropriate number of bytes for each entity struct onto the arena's stack
  player = m_arena_alloc(&arena, sizeof (Entity)); 
  enemy1 = m_arena_alloc(&arena, sizeof (Entity));
  enemy2 = m_arena_alloc(&arena, sizeof (Entity));

  player->id = 1;
  enemy1->id = 2;
  enemy2->id = 3;
  printf("%i %i %i\n", player->id, enemy1->id, enemy2->id);

  // Free entire memory block
  m_arena_destroy(&arena);
  
  return 0;
}
```
