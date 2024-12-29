#include "../cfs.h"
#include <stdio.h>

void utob(uint num, byte* callback){
    for (uint i=0;i<ARCH;i++,num-=num%256,num/=256)
        callback[i]=num%256;
}

uint power(uint a,uint n){
    uint out=1;
    for (uint i=0;i<n;i++){
        out*=a;
    }
    return out;
}

uint btou(byte* num){
    uint out = 0;
    for (uint i=0;i<ARCH;i++)
        out+=num[i]*power(256, i);
    return out;
}
void writenum(uint num, rootfl *self){
    byte arr[ARCH] = {0};
    utob(num,arr);
    for (uint i=0;i<ARCH;i++){
        fputc(arr[i],self->fd);
    }
}
uint readnum(rootfl *self){
    byte arr[ARCH];
    for (uint i = 0; i<ARCH;i++)
        arr[i]=fgetc(self->fd);
    return btou(arr);
}

rootfl format(char* filename, uint blocksize, uint filesize){
    rootfl out={
        .fd=fopen(filename, "r+"),
        .blksize=blocksize,
        .freestart=0,
        .blockcount=((filesize-40)-((filesize-40)%blocksize))/blocksize
    };
    uint jd=blocksize-ARCH;
    uint pos=40;
    for (uint i=0;i<out.blockcount;i++){
        uint ptr=out.freestart;
        out.freestart=pos;
        pos+=jd;
        fseek(out.fd, pos, 0);
        writenum(ptr, &out);
        pos+=ARCH;
    }

    fseek(out.fd,0,0);
    writenum(out.blksize, &out);
    writenum(out.freestart, &out);
    writenum(out.blockcount, &out);
    writenum(out.usedblkcount, &out);
    return out;
}

rootfl scanfs(char* filename){
    rootfl out={
        .fd = fopen(filename, "r+"),
    };
    out.blksize=readnum(&out);
    out.freestart=readnum(&out);
    out.blockcount=readnum(&out);
    out.usedblkcount=readnum(&out);
    out.rootfld=readnum(&out);
    return out;
}

uint balloc(rootfl *fs){
    uint ptr = fs->freestart;
    uint jd=fs->blksize-ARCH;
    fseek(fs->fd, ptr+jd, 0);
    if (ptr != 0){
        fs->freestart=readnum(fs);
        fs->usedblkcount+=1;
    }
    fseek(fs->fd, ptr, 0);
    for (uint i=0;i<fs->blksize;i++)
        fputc(0, fs->fd);
    return ptr;
}
void bfree(rootfl *fs, uint ptr){
    if (ptr != 0){
        uint optr = fs->freestart;
        uint jd = fs->blksize-ARCH;
        fseek(fs->fd, ptr+jd, 0);
        writenum(optr, fs);
        fs->freestart=ptr;
        fs->usedblkcount-=1;
    }
}

void updatefsheader(rootfl *fs){
    fseek(fs->fd, 0, 0);
    writenum(fs->blksize, fs);
    writenum(fs->freestart, fs);
    writenum(fs->blockcount, fs);
    writenum(fs->usedblkcount, fs);
    writenum(fs->rootfld, fs);
}

ublock bcreate(rootfl *fs){
    ublock out ={
        .fs=fs,
        .jd=fs->blksize-ARCH,
        .start=balloc(fs),
        .pos=0,
    };
    out.curr=out.start;
    return out;
}
byte bseek(ublock *blk, uint pos){
    blk->curr=blk->start;
    uint off=pos%blk->jd;
    uint jmps = (pos-off)/blk->jd;
    if (off == 0 && pos>0)
        jmps-=1;
    for (uint i=0;i<jmps&&blk->curr != 0;i++){
        fseek(blk->fs->fd, blk->curr+blk->jd, 0);
        blk->curr=readnum((blk->fs));
    }
    blk->pos=pos;
    return (blk->curr!=0);
}
byte bputb(ublock *blk, byte val){
    uint off=blk->pos%blk->jd;
    uint next=0;
    if (blk->pos > 0 && off == 0){
        fseek(blk->fs->fd, blk->curr+blk->jd, 0);
        next=readnum(blk->fs);
        if (next == 0){
            uint ptr = balloc(blk->fs);
            fseek(blk->fs->fd, blk->curr+blk->jd, 0);
            writenum(ptr, blk->fs);
            blk->curr=ptr;
        }else{
            blk->curr=next;
        }
    }
    if (blk->curr != 0){
        fseek(blk->fs->fd, blk->curr+off, 0);
        fputc(val,blk->fs->fd);
        blk->pos++;
    }
    return 1;
}
ushort bgetb(ublock *blk){
    uint off = blk->pos%blk->jd;
    if (blk->pos > 0 && off == 0){
        fseek(blk->fs->fd, blk->curr+blk->jd, 0);
        blk->curr=readnum(blk->fs);
    }
    ushort num=0;
    if (blk->curr != 0){
        fseek(blk->fs->fd, blk->curr+off, 0);
        num=fgetc(blk->fs->fd);
        blk->pos++;
    }
    if (blk->curr == 0)
        return 0x100;
    return num;

}
byte bdel(ublock *blk);
