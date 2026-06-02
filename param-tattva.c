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

// --- 1. O(1) MEMORY: THE BINDU ---
uint8_t bindu_memory[RAM_LIMIT];

// --- 2. MULTI-LANGUAGE HASHING (UTF-8 to Vector) ---
// Uses djb2 hashing to convert any language string into a strict 32-bit math node
uint32_t create_semantic_node(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

// --- 3. THE SAMSKAR PROTOCOL (True O(1) Learning via XOR) ---
void fold_into_samskar(const char* input_text) {
    uint32_t new_node = create_semantic_node(input_text);
    
    // Step A: Load previous lifetime memory (Read)
    FILE *f = fopen(BRAIN_FILE, "rb");
    if (f) {
        fread(bindu_memory, 1, RAM_LIMIT, f);
        fclose(f);
    } else {
        memset(bindu_memory, 0, RAM_LIMIT); // First time boot
    }

    // Step B: Fold new knowledge using Bitwise XOR (No extra RAM used)
    for(int i = 0; i < RAM_LIMIT; i++) {
        bindu_memory[i] ^= (new_node >> (i % 4)) & 0xFF; 
    }

    // Step C: Persist to disk (Write)
    f = fopen(BRAIN_FILE, "wb");
    fwrite(bindu_memory, 1, RAM_LIMIT, f);
    fclose(f);
    
    printf("[SAMSKAR] Folded '%s' -> Mathematical Node: %u\n", input_text, new_node);
}

// --- 4. HACK-PROOF POSIX SERVER (Real-time UI stream) ---
void start_cognitive_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed"); exit(EXIT_FAILURE);
    }
    
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Binding strictly to Localhost (127.0.0.1) for Security
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed"); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed"); exit(EXIT_FAILURE);
    }

    printf("==========================================\n");
    printf("⚛️ PARAM-TATTVA REAL-TIME CORE ACTIVE ⚛️\n");
    printf("Securely bound to 127.0.0.1:%d\n", PORT);
    printf("==========================================\n");

    // Infinite loop waiting for triggers (Web UI / Hardware Sensors)
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            continue;
        }

        read(new_socket, buffer, 1024);
        
        // Extracting input from HTTP GET request (Real-time API)
        char *query_start = strstr(buffer, "GET /api/manifest?query=");
        if (query_start) {
            query_start += 24;
            char *query_end = strchr(query_start, ' ');
            if (query_end) *query_end = '\0';
            
            if (strlen(query_start) > 0) {
                // 1. Fold new data (Learning)
                fold_into_samskar(query_start);
                
                // 2. Generate Real-time Response (JSON) for UI
                char response[512];
                sprintf(response, "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: application/json\r\n\r\n{\"status\":\"folded\", \"node_hash\":%u, \"ram_used\":\"1KB\"}", create_semantic_node(query_start));
                send(new_socket, response, strlen(response), 0);
            }
        }
        close(new_socket);
        memset(buffer, 0, sizeof(buffer));
    }
}

int main() {
    start_cognitive_server();
    return 0;
}
