cat << 'EOF' > param-tattva.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8108
#define RAM_LIMIT 1024
#define BRAIN_FILE "brain.bin"
#define SNN_THRESHOLD 15000

uint8_t bindu_memory[RAM_LIMIT];

uint32_t create_semantic_node(const char *str) {
    uint32_t hash = 5381; int c;
    while ((c = *str++)) { hash = ((hash << 5) + hash) + c; }
    return hash;
}

void load_brain() {
    FILE *f = fopen(BRAIN_FILE, "rb");
    if (f) { fread(bindu_memory, 1, RAM_LIMIT, f); fclose(f); }
    else { memset(bindu_memory, 0, RAM_LIMIT); }
}

void save_brain() {
    FILE *f = fopen(BRAIN_FILE, "wb");
    if (f) { fwrite(bindu_memory, 1, RAM_LIMIT, f); fclose(f); }
}

uint32_t process_and_fold(const char* input_text) {
    uint32_t new_node = create_semantic_node(input_text);
    load_brain();
    for(int i = 0; i < RAM_LIMIT; i++) {
        bindu_memory[i] ^= (new_node >> (i % 4)) & 0xFF; 
    }
    save_brain();
    return new_node;
}

uint32_t karma_backward_pass(const char* input_text) {
    uint32_t node_to_remove = create_semantic_node(input_text);
    load_brain();
    for(int i = 0; i < RAM_LIMIT; i++) {
        bindu_memory[i] ^= (node_to_remove >> (i % 4)) & 0xFF; 
    }
    save_brain();
    return node_to_remove;
}

void send_json_response(int socket, const char* json_body) {
    char response[1024];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s", json_body);
    send(socket, response, strlen(response), 0);
}

void start_cognitive_server() {
    int server_fd, new_socket; struct sockaddr_in address;
    int opt = 1; int addrlen = sizeof(address); char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { exit(EXIT_FAILURE); }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) { exit(EXIT_FAILURE); }
    if (listen(server_fd, 3) < 0) { exit(EXIT_FAILURE); }

        printf("\n");
    printf("      :::======== :::======== :::====  :::====::: :::==== \n");
    printf("      :::====:::: :::====:::: :::====  :::====::: :::==== \n");
    printf("      :::         :::         :::  === ::: ===::: :::  ===\n");
    printf("      :::         :::======== :::  === ::: ===::: :::==== \n");
    printf("      :::         :::======== :::====  :::     ::: :::==== \n");
    printf("      :::         :::         ::: ==== :::     ::: :::  ===\n");
    printf("\n");
    printf("         [ ⊕ ] PARAM-TATTVA CORE OS | v1.0 FINAL [ ⊕ ]\n");
    printf("=================================================================\n");
    printf("[SYSTEM] 1KB Cognitive Micro-Kernel Initialized.\n");
    printf("[STATUS] Engine Active & Locked on 127.0.0.1:%d\n", PORT);
    printf("=================================================================\n");


    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) continue;
        read(new_socket, buffer, 1024);
        
        char json_out[512];

        if (strstr(buffer, "GET /api/manifest?query=")) {
            char *q_start = strstr(buffer, "?query=") + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash = process_and_fold(q_start);
            int snn_spike = (hash > SNN_THRESHOLD) ? 1 : 0;
            
            sprintf(json_out, "{\"status\":\"success\", \"operation\":\"fold\", \"node_hash\":%u, \"snn_spike\":%d}", hash, snn_spike);
            send_json_response(new_socket, json_out);
            printf("[KERNEL] Manifested: %s | Hash: %u | Spike: %d\n", q_start, hash, snn_spike);
        }
        else if (strstr(buffer, "GET /api/karma?query=")) {
            char *q_start = strstr(buffer, "?query=") + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash = karma_backward_pass(q_start);
            sprintf(json_out, "{\"status\":\"success\", \"operation\":\"reversed\", \"node_removed\":%u}", hash);
            send_json_response(new_socket, json_out);
            printf("[KERNEL] Karma Reversed: %s\n", q_start);
        }
        else if (strstr(buffer, "GET /api/state")) {
            sprintf(json_out, "{\"os\":\"Param-Tattva\", \"memory_limit_bytes\":%d, \"status\":\"running\"}", RAM_LIMIT);
            send_json_response(new_socket, json_out);
        }
        else {
            send_json_response(new_socket, "{\"error\":\"Invalid Route.\"}");
        }
        close(new_socket); memset(buffer, 0, sizeof(buffer));
    }
}

int main() { start_cognitive_server(); return 0; }
EOF
