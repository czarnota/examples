.data

.set stdout, 1
.set AF_INET, 2
.set SOCK_STREAM, 1

sockaddr_in:
    .word AF_INET
    .word 0x8D13 //port 5005 is 0x138D and x86_64 is little-endian so it is 0x8D13
    .long 0x0
    .quad 0x0
.set sockaddr_in_size, . - sockaddr_in //sizeof sockaddr_in


fd:
    .long -1

client_fd:
    .long -1

message:
    .asciz "I've been written in assembly and I am too simple to handle you. Good bye :)\n"

.set message_size, . - message
    

.text
.global _start


//int bind(int fd, struct sockaddr * sockaddr, int addrlen)
bind:
    movq $49, %rax
    syscall
    retq


//int write(int fd, void * ptr, int size)
write:
    movq $1, %rax       //1 is sys_write syscall number
    syscall
    retq


//int exit_successfully(void)
exit_successfully:
    movq $60, %rax      //60 is sys_exit syscall number
    movq $0, %rdi       //first argument is 0
    syscall             //calling exit(0)
    retq                //return


//int socket(int family, int type, int protocol)
socket:
    movq $41, %rax      //41 is sys_socket syscall number
    movq $AF_INET, %rdi 
    movq $SOCK_STREAM, %rsi
    movq $0, %rdx
    syscall
    retq                //%rax will contain return value


//int close(int fd)
close:
    movq $3, %rax
    syscall
    retq


//int listen(int fd, int backlog)
listen:
    movq $50, %rax
    syscall
    retq


//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
accept:
    movq $43, %rax
    syscall
    retq


_start:
    callq socket
    cmpq $0, %rax
    jl error_socket

    movl %eax, fd(%rip)
    
    movl fd(%rip), %edi
    leaq sockaddr_in(%rip), %rsi
    movq $sockaddr_in_size, %rdx
    callq bind
    cmp $0, %rax
    jnz error_bind

    movl fd(%rip), %edi
    movq $128, %rsi
    callq listen
    cmp $0, %rax
    jnz error_listen

server_loop:

    movl fd(%rip), %edi
    movq $0, %rsi
    movq $0, %rdx
    callq accept
    movl %eax, client_fd(%rip)

    cmpq $0, client_fd(%rip)
    jl error_accept

    movl client_fd(%rip), %edi
    leaq message(%rip), %rsi
    movq $message_size, %rdx
    callq write

    movl client_fd(%rip), %edi
    callq close
     
    jmp server_loop

error_accept:
error_listen:
error_bind:
    movl fd(%rip), %edi 
    callq close

error_socket:
    callq exit_successfully
