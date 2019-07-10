#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <string.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/epoll.h>
#include <errno.h>



struct interface {
    int socket;
    int index;
    char name[255];
    struct tpacket_hdr * first_tpacket_hdr;
    int tpacket_i;
    int mapped_memory_size;
    struct tpacket_req tpacket_req;
};


int get_interface_index(int socket, const char * name)
{
    struct ifreq ifreq;
    memset(&ifreq, 0, sizeof ifreq);

    snprintf(ifreq.ifr_name, sizeof ifreq.ifr_name, "%s", name);

    if (ioctl(socket, SIOCGIFINDEX, &ifreq)) {
        return -1;
    }

    return ifreq.ifr_ifindex;
}


int bind_to_interface(int socket, int interface_index)
{
    struct sockaddr_ll sockaddr_ll;
    memset(&sockaddr_ll, 0, sizeof sockaddr_ll);
    
    sockaddr_ll.sll_family = AF_PACKET;
    sockaddr_ll.sll_protocol = htons(ETH_P_ALL);
    sockaddr_ll.sll_ifindex = interface_index;

    if (-1 == bind(socket, (struct sockaddr *)&sockaddr_ll, sizeof sockaddr_ll)) {
        return -1;
    }

    return 0;
}

struct tpacket_hdr * get_packet(struct interface * interface)
{
    return (struct tpacket_hdr *)((void *)interface->first_tpacket_hdr + interface->tpacket_i * interface->tpacket_req.tp_frame_size);
}


struct ethhdr * get_ethhdr(struct tpacket_hdr * tpacket_hdr)
{
    return ((void *) tpacket_hdr) + tpacket_hdr->tp_mac;
}


struct sockaddr_ll * get_sockaddr_ll(struct tpacket_hdr * tpacket_hdr)
{
    return ((void *) tpacket_hdr) + TPACKET_ALIGN(sizeof *tpacket_hdr);
}


void next_packet(struct interface * interface)
{
    interface->tpacket_i = (interface->tpacket_i + 1) % interface->tpacket_req.tp_frame_nr;
}


int setup_packet_mmap(struct interface * interface)
{
    struct tpacket_req tpacket_req = {
        .tp_block_size = 4096,
        .tp_frame_size = 2048,
        .tp_block_nr = 32,
        .tp_frame_nr = 64
    };

    int size = tpacket_req.tp_block_size * tpacket_req.tp_block_nr;

    void * mapped_memory = NULL;

    if (setsockopt(interface->socket, SOL_PACKET, PACKET_RX_RING, &tpacket_req, sizeof tpacket_req)) {
        return -1;
    }

    mapped_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, interface->socket, 0);

    if (MAP_FAILED == mapped_memory) {
        return -1;
    }

    interface->first_tpacket_hdr = mapped_memory;
    interface->mapped_memory_size = size;
    interface->tpacket_req = tpacket_req;

    return 0;
}




int init_interface(struct interface * interface, const char * name)
{
    interface->socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (-1 == interface->socket) {
        return -1;
    }

    interface->index = get_interface_index(interface->socket, name);

    if (interface->index < 0) {
        close(interface->socket);
        return -1;
    }

    snprintf(interface->name, sizeof interface->name, "%s", name); 

    if (bind_to_interface(interface->socket, interface->index)) {
        close(interface->socket);
        return -1;
    }

    if (setup_packet_mmap(interface)) {
        close(interface->socket);
        return -1;
    }

    return 0;
}


struct interface * create_interface(const char * name) 
{
    struct interface * interface = malloc(sizeof *interface);

    if (!interface) {
        return NULL;
    }

    if (init_interface(interface, name)) {
        free(interface);
        return NULL;
    }

    return interface;
}


void exit_interface(struct interface * interface)
{
    munmap(interface->first_tpacket_hdr, interface->mapped_memory_size);
    close(interface->socket);
}


void destroy_interface(struct interface * interface)
{
    exit_interface(interface);
    free(interface);
}

void handler(int signal_number) 
{
    printf("Received %d\n", signal_number);
}

int install_signal(void) 
{
    struct sigaction ssigaction;
    memset(&ssigaction, 0, sizeof ssigaction);
    ssigaction.sa_handler = handler;
    return sigaction(SIGINT, &ssigaction, NULL);
}


void print_packet(struct ethhdr * ethhdr)
{
    printf("{\"src\": \"%02x:%02x:%02x:%02x:%02x:%02x\", \"dst\":\"%02x:%02x:%02x:%02x:%02x:%02x\"}\n",
        ethhdr->h_source[0],
        ethhdr->h_source[1],
        ethhdr->h_source[2],
        ethhdr->h_source[3],
        ethhdr->h_source[4],
        ethhdr->h_source[5],
        ethhdr->h_dest[0],
        ethhdr->h_dest[1],
        ethhdr->h_dest[2],
        ethhdr->h_dest[3],
        ethhdr->h_dest[4],
        ethhdr->h_dest[5]
    );
}


void hex_dump (void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}


void print_sockaddr_ll(struct sockaddr_ll * sockaddr_ll)
{
    printf("sll_family: %d\n", sockaddr_ll->sll_family);
    printf("sll_protocol: %d\n", sockaddr_ll->sll_protocol);
    printf("sll_ifindex: %d\n", sockaddr_ll->sll_ifindex);
    printf("sll_hatype: %d\n", sockaddr_ll->sll_hatype);
    printf("sll_halen: %d\n", sockaddr_ll->sll_halen);
    printf("sll_family: %d\n", sockaddr_ll->sll_family);
    printf("sll_addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
        sockaddr_ll->sll_addr[0],
        sockaddr_ll->sll_addr[1],
        sockaddr_ll->sll_addr[2],
        sockaddr_ll->sll_addr[3],
        sockaddr_ll->sll_addr[4],
        sockaddr_ll->sll_addr[5]
    );
}


int main(int argc, char ** argv)
{
    int epoll_fd = -1;
    struct interface * interface = create_interface("eth0");
    struct epoll_event event = {
        .events = EPOLLIN | EPOLLET,
        .data = { .ptr = NULL }
    };

    if (install_signal()) {
        perror("failed to install signal");
        return 1;
    }

    if (!interface) {
        perror("create_interface failed.");
        return 1;
    }

    epoll_fd = epoll_create(1024);

    if (-1 == epoll_fd) {
        perror("epoll_create failed.");
        destroy_interface(interface);
        return 1;
    }

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, interface->socket, &event)) {
        perror("epoll_ctl failed");
        close(epoll_fd);
        destroy_interface(interface);
        return 1;
    }


    while (1) {
        struct epoll_event events[16];
        int num_events = -1;
        int i = 0;

        num_events = epoll_wait(epoll_fd, events, sizeof events / sizeof *events, -1);

        if (num_events == -1)  {
            if (errno == EINTR)  {
                perror("epoll_wait returned -1");
                break;
            }
            perror("error");
            continue;
        }

        for (int i = 0; i < num_events; ++i)  {
            struct epoll_event * event = &events[i];

            if (event->events & EPOLLIN) {
                struct tpacket_hdr * tpacket_hdr = get_packet(interface);
                struct sockaddr_ll * sockaddr_ll = NULL;
                struct ethhdr * ethhdr = NULL;

                if (tpacket_hdr->tp_status & TP_STATUS_COPY) {
                    printf("Packet truncated\n");
                    next_packet(interface);
                    continue;
                }

                if (tpacket_hdr->tp_status & TP_STATUS_LOSING) {
                    printf("Packet drops were detected");
                    next_packet(interface);
                    continue;
                }
                
                ethhdr = get_ethhdr(tpacket_hdr);
                sockaddr_ll = get_sockaddr_ll(tpacket_hdr);

                print_packet(ethhdr);
                print_sockaddr_ll(sockaddr_ll);

                tpacket_hdr->tp_status = TP_STATUS_KERNEL;
                next_packet(interface);
            }
        }
    }


    close(epoll_fd);
    destroy_interface(interface);

    return 0;
}
