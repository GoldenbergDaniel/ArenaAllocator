# ArenaAllocator
Basic arena allocator implementation in C, inspired by Ryan Fleury's article. Please use arenas instead of malloc and free, it makes manual memory management virtually trivial.

For more info on arenas, see www.rfleury.com/p/untangling-lifetimes-the-arena-allocator

# Usage example
```c
#include "arena_lib.h"

typedef struct Entity Entity;
struct Entity
{
  int id;
  float x;
  float y;
};

int main(void)
{
  // - Create arena with 16 MiB of reserved virtual memory ---
  Arena arena = arena_create(MiB(16));

  // - Scope 1 ---
  {
    // - Allocate memory for the entities ---
    Entity *player = arena_push(Entity, 1, &arena);
    Entity *enemies = arena_push(Entity, 64, &arena);

    // - Use the data ---
    // ...

    // - Reset arena's memory ---
    arena_clear(&arena);
  }

  // - Scope 2 ---
  {
    Entity *player = arena_push(Entity, 1, &arena);
    Entity *enemy = arena_push(Entity, 1, &arena);

    // - Use the data ---
    // ...

    arena_clear(&arena);
  }
  
  return 0;
}
```
