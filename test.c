#include "src/cfs.h"
#include <stdio.h>

void printb(void* data, uint length){
    for (uint i=0;i<length;i++)
        printf("byte (%lu): [%u] <%c>\n",i,*((byte*)data+i),*((byte*)data+i));
}

int main(){
    //rootfl r = scanfs("bin/test.img");
    rootfl r = format("bin/test.img",K(4),M(4));

    ublock b = bcreate(&r);
    char* text = "Hello World I am Groot :)\n";
    for (uint i=0;text[i]!=0;i++)
        bputb(&b, text[i]);

    bseek(&b, 0);
    ushort c=0;
    uint i=0;
    while (c < 0x100){
        bseek(&b, i);
        c=bgetb(&b);
        printf("%c",c);
        i++;
    }

    printf("CSMS:\n\
        blksize: %lu\n\
        freestart: %lu\n\
        blockcount: %lu\n\
        used: %lu\n",r.blksize,r.freestart,r.blockcount,r.usedblkcount);

    updatefsheader(&r);
    fclose(r.fd);
}
