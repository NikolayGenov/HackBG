#include "getjpeg.h"

// number of buffers
#define VIDIOC_REQBUFS_COUNT 2

static int fd = -1;
struct buffer* buffers = NULL;
static unsigned int n_buffers = 0;

static unsigned int WIDTH = 0;
static unsigned int HEIGHT = 0;
static unsigned int fps = 30;
static unsigned char jpegQuality = 100;
static char* jpegFilename = NULL;
static char* deviceName = "/dev/video0";

#define CLEAR(x) memset(&(x), 0, sizeof(x))

void errno_exit(const char* s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void* argp) {
    int r;
    do {
        r = v4l2_ioctl(fd, request, argp);
    } while (-1 == r && EINTR == errno);

    return r;
}

void deviceOpen(void) {
    struct stat st;

    if (-1 == stat(deviceName, &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", deviceName, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //Check if its device
    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", deviceName);
        exit(EXIT_FAILURE);
    }

    fd = v4l2_open(deviceName, O_RDWR | O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", deviceName, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void mmapInit(void) {
    struct v4l2_requestbuffers req;

    CLEAR(req);
    req.count = VIDIOC_REQBUFS_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        errno_exit("VIDIOC_REQBUFS OR Memory mapping error");
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n", deviceName);
        exit(EXIT_FAILURE);
    }

    buffers = calloc(req.count, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = v4l2_mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

void deviceInit(void) {
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_format fmt;
    struct v4l2_streamparm frameint;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", deviceName);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is no video capture device\n", deviceName);
        exit(EXIT_FAILURE);
    }

    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    xioctl(fd, VIDIOC_CROPCAP, &cropcap);

    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        errno_exit("VIDIOC_S_FMT");

    if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420) {
        fprintf(stderr, "Libv4l didn't accept YUV420 format. Can't proceed.\n");
        exit(EXIT_FAILURE);
    }

    if (WIDTH != fmt.fmt.pix.width)
        WIDTH = fmt.fmt.pix.width;

    if (HEIGHT != fmt.fmt.pix.height)
        HEIGHT = fmt.fmt.pix.height;

    CLEAR(frameint);

    frameint.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frameint.parm.capture.timeperframe.numerator = 1;
    frameint.parm.capture.timeperframe.denominator = fps;
    if (-1 == xioctl(fd, VIDIOC_S_PARM, &frameint))
        fprintf(stderr, "Unable to set frame interval.\n");

    mmapInit();
}

void deviceUninit(void) {
    unsigned int i;

    for (i = 0; i < n_buffers; ++i)
        if (-1 == v4l2_munmap(buffers[i].start, buffers[i].length))
            errno_exit("munmap");

    free(buffers);
}

void deviceClose(void) {
    if (-1 == v4l2_close(fd))
        errno_exit("close");
    fd = -1;
}

void captureStart(void) {
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
}

void jpegWrite(unsigned char* img) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];
    FILE* outfile = fopen(jpegFilename, "wb");

    if (!outfile) {
        errno_exit("Can't open file");
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = WIDTH;
    cinfo.image_height = HEIGHT;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_YCbCr;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, jpegQuality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &img[cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

void imageProcess(const void* p) {
    unsigned char* src = (unsigned char*)p;
    unsigned char* dst = malloc(WIDTH * HEIGHT * 3 * sizeof(char));

    YUV420toYUV444(WIDTH, HEIGHT, src, dst);

    jpegWrite(dst);

    free(dst);
}

int frameRead(void) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
        case EAGAIN:
            return 0;

        case EIO:

        default:
            errno_exit("VIDIOC_DQBUF");
        }
    }

    imageProcess(buffers[buf.index].start);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");

    return 1;
}

void mainLoop(void) {
    unsigned int count;
    unsigned int numberOfTimeouts;

    numberOfTimeouts = 0;
    count = 2;

    while (count-- > 0) {
        while (1) {
            fd_set fds;
            struct timeval tv;
            int r;

            FD_ZERO(&fds);
            FD_SET(fd, &fds);

            tv.tv_sec = 0;
            tv.tv_usec = 100000;

            r = select(fd + 1, &fds, NULL, NULL, &tv);

            if (-1 == r) {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }

            if (0 == r) {
                if (numberOfTimeouts <= 0) {
                    count++;
                } else {
                    fprintf(stderr, "select timeout\n");
                    exit(EXIT_FAILURE);
                }
            }

            if (frameRead())
                break;
        }
    }
}

void captureStop(void) {
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
        errno_exit("VIDIOC_STREAMOFF");
}


int getJpeg(char* filename, int width, int height) {

    jpegFilename = filename;
    WIDTH = width;
    HEIGHT = height;

    deviceOpen();
    deviceInit();

    captureStart();

    mainLoop();

    captureStop();

    deviceUninit();
    deviceClose();

    return 0;
}
