#define DATA_SIZE 512*1024
#define PRIVATE_DIRTY_DATA_SIZE 128*1024

char data[DATA_SIZE] = {1};

void mark_pages_dirty() {
    for(int i = 0; i < PRIVATE_DIRTY_DATA_SIZE; i++) {
        data[i] = 'x';
    }
}
