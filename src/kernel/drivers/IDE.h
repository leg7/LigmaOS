#pragma once
#include <library/types.h>
#include <architecture/x86/io.h>

int bl_common(int drive,int numblock,int count);

int bl_read(int drive, int numblock, int count , char* buf);

int bl_write(int drive,int numblock,int count, char* buf);
