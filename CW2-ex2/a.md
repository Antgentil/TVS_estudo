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

### 5º System Call (exit_group)
```
xorq %rdi, %rdi            ; Exit status 0 (success)
movq $231, %rax            ; Syscall number for exit_group (231)
syscall                    ; Perform system call
```
- This is the exit_group system call (syscall number 231) that terminates the program