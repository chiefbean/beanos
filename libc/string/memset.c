#include <string.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t count)
{
   uint8_t val = (uint8_t)(value & 0xFF);
   uint8_t *dest2 = (uint8_t*)(dest);

   size_t i = 0;

   while(i < count)
   {
      dest2[i] = val;
      i++;
   }

   return dest;
}
