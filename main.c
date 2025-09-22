#include<stdio.h>

/*
 * building a http responses
 *
 * number in memory
 * byte number
 * null-terminated string
 * getting a string's length
 *
 */

int main() {
    char header[] = "HTTP/1.1 200 OK";
    printf("Header: %s\n", header);
    return 0;
}
