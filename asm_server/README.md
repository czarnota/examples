# asm-server
A stupid simple server example using sockets written in GNU Assembler. No dependency to C standard library - just raw system calls.
This will only work on x86_64 linux. **It will not work on x86**

# Building
```bash
make
```

#Running
To start listening on port 5005
```bash
./main
```

#Testing
```bash
telnet localhost 5005
```
Will output

```
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
I've been written in assembly and I am too simple to handle you. Good bye :)
Connection closed by foreign host.

```
