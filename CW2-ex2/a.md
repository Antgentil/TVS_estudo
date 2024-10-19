### Exercise 2

**a) List and explain the sequence of calls performed by the program in x86-64/prog.s**

----
```
data:   .byte  47, 101, 116,  99      ; 47 -> '/', 101 -> 'e', 116 -> 't', 99 -> 'c'
        .byte  47, 111, 115,  45      ; 47 -> '/', 111 -> 'o', 115 -> 's', 45 -> '-'
        .byte 114, 101, 108, 101      ; 114 -> 'r', 101 -> 'e', 108 -> 'l', 101 -> 'e'
        .byte  97, 115, 101,   0      ; 97 -> 'a', 115 -> 's', 101 -> 'e', 0 -> null terminator
        .byte  73,  83,  69,  76      ; 73 -> 'I', 83 -> 'S', 69 -> 'E', 76 -> 'L'
```
**First 4 lines:**
``/etc/os-release\0``

``\0`` indicates the null terminator at the end. 

**Last line:**
``ISEL``


### 1º System Call (openat)
```
movq $-100, %rdi         
leaq data(%rip), %rsi     
xorq %rdx, %rdx          
movq $257, %rax          
syscall

movq %rax, %r15
```
``movq $-100, %rdi:``

- 1º argument (-100, a special **AT_FDCWD** constant) no **RDI**.

Info: https://sites.uclouvain.be/SystInfo/usr/include/linux/fcntl.h.html

*If **openat()** is passed the special value ``AT_FDCWD`` in the fd parameter, the current
     working directory is used and the behavior is identical to a call to open().*

Info: https://manpages.ubuntu.com/manpages/focal/en/man2/open.2freebsd.html

``leaq data(%rip), %rsi:``

- 2º argument (pointer to the string "/etc/os-release") no **RSI**. 

``leaq`` - **load effective address**: Get the memory address where the data starts.
``data(%rip)`` - is pointing to the beginning of the data section in memory.
The result (the address of the first byte of data) is stored in %rsi.

After this line runs, **%rsi holds the memory address of the start** of the data section, which means it points to the string **/etc/os-release**.

Even though **ISEL** is stored right after /etc/os-release, the program doesn't read it because it stops at the null terminator.

> This performs the ``openat`` system call (syscall number **257**), opening the file ``/etc/os-release`` using the ``AT_FDCWD`` constant (-100), which tells the system to **use the current working directory**.
The file descriptor returned is stored in %rax and later moved to %r15 for future use.

``xorq %rdx, %rdx: ``

- Flags = 0 (by XORing **RDX** with itself)

``movq $257, %rax: ``
- Syscall number for openat (257) - save fd (file descriptor) in **RAX**

``syscall``
- Perform system call

``movq %rax, %r15``

- The file descriptor returned, stored in %rax, is later moved to %r15 for future use.
---

### 2º System Call (lseek)
```
movq %r15, %rdi            ; File descriptor from previous openat
xorq %rsi, %rsi            ; Offset = 0
movq $2, %rdx              ; Whence = 2 (SEEK_END: move to the end of the file)
movq $8, %rax              ; Syscall number for lseek (8)
syscall                    ; Perform system call

 movq %rax, %r14           ; The result (new file offset) stored in %rax is later moved to %r14.
```

- This performs the lseek system call, which **adjusts the file offset**. Here, it seeks to the end of the file (**SEEK_END**).

**lseek** is used to adjust the file offset. The Whence argument controls how the offset is calculated:
- Whence = 0 (SEEK_SET): Offset is set relative to the start of the file.
- Whence = 1 (SEEK_CUR): Offset is set relative to the current file position.
- Whence = 2 (SEEK_END): Offset is set relative to the end of the file.

Info: https://unix.superglobalmegacorp.com/BSD4.4/newsrc/sys/unistd.h.html

### 3º System Call (mmap)
```
movq %r14, %rsi            ; File offset (from lseek)
movq $1, %rdx              ; Length = 1 byte
movq $2, %r10              ; Protection = 2 (PROT_READ)
movq %r15, %r8             ; File descriptor from openat
movq $9, %rax              ; Syscall number for mmap (9)
syscall                    ; Perform system call
```

- This **maps 1 byte of the file**, starting from the file offset obtained in the previous lseek call, **into memory**.
The result (address of the mapped memory) is returned in %rax.


### 4º System Call (write)
```
movq $1, %rdi             ; Set file descriptor to 1 (stdout)
movq %rax, %rsi           ; Move the result from mmap (the address) into %rsi
movq %r14, %rdx           ; Set the number of bytes to write (from lseek result)
movq $1, %rax             ; Syscall number for write (1)
syscall                   ; Make the write system call
```
- The program writes the contents of the mapped file (in this case, 1 byte) to stdout.

## However:

- The operating system does not map just 1 byte.

``mmap`` works in units of memory pages, which are typically 4 KB in size.

Even though the code asks for just 1 byte to be mapped (%rdx = 1), the OS rounds this up to the nearest page size, mapping an entire memory page (usually 4 KB).

As a result, when the program later writes data, it ends up accessing and writing more than just the 1 byte that was requested because the entire memory page, which includes the whole file content, is mapped.

### 5º System Call (exit_group)
```
xorq %rdi, %rdi            ; Exit status 0 (success)
movq $231, %rax            ; Syscall number for exit_group (231)
syscall                    ; Perform system call
```
- This is the exit_group system call (syscall number 231) that terminates the program

---

## After running: strace ./prog
```
isel@isel-tvs:~/Documents/semana7/ex2/x86-64$ strace ./prog
execve("./prog", ["./prog"], 0x7ffcbd517b40 /* 60 vars */) = 0
openat(AT_FDCWD, "/etc/os-release", O_RDONLY) = 3
lseek(3, 0, SEEK_END)                   = 400
mmap(NULL, 400, PROT_READ, MAP_PRIVATE, 3, 0) = 0x75a0a526f000
write(1, "PRETTY_NAME=\"Ubuntu 24.04.1 LTS\""..., 400PRETTY_NAME="Ubuntu 24.04.1 LTS"
NAME="Ubuntu"
VERSION_ID="24.04"
VERSION="24.04.1 LTS (Noble Numbat)"
VERSION_CODENAME=noble
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=noble
LOGO=ubuntu-logo
) = 400
exit_group(0)                           = ?
+++ exited with 0 +++
isel@isel-tvs:~/Documents/semana7/ex2/x86-64$ 
```
--
```
execve("./prog", ["./prog"], 0x7ffcbd517b40 /* 60 vars */) = 0
```
- The system call that starts running your program prog. It loads the program into memory and starts its execution.
- The program is executed with the argument ["./prog"] (which just runs the program without any additional command-line arguments).
- The 0x7ffcbd517b40 is a pointer to the environment variables passed to the program (60 variables in total).
```
openat(AT_FDCWD, "/etc/os-release", O_RDONLY) = 3
```
- The program opens the file /etc/os-release in read-only mode (O_RDONLY).
- AT_FDCWD is a special constant that means the file is opened relative to the current working directory.
- The system returns 3, which is the file descriptor for the opened file. File descriptor 0,1 and 2 are set up for stdin, stdout and stderr.
```
lseek(3, 0, SEEK_END) = 400
```
- The program moves the file pointer to the end of the file using the lseek system call.
- File descriptor 3 (the opened /etc/os-release file) is used here.
- SEEK_END tells the system to move the file pointer to the end. (The zero, 2nd argument, means there's no ``off_t offset``, offset of the pointer). É para começar mesmo no fim.
- The result (400) indicates that the file is 400 bytes long, meaning the file pointer is now positioned at the 400th byte (the end of the file)

```
mmap(NULL, 400, PROT_READ, MAP_PRIVATE, 3, 0) = 0x75a0a526f000
```

- The program uses the mmap system call to map 400 bytes of the file into memory.
- It maps the entire file, starting from byte 0, into memory. The memory address where the file is mapped is 0x75a0a526f000.
- ``PROT_READ``: The memory is mapped as read-only, meaning the program can read from it but not modify it.
- ``MAP_PRIVATE``: Any changes made would be private to the program (not shared with other processes).
```
write(1, "PRETTY_NAME=\"Ubuntu 24.04.1 LTS\""..., 400) = 400
```

- The program uses the ``write`` system call to write 400 bytes of data to file descriptor 1.
- File descriptor 1 is standard output (stdout); content printed to the terminal.
- The program writes the entire content of ``/etc/os-release`` (400 bytes) to the terminal.
- Result: 400 means the system successfully wrote all 400 bytes to stdout.
```
exit_group(0) = ?
+++ exited with 0 +++
```
- The program uses the ``exit_group`` system call to terminate. The **0** indicates that it exits successfully without errors.

---


# 2.a) Explicação:


(1ª system call - openat)

- O programa abre o ficheiro /etc/os-release usando a chamada de sistema openat. 

(2ª system call - lseek)

- De seguida, utiliza lseek para mover o ponteiro para o fim do ficheiro, determinando o tamanho do ficheiro (Neste caso são 400 bytes). 

(3ª system call - mmap)

- O programa pede para mapear 1 byte com mmap, mas devido ao sistema de páginas de memória do sistema operativo, acaba por mapear mais do que 1 byte (provavelmente 4KB). 

(4ª system call - write)

- A chamada de sistema write escreve a memória mapeada para o ecrã, resultando na impressão de todo o conteúdo do ficheiro, e não apenas 1 byte. 

(5ª system call - exit_group)

- Encerra o programa com sucesso, retornando o valor 0. 