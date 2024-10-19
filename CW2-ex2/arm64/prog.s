		.section .rodata

data:   .byte  47, 101, 116,  99       // "/etc"
        .byte  47, 111, 115,  45       // "/os-"
        .byte 114, 101, 108, 101       // "rele"
        .byte  97, 115, 101,   0       // "ase\0"
        .byte  73,  83,  69,  76       // "ISEL"

		.text
		.globl _start
_start:
        // 1. openat(-100, data, 0, 0)
        mov x0, #-100                   // dirfd = AT_FDCWD (-100)
        ldr x1, =data                   // pathname = &data
        mov x2, #0                      // flags = 0
        mov w8, #56                     // syscall number for openat
        svc #0

        mov x21, x0                     // Save file descriptor to x21

        // 2. lseek(fd, 0, SEEK_END)      
        mov x2, #2                      // SEEK_END = 2
        mov x1, #0                      // offset = 0
        mov x0, x21                     // fd = x21
        mov w8, #62                     // syscall number for lseek
        svc #0

        mov x20, x0                     // Save new offset to x20

        // 3. mmap(0, length, PROT_READ, MAP_PRIVATE, fd, 0)
        mov x0, #0                      // addr = 0
        mov x5, #0                      // offset = 0
        mov x2, #1                      // PROT_READ = 1
        mov x3, #2                      // MAP_PRIVATE = 2
        mov x1, x20                     // length = x20
		mov x4, x21                     // fd = x21 
        mov w8, #222                    // syscall number for mmap
        svc #0

        mov x19, x0                     // Save mapped address to x19

        // 4. write(1, buffer, length)
        mov x1, x19                     // buffer = x19
        mov x2, x20                     // length = x20
        mov x0, #1                      // fd = 1 (stdout)
        mov w8, #64                     // syscall number for write
        svc #0

        mov x18, x0                     // Save bytes written to x18

        // 5. exit(0)
        mov x0, #0                      // exit status = 0
        mov w8, #93                     // syscall number for exit
        svc #0

		.end
