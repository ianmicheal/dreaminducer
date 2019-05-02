#include "cdfs.h"

void exit() {while(1);}
void atexit() { }

void (*bin)() = (void (*)())0x8c010000;
unsigned char *ptr = (unsigned char*)(0x8c010000);

#define L1_CACHE_BYTES 32
#define CACHE_IC_ADDRESS_ARRAY  0xf0000000
#define CACHE_IC_ENTRY_MASK     0x1fe0

struct __large_struct { unsigned long buf[100]; };   
#define __m(x) (*(struct __large_struct *)(x))

unsigned char calledbin[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";

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

#define min(x,y) ((x<y) ? x : y)

unsigned long load_file(unsigned char *filename) {
	int fd;
	unsigned long len, tr, r;
	unsigned char *lp=ptr;

	fd=open(filename, O_RDONLY);
	if(fd<0) return 0;

	len=tr=total(fd);
	if(len==0) {
		close(fd);
		return 0;
	}

	while(tr) {
		r = read(fd, lp, min(2048,tr));
		tr -= r;
		lp += r;
	}

	close(fd);

	return len;
}

int main() {
	unsigned long len;

	cdfs_init();

	len=load_file(calledbin);

	if(len>0) {
		flush_icache_range(0x8c010000, len);

		bin();
	}

	len=load_file("INDUCER.BIN");

	if(len>0) {
		flush_icache_range(0x8c010000, len);

		bin();
	}

	return 0;
}
