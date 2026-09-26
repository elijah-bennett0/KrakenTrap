#include <stdio.h>
#include <unistd.h>

int main(void) {

    unsigned char* test_var = "FLAG";
    for (int i = 0; i < 5; i++) {
        printf("loop: %d\n", i);
        sleep(1);
    }

    return 0;
}
