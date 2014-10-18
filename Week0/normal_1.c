#include <sys/inotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main()
{
    int fd, wd, length, i = 0;
    const int EVENT_SIZE = sizeof(struct inotify_event);
    const int BUF_LEN = 1024 * (EVENT_SIZE + FILENAME_MAX + 1);
    char buffer[BUF_LEN];
    const char* dir = "./";

    printf("%d\n", BUF_LEN);
    fd = inotify_init();
    if (fd < 0) {
        puts(strerror(errno));
        exit(EXIT_FAILURE);
    }

    wd = inotify_add_watch(fd, dir, IN_MODIFY | IN_CREATE | IN_DELETE);
    if (wd < 0) {
        puts(strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }

    length = read(fd, buffer, BUF_LEN);
    if (length == -1) puts("Fail with reading");

    while (1) {
        if (length > 0) {
            while (i < length) {
                printf("%d\n", length);
                struct inotify_event* event = (struct inotify_event*)&buffer[i];
                if (event->len) {
                    if (event->mask & IN_CREATE) {
                        if (event->mask & IN_ISDIR) {
                            printf("The directory %s was created.\n", event->name);
                        } else {
                            printf("The file %s was created.\n", event->name);
                        }
                    }
                }
                i += EVENT_SIZE + event->len;
            }
        }
        length = read(fd, buffer, BUF_LEN);
        i = 0;
    }

    inotify_rm_watch(fd, wd);
    close(fd);
    return 0;
}
