#include <stdio.h>

typedef unsigned long uint;
typedef unsigned char byte;
typedef unsigned short ushort ;

#define K(s) s*1024
#define M(s) 2*K(1024)
#define G(s) 2*M(1024)
#define T(s) 2*G(1024)

typedef struct{
    FILE* fd;
    uint blksize;
    uint freestart;
    uint rootfld;
    uint blockcount;
    uint usedblkcount;
} rootfl ;

typedef struct{
    const uint start;
    rootfl *fs;
    const uint jd;
    uint curr;
    uint pos;
} ublock;

typedef rootfl csms;

#define ARCH sizeof(long)

void utob(uint num, byte* callback);
uint btou(byte* num);
uint power(uint i,uint n);
void writenum(uint num, rootfl *self);
uint readnum(rootfl *self);

rootfl format(char* filename, uint blocksize, uint filesize);
rootfl scanfs(char* filename);

uint balloc(rootfl *fs);
void bfree(rootfl *fs, uint ptr);

void updatefsheader(rootfl *fs);

ublock bcreate(rootfl *fs);
byte bputb(ublock *blk, byte val);
ushort bgetb(ublock *blk);
byte bseek(ublock *blk, uint pos);
byte bdel(ublock *blk);
