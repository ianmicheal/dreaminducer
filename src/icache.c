#define L1_CACHE_BYTES 32
#define CACHE_IC_ADDRESS_ARRAY  0xf0000000
#define CACHE_IC_ENTRY_MASK     0x1fe0

struct __large_struct { unsigned long buf[100]; };
#define __m(x) (*(struct __large_struct *)(x))

void flush_icache_range(unsigned long start, unsigned long end) {
        unsigned long addr, data, v;
        end += start;
        start &= ~(L1_CACHE_BYTES-1);
        for (v=start; v<end; v+=L1_CACHE_BYTES) {
                /* Write back O Cache */
                        asm volatile("ocbwb %0"
                                        : /* no output */
                                        : "m" (__m(v))); 
                        /* Invalidate I Cache */ 
                        //addr = CACHE_IC_ADDRESS_ARRAY |
                        //      (v & CACHE_IC_ENTRY_MASK) | 0x8 /* A-bit */;
                        addr = CACHE_IC_ADDRESS_ARRAY |  
                                (v & CACHE_IC_ENTRY_MASK);
                        data = (v & 0xfffffc00);        /* Valid = 0 */
                        *((volatile unsigned long*)addr) = data;
        }
}
