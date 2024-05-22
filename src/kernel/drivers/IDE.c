#include "IDE.h"


int bl_common(int drive,int numblock,int count){
   x86_out_8(0x1F1,0x00);
   x86_out_8(0x1F2,count);
   x86_out_8(0x1F3,(unsigned char) numblock);
   x86_out_8(0x1F4,(unsigned char) (numblock>> 8));
   x86_out_8(0x1F5,(unsigned char) (numblock >> 16));
   
   x86_out_8(0x1F6,0xE0 | (drive <<4) | ((numblock >> 24) & 0x0F));
   
   return 0;
}

int bl_read(int drive, int numblock, int count , char* buf){
    u16 tmpword;
    int idx;
    
    bl_common(drive,numblock,count);
    x86_out_8(0x1F7,0x20);
    
    while (!(x86_in_8(0x1F7) & 0x08));
    
    for (idx = 0; idx<256*count;idx++){
        tmpword=x86_in_16(0x1F0);
        buf[idx*2] = (unsigned char) tmpword;
        buf[idx*2+1]=(unsigned char) (tmpword >> 8);
    }
    return count;
}

int bl_write(int drive,int numblock,int count, char* buf){
    u16 tmpword;
    int idx;
    
    bl_common(drive,numblock,count);
    x86_out_8(0x1F7,0x30);
    
    while (!(x86_in_8(0x1F7) & 0x08));
    
    for (idx = 0; idx<256*count;idx++){
        tmpword=(buf[idx*2+1]<<8) | buf[idx*2];
        x86_out_16(0x1F0,tmpword);
    }
    return count;
    
}


