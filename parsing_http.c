#include<stdio.h>
#include<string.h>

char* to_path(char *req) {
    char* start = req;
    while (start[0] != ' ') {
        if (!start[0])
            return NULL;
        start++;
    }
}

int main(void) {
    char* req = "";
    char* path = to_path(req);

    printf("Path: %s\n", path);

    return 0;
}
