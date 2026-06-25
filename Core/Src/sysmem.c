#include <errno.h>
#include <stdint.h>

extern char end;
static char *heap_end;

void *_sbrk(int incr)
{
  extern char _estack;
  char *prev_heap_end;

  if (heap_end == 0)
  {
    heap_end = &end;
  }

  prev_heap_end = heap_end;
  if ((uintptr_t)(heap_end + incr) > (uintptr_t)&_estack)
  {
    errno = ENOMEM;
    return (void *)-1;
  }

  heap_end += incr;
  return prev_heap_end;
}
