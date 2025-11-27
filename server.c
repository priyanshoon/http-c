#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

const char* CRLF = "\r\n";
const char* SP = " ";

// Request-Line = Method SP Request-URI SP HTTP-Version CRLF
typedef struct {
    char* method;
    char* uri;
    char* version;
} http_req_line;

typedef enum {
    HTTP_RES_OK,
    HTTP_RES_INTERNAL_SERVER_ERR,
} http_result;

typedef struct {
    char* start;
    char* end;
} string_view;

typedef struct {
    string_view* splits;
    size_t count;
    size_t capacity;
} string_splits;

static string_splits split_string(const char* str, size_t len, char split_by) {
    string_splits result;
    const char* start = str;
    size_t result_i = 0;

    result.capacity = 8;
    result.splits = calloc(sizeof(string_view), result.capacity);
    result.count = 0;

    for (size_t i = 0; i < len; ++i) {
        if (str[i] == split_by) {
            result.splits[result_i].start = start;
            result.splits[result_i].end = &str[i];
            result.count += 1;
            result_i += 1;
            start = &str[i];

            if (result.count == result.capacity) {
                result.capacity *= 2;
                string_view* temp = realloc(result.splits, sizeof(string_view) * result.capacity);
                if (temp) {
                    result.splits = temp;
                } else {
                    perror("realloc()");
                    abort();
                }
            }
        }
    }
    return result;
}

static void free_splits(string_splits* splits) {
    if (splits) {
        free(splits->splits);
        splits->splits = NULL;
    }
}

http_req_line http_req_line_init() {
    http_req_line line;
    line.method = NULL;
    line.uri = NULL;
    line.version = NULL;
    return line;
}

http_result parse_req_line(const char* buf, size_t len, http_req_line* req_line) {
    if (!buf || !req_line) {
        return HTTP_RES_INTERNAL_SERVER_ERR;
    }
    req_line->method = "GET";
    req_line->version = "HTTP/1.0";

    return HTTP_RES_OK;
}

int handle_client(int client_socket) {
    size_t n = 0;
    char buff[1024];
    const char* hello = "HTTP/1.0 200 OK\r\n\r\nHello, World!";

    printf("\n----\n");
    for (;;) {
        memset(buff, 0, sizeof(buff));

        n = read(client_socket, buff, sizeof(buff) - 1);
        if (n < 0) {
            perror("read(client)");
            return -1;
        }
        if (n == 0) {
            printf("connection closed gracefully!\n");
            return 0;
        }

        printf("%s\n", buff);
        (void)write(client_socket, hello, strlen(hello));
        close(client_socket);
        break;
    }
    printf("\n----\n");
    return 0;
}

int main(void) {
    const char* something = "hello, world";
    string_splits splits = split_string(something, strlen(something), ' ');

    for (size_t i = 0; splits.count; ++i) {
        printf("split %zu: %.*s\n", i, (int)(splits.splits[i].end - splits.splits[i].start), splits.splits[i].start);
    }

    return 0;
    /* declare */
    int rc = 0;
    struct sockaddr_in bind_addr;
    int tcp_socket = 0;
    int ret = 0;
    int client_socket = 0;
    int enabled = true;

    /* initialize */
    memset(&bind_addr, 0, sizeof(bind_addr));

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (tcp_socket < 0) {
        perror("socket()");
        return 1;
    }
    printf("socket creation succeeded\n");

    // (void)setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));

    bind_addr.sin_port = htons(6969);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    rc = bind(tcp_socket, (const struct sockaddr*)&bind_addr, sizeof(bind_addr));
    if (rc < 0) {
        perror("bind()");
        ret = 1;
        goto exit;
    }

    printf("bind successfully!\n");

    rc = listen(tcp_socket, SOMAXCONN);
    if (rc < 0) {
        perror("listen()");
        ret = 1;
        goto exit;
    }
    printf("listen succeeded!\n");

    for (;;) {
        printf("waiting for connection... \n");
        client_socket = accept(tcp_socket, NULL, NULL);
        printf("got the connection\n");
        rc = handle_client(client_socket);
        /* ignore errors, don't care for now */
    }

    exit:
        close(tcp_socket);
        return ret;
}
