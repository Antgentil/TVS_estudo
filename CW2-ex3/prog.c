#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>   // For mmap and memory protection macros
#include <fcntl.h>      // For file control options (used with mmap)

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define DATA_SIZE 16*1024*1024				// 16MiB
#define RSS_INCREASE_SIZE 3*1024*1024		// 3MiB
#define PRIVATE_CLEAN_DATA_SIZE 256			// 256 bytes
#define REDUCE_PSS_TIME 1

char info[DATA_SIZE]; 			// This array is 16MiB 
char data[DATA_SIZE] = {1};

int main() {
	printf("PID: %u\n", getpid());

	const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
	printf("PAGE_SIZE: %ld\n", PAGE_SIZE);

	printf("#1 (press ENTER to continue)"); getchar();

	// a
	// The info array is part of the .bss section because it is uninitialized.
	//
	// This loop accesses 3MiB of the info array, which forces the operating system 
	// to allocate memory for this part of the array in the resident set. 
	// Accessing the memory in the .bss section triggers the operating system to 
	// assign physical memory pages to this portion of the process's address space.
	//
	// The Rss will increase by about 3MiB because that's the amount of data accessed in the info array

	for(int i = 0; i < RSS_INCREASE_SIZE; i++) {
		info[i] = 'a';
	}
	
	printf("#2 (press ENTER to continue)"); getchar();

	// b
	
	// This loop reads one byte from every page in the data array (256 pages),
    // creating private clean pages without modifying them.
    // The pages will be marked as "clean" because no writes are done, only reads.
    
    // Private clean pages: These are memory pages that have been mapped from 
	// a file (e.g., the initialized data section) and have been read but not modified.
	// They are 'clean' because they match the content in the executable and have not been written to.
	
	for(int i = 0; i < PRIVATE_CLEAN_DATA_SIZE; i++) {
		
		char temp = data[i * PAGE_SIZE];
		// The loop accesses memory at intervals of PAGE_SIZE to ensure that each access
		// touches a different memory page. This is necessary for maximum impact, as accessing 
		// just data[i] would only interact with the first page of memory, missing the chance to 
		// create more private clean pages.
	}
	
	printf("#3 (press ENTER to continue)"); getchar();

	// c
	
	// c
	// The fork() creates a new child process by duplicating the calling process.
	// After fork(), both parent and child processes SHARE memory pages in a copy-on-write manner.
	// Sleep for REDUCE_PSS_TIME (30 seconds) to observe changes in the PSS of the process.	
	
	pid_t pid = fork(); 
	sleep(REDUCE_PSS_TIME);
	if (pid == 0) return 0;

	printf("#4 (press ENTER to continue)"); getchar();

	// d
	//
	// Dynamically load the shared library "my_lib.so" using dlopen().
	// When you call dlopen() to load a shared library, the operating system performs 
	// the following actions in a single function call:
		// 1) Maps the Code Segment: The text (code) segment of the shared library is 
		// mapped into your process's address space as a new memory region with read and execute permissions.
		// 2) Maps the Data Segment: The data segment of the shared library is also mapped 
		// into your address space as a separate memory region with read and write permissions.
		
	// RTLD_LOCAL ensures the symbols defined in the shared object are local to the
	// library, and RTLD_NOW loads the library immediately. If the library is not found, 
	// an error is printed, and the program exits.

	void *lib = dlopen("./my_lib/my_lib.so", RTLD_LOCAL | RTLD_NOW);
	if (!lib) {
		printf("Error: %s\n", dlerror());
		return 1;
	}


	printf("#5 (press ENTER to continue)"); getchar();

	// e)
	//
	// Obtain the function pointer to the "mark_pages_dirty()" function inside the dynamically loaded library.
	// This is done using dlsym(), which retrieves the address of the symbol "mark_pages_dirt".
	// If the function is not found, an error message is printed, and the library is closed.
	// Call the "mark_pages_dirty()" function, which modifies 128KiB of the "data" array in the shared library.
	// This action marks those memory pages as private dirty.
		// The mark_pages_dirty() function writes to the first 128KiB of the data array, 
		// causing the operating system to mark these pages as private dirty.
	
	void (*mark_pages_dirty)() = dlsym(lib, "mark_pages_dirty");
	if (mark_pages_dirty == NULL) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(lib);
        return 1;
    }
    
	mark_pages_dirty();
	
	printf("END (press ENTER to continue)"); getchar();

	return 0;
}
