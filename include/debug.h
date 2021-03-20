/* debug.h */

#define DEBUG_ASCII 0x01
#define DEBUG_HEX   0x02

void hexdump(void *buffer, uint32 length, bool8 text);
void debugbreak(void);
void debugret(void);
