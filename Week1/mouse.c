#include <stdio.h>
#include <unistd.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>
#include <math.h>
#define MOUSEFILE "/dev/input/mice"

int main()
{
    int fd;
    struct input_event ie;

    unsigned char button, bLeft, bMiddle, bRight;
    char x, y;
    int absolute_x, absolute_y;
    int freq_x, freq_y, sound, value;
    int freq[] = { /* C   D    E    F    G    A    B    C */
                   523, 587, 659, 698, 784, 880, 988, 1046 };

    if ((fd = open(MOUSEFILE, O_RDONLY)) == -1) {
        printf("Device open ERROR\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Device open OK\n");
    }

    int dev = open("/dev/console", O_RDWR, 0);

    printf("right-click to set absolute x,y coordinates origin (0,0)\n");
    while (read(fd, &ie, sizeof(struct input_event))) {
        unsigned char* ptr = (unsigned char*)&ie;
        int i;

        button = ptr[0];
        bLeft = button & 0x1;
        bMiddle = (button & 0x4) > 0;
        bRight = (button & 0x2) > 0;
        x = (char)ptr[1];
        y = (char)ptr[2];

        if (bLeft) puts("Left button pressed");
        if (bRight) puts("Right button pressed");
        if (bMiddle) puts("Middle button pressed");

        absolute_x += x;
        absolute_y -= y;

        if (bMiddle) {
            absolute_x = 0;
            absolute_y = 0;
            printf("Absolute x,y coords origin recorded\n");
        }
        printf("X,Y: (%i, %i) dX,dY: (%d, %d)\n", absolute_x, absolute_y, x, y);

        freq_x = 700 * abs(x) % 20000;
        freq_y = 700 * abs(y) % 20000;
        sound = freq_x > freq_y ? freq_x : freq_y;
        value = ((abs(x) > abs(y) ? abs(x) : abs(y)) / 2) % 8;
        if (x || y) {
            ioctl(dev, KIOCSOUND, 1193180 / freq[value]);
            usleep(40000);
            ioctl(dev, KIOCSOUND, 0);
        } else {
            ioctl(dev, KIOCSOUND, 0);
        }
    }
    ioctl(dev, KIOCSOUND, 0);
    return 0;
}
