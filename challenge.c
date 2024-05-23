#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void connect_socket(FILE **socket_r_ptr, FILE **socket_w_ptr) {

    int listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int option_value = 1;
    setsockopt(listen_socket_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value));
    struct sockaddr_in server_address = {
            .sin_family = AF_INET,
            .sin_port = htons(1234),
            .sin_addr = {INADDR_ANY}
    };
    bind(listen_socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    listen(listen_socket_fd, 5);
    int client_socket_fd = accept(listen_socket_fd, NULL, NULL);

    *socket_r_ptr = fdopen(client_socket_fd, "r");
    *socket_w_ptr = fdopen(client_socket_fd, "w");
    setbuf(*socket_w_ptr, NULL);
}

int main() {
    char notes[50][8] = {};

    FILE *client_socket_r, *client_socket_w;
    connect_socket(&client_socket_r, &client_socket_w);

    while (1) {
        char *command;
        int note_index = 0;
        if (fscanf(client_socket_r, " %ms %d", &command, &note_index) != 2)
            break;

        if (strcmp(command, "read") == 0) {
            fputs("Here is your note:\n>>>>>>>>>>>>>>>>>>>>\n", client_socket_w);
            fwrite(notes[note_index], 1, sizeof(notes[note_index]), client_socket_w);
            fputs("<<<<<<<<<<<<<<<<<<<<\n", client_socket_w);
        } else if (strcmp(command, "write") == 0) {
            fgetc(client_socket_r);
            fgets(notes[note_index], 0x100, client_socket_r);
        } else if (strcmp(command, "exit") == 0) {
            break;
        }

        free(command);
    }

}