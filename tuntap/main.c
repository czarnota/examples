#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <linux/if_tun.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

struct meta {
    uint16_t flags;
    uint16_t proto;
};

struct device {
    int descriptor;
    char name[255];
};

int create_tun_device(char * out_device_name, size_t size, const char * pattern) {
    int tun_device = -1;
    struct ifreq ifreq;
    int error = -1;

    memset(&ifreq, 0, sizeof ifreq);

    if ((tun_device = open("/dev/net/tun", O_RDWR)) < 0) {
        return tun_device;
    }

    ifreq.ifr_flags = IFF_TAP | IFF_NO_PI;

    if (pattern) {
        strncpy(ifreq.ifr_name, pattern, sizeof ifreq.ifr_name);
    }

    if ((error = ioctl(tun_device, TUNSETIFF, &ifreq)) < 0) {
        close(tun_device);
        return error;
    }

    snprintf(out_device_name, size, "%s", ifreq.ifr_name);

    return tun_device;
}


int destroy_tun_device(int fd) {
    return close(fd);
}


void handler(int unused) {
    printf("Interrupted \n");
}


int init_device(struct device * dev, const char * name) {
    memset(dev, 0, sizeof *dev);

    if ((dev->descriptor = create_tun_device(dev->name, sizeof dev->name, name)) < 0) {
        return dev->descriptor;
    }

    return 0;
}

void exit_device(struct device * dev) {
    if (dev->descriptor > 0)  {
        close(dev->descriptor);
    }
    memset(dev, 0, sizeof *dev);
}


int install_signal(void) {
    struct sigaction saction;
    memset(&saction, 0, sizeof saction);
    saction.sa_handler = handler;
    if (sigaction(SIGINT, &saction, NULL)) {
        fprintf(stderr, "Unable to install signal handler\n");
        return -1;
    }
    return 0;
}

struct device * create_devices(size_t num_devices) {
    int i = 0;
    int j = 0;
    struct device * devices = NULL;

    if (!(devices = malloc(num_devices * sizeof *devices))) {
        return NULL;
    }

    for (i = 0; i < num_devices; ++i) {
        if (init_device(&devices[i], "xdpa%d")) {
            for (j = 0; j < i; ++j) {
                exit_device(&devices[j]);
            }
            free(devices);
            return NULL;
        }
    }

    return devices;
}

void destroy_devices(struct device * devices, size_t num_devices) {
    int i = 0;
    for (i = 0; i < num_devices; ++i) {
        exit_device(&devices[i]);
    }
    free(devices);
}


int add_devices_to_epoll(int epoll, struct device * devices, size_t num_devices) {
    int i = 0;
    int j = 0;
    for (i = 0; i < num_devices; ++i) {
        struct epoll_event event;

        memset(&event, 0, sizeof event);

        event.events = EPOLLIN;
        event.data.ptr = devices + i;

        if (-1 == epoll_ctl(epoll, EPOLL_CTL_ADD, devices[i].descriptor, &event)) {
            int err = errno;
            for (j = 0; j < i; ++j) {
                epoll_ctl(epoll, EPOLL_CTL_DEL, devices[j].descriptor, &event);
            }
            errno = err;
            return err;
        }
    }

    return 0;
}

void remove_devices_from_epoll(int epoll, struct device * devices, size_t num_devices) {
    int i = 0;
    for (i = 0; i < num_devices; ++i) {
        struct epoll_event event;
        epoll_ctl(epoll, EPOLL_CTL_DEL, devices[i].descriptor, &event);
    }
}


int main(int argc, char ** argv) {
    long num_devices = 0;
    struct device * devices = NULL;
    int epoll_fd = -1;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_devices>\n", argv[0]);
        return -1;
    }

    num_devices = strtol(argv[1], NULL, 10);

    if (errno) {
        perror("strtol error");
        return errno;
    }

    if (num_devices <= 0) {
        fprintf(stderr, "Incorrect number of deviced %ld", num_devices);
        return -1;
    }

    if (install_signal()) {
        fprintf(stderr, "Unable to install signal handler\n");
        return -1;
    }

    if (!(devices = create_devices(num_devices))) {
        perror("Unable to create device");
        return errno;
    }

    if ((epoll_fd = epoll_create(10)) < 0) {
        destroy_devices(devices, num_devices);
        perror("Unable to create epoll");
        return errno;
    }

    if (add_devices_to_epoll(epoll_fd, devices, num_devices)) {
        close(epoll_fd); 
        destroy_devices(devices, num_devices);
        perror("Unable to add devices to epoll");
        return errno;
    }


    while (1) {
        struct epoll_event events[10];
        int num_events = 0;
        int i = 0;

        num_events = epoll_wait(epoll_fd, events, sizeof events / sizeof *events, -1);

        if (num_events == -1) {
            if (errno == EINTR) {
                perror("epoll_wait interrupted");
                break;
            } else {
                perror("epoll_wait failed");
                break;
            }
        }

        for (i = 0; i < num_events; ++i) {
            struct epoll_event * event = events + i;
            struct device * dev = event->data.ptr;

            if (event->events & EPOLLIN) {
                unsigned char buffer[4096] = { 0 }; 
                int num_bytes = -1;
                int j = 0;

                num_bytes = read(dev->descriptor, buffer, sizeof buffer);


                if (num_bytes <= 0) {
                    struct epoll_event event;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, dev->descriptor, &event);
                    exit_device(dev);
                }

                if (num_bytes > 0) {
                    for (j = 0; j < num_devices; ++j) {
                        if (devices[j].descriptor > 0 && devices[j].descriptor != dev->descriptor) {
                            write(devices[j].descriptor, buffer, num_bytes);
                        }
                    }
                }
            }

            if (event->events & (EPOLLRDHUP | EPOLLERR | EPOLLHUP)) {
                struct epoll_event event;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, dev->descriptor, &event);
                exit_device(dev);
            }

        }

    }


    destroy_devices(devices, num_devices);

    return 0;
}
