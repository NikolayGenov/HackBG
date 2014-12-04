#ifndef _GETJPEG_H_
#define _GETJPEG_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <jpeglib.h>
#include <libv4l2.h>

#include "yuv.h"

struct buffer {
    void* start;
    size_t length;
};

void errno_exit (const char* s);
int xioctl (int fd, int request, void* argp);

void deviceOpen (void);
void deviceInit (void);

void mmapInit (void);

void deviceUninit (void);
void deviceClose (void);

void captureStart (void);
void captureStop (void);

void jpegWrite (unsigned char* img);
void imageProcess (const void* p);
int frameRead (void);

void mainLoop (void);
int getJpeg (char* filename, int width, int height);

#endif

