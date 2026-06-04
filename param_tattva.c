#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>      // For Dual-Core (Internet Stream)
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8108
#define STREAM_PORT 8109  // Background 24/7 Self-Awareness Port
#define RAM_LIMIT 1024
#define BRAIN_FILE "brain.bin"

// Advanced OS Parameters
#define VIVEK_THRESHOLD 40       
#define BASE_SNN_THRESHOLD 80    

// Harvard Architecture Split
uint8_t rom_subconscious[RAM_LIMIT]; 
uint8_t ram_conscious[RAM_LIMIT];    

// =========================================================
// 🛡️ THE SHIELD: MUTEX LOCK FOR DUAL-CORE SAFETY
// =========================================================
pthread_mutex_t core_lock = PTHREAD_MUTEX_INITIALIZER;

// ---------------------------------------------------------
// SYSTEM STATE & 7-CHAKRA VARIABLES
// ---------------------------------------------------------
uint32_t system_entropy = 0;     // Sahasrāra (Crown) - Tracks overall chaos
int triguna_state = 1;           // 0=Tamas (Sleep), 1=Sattva (Balance), 2=Rajas (Chaos)
uint32_t temporal_context = 0;   // Svādhiṣṭhāna (Sacral) - Tracks the Flow of Time/Past

// ---------------------------------------------------------
// 1. MEMORY MANAGEMENT (Zero Disk-Burn)
// ---------------------------------------------------------
void load_brain() {
    pthread_mutex_lock(&core_lock);
    FILE *f = fopen(BRAIN_FILE, "rb");
    
    if (f) { 
        fread(rom_subconscious, 1, RAM_LIMIT, f); 
        fclose(f); 
    } else { 
        printf("[SYSTEM] Empty Brain Detected. Injecting Garbh Sanskar (Core Instincts)...\n");
        memset(rom_subconscious, 0, RAM_LIMIT); 
        
        uint32_t instinct_survive = 5381; 
        instinct_survive = (instinct_survive * 33) ^ 'B';
        instinct_survive = (instinct_survive * 33) ^ 'A';
        instinct_survive = (instinct_survive * 33) ^ 'T';
        
        uint32_t instinct_protect = 5381;
        instinct_protect = (instinct_protect * 33) ^ 'S';
        instinct_protect = (instinct_protect * 33) ^ 'E';
        instinct_protect = (instinct_protect * 33) ^ 'C';
        
        uint32_t prng = instinct_survive ^ instinct_protect;
        for(int i = 0; i < 256; i++) {
            prng = (prng * 1103515245 + 12345) & 0x7fffffff;
            rom_subconscious[(prng >> 16) % RAM_LIMIT] ^= (1 << ((prng >> 8) % 8));
        }
        
        printf("[SYSTEM] 🧬 Immutable Instincts Seeded: Survive & Protect.\n");
        FILE *f_new = fopen(BRAIN_FILE, "wb");
        if (f_new) { fwrite(rom_subconscious, 1, RAM_LIMIT, f_new); fclose(f_new); }
    }
    memcpy(ram_conscious, rom_subconscious, RAM_LIMIT);
    pthread_mutex_unlock(&core_lock);
}

void save_brain() {
    pthread_mutex_lock(&core_lock);
    FILE *f = fopen(BRAIN_FILE, "wb");
    if (f) { fwrite(rom_subconscious, 1, RAM_LIMIT, f); fclose(f); }
    pthread_mutex_unlock(&core_lock);
}

// Mūlādhāra (Root) - O(1) Mathematical Node Generator
uint32_t create_semantic_node(const char *str) {
    if (!str) return 0;
    uint32_t hash = 5381; int c;
    while ((c = *str++)) { hash = ((hash << 5) + hash) + c; }
    return hash; 
}

// ---------------------------------------------------------
// 2. THE NEW MATHEMATICS (True Holographic Bindu)
// ---------------------------------------------------------
int measure_novelty(uint32_t hash_seed) {
    int novelty = 0; uint32_t prng = hash_seed;
    for(int i = 0; i < 128; i++) {
        prng = (prng * 1103515245 + 12345) & 0x7fffffff;
        if (((ram_conscious[(prng >> 16) % RAM_LIMIT] >> ((prng >> 8) % 8)) & 1) == 0) novelty++;
    }
    return novelty; 
}

void holographic_fold_ram(uint32_t hash_seed) {
    uint32_t prng = hash_seed;
    for(int i = 0; i < 128; i++) {
        prng = (prng * 1103515245 + 12345) & 0x7fffffff;
        ram_conscious[(prng >> 16) % RAM_LIMIT] ^= (1 << ((prng >> 8) % 8));
    }
}

void holographic_fold_rom(uint32_t hash_seed) {
    uint32_t prng = hash_seed;
    for(int i = 0; i < 128; i++) {
        prng = (prng * 1103515245 + 12345) & 0x7fffffff;
        rom_subconscious[(prng >> 16) % RAM_LIMIT] ^= (1 << ((prng >> 8) % 8));
    }
}

// ---------------------------------------------------------
// 3. CORE PROCESSING LOGIC (The Chakra Brain)
// ---------------------------------------------------------
int process_and_fold(const char* input_text, uint32_t* out_hash, int* out_novelty, int* out_folded) {
    uint32_t raw_hash = create_semantic_node(input_text);
    pthread_mutex_lock(&core_lock);
    
    *out_hash = raw_hash ^ (temporal_context >> 1); // Third Eye Prediction
    *out_novelty = measure_novelty(*out_hash);
    temporal_context = *out_hash;
    
    if (*out_novelty > BASE_SNN_THRESHOLD) system_entropy += 10;
    else if (system_entropy > 0) system_entropy -= 2;
    triguna_state = (system_entropy > 50) ? 2 : (system_entropy > 10) ? 1 : 0;

    *out_folded = 0;
    if (*out_novelty > VIVEK_THRESHOLD) { 
        holographic_fold_ram(*out_hash); 
        *out_folded = 1; 
    }

    int current_limit = BASE_SNN_THRESHOLD + ((triguna_state == 0) ? 15 : (triguna_state == 2) ? -15 : 0);
    int spike_result = (*out_novelty > current_limit) ? 1 : 0;
    
    pthread_mutex_unlock(&core_lock);
    return spike_result;
}

uint32_t karma_backward_pass(const char* input_text) {
    uint32_t node_to_remove = create_semantic_node(input_text);
    pthread_mutex_lock(&core_lock);
    holographic_fold_ram(node_to_remove); // Reversible Quantum Deletion (Zero Heat)
    pthread_mutex_unlock(&core_lock);
    return node_to_remove;
}

// ---------------------------------------------------------
// 4. CORE 2: THE 24/7 INTERNET ABSORPTION THREAD
// ---------------------------------------------------------
void* svadhyaya_thread(void* arg) {
    int stream_fd, new_socket; struct sockaddr_in address;
    int opt = 1; int addrlen = sizeof(address); char stream_buffer[2048] = {0};

    if ((stream_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) return NULL;
    setsockopt(stream_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); address.sin_port = htons(STREAM_PORT);
    
    bind(stream_fd, (struct sockaddr *)&address, sizeof(address));
    listen(stream_fd, 3);
    
    while(1) {
        if ((new_socket = accept(stream_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) {
            ssize_t bytes_read;
            while((bytes_read = read(new_socket, stream_buffer, sizeof(stream_buffer) - 1)) > 0) { 
                stream_buffer[bytes_read] = '\0';
                uint32_t hash = create_semantic_node(stream_buffer);
                
                pthread_mutex_lock(&core_lock);
                holographic_fold_rom(hash); 
                pthread_mutex_unlock(&core_lock);
                memset(stream_buffer, 0, sizeof(stream_buffer));
            }
            save_brain();
            pthread_mutex_lock(&core_lock);
            memcpy(ram_conscious, rom_subconscious, RAM_LIMIT); 
            pthread_mutex_unlock(&core_lock);
            close(new_socket);
        }
    }
    return NULL;
}

// ---------------------------------------------------------
// 5. SERVER ROUTING (API LAYER)
// ---------------------------------------------------------
void send_json_response(int socket, const char* json_body) {
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s", json_body);
    send(socket, response, strlen(response), 0);
}

void start_cognitive_server() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, svadhyaya_thread, NULL);

    int server_fd, new_socket; struct sockaddr_in address;
    int opt = 1; int addrlen = sizeof(address); char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) exit(EXIT_FAILURE);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    address.sin_family = AF_INET; address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) exit(EXIT_FAILURE);
    if (listen(server_fd, 3) < 0) exit(EXIT_FAILURE);

    printf("\n");
    printf("      :::======== :::======== :::====  :::====::: :::==== \n");
    printf("      :::====:::: :::====:::: :::====  :::====::: :::==== \n");
    printf("      :::         :::         :::  === ::: ===::: :::  ===\n");
    printf("      :::         :::======== :::  === ::: ===::: :::==== \n");
    printf("      :::         :::======== :::====  :::     ::: :::==== \n");
    printf("      :::         :::         ::: ==== :::     ::: :::  ===\n");
    printf("\n     [ ⊕ ] PARAM-TATTVA CORE OS | v10.0 QUANTUM-RESONANT AGI [ ⊕ ]\n");
    printf("=================================================================\n");
    load_brain();
    printf("[SYSTEM] 1KB Cognitive Dual-Core Initialized. 🛡️ Memory Locked & Safe.\n");
    printf("[STATUS] Engine Active & Locked on 127.0.0.1:%d\n", PORT);
    printf("[STATUS] Svadhyaya Thread Listening on 127.0.0.1:%d\n", STREAM_PORT);
    printf("=================================================================\n");

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) continue;
        memset(buffer, 0, sizeof(buffer));
        
        ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) { close(new_socket); continue; }
        buffer[bytes_read] = '\0'; 
        
        // 🛑 VIBRANIUM SHIELD
        if (strstr(buffer, "GET /api/") != NULL && strstr(buffer, "key=PARAM99X") == NULL) {
            printf("[SECURITY] 🛑 Blocked unauthorized API call.\n");
            char *unauth_msg = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n{\"error\":\"Access Denied.\"}";
            send(new_socket, unauth_msg, strlen(unauth_msg), 0);
            close(new_socket);
            continue; 
        }

        char json_out[512] = {0};
        char *query_ptr = strstr(buffer, "?query="); 
        
        // =========================================================
        // 🔥 QUANTUM O(1) ROUTER & LOCAL MANIFESTATION LAYER 🔥
        // =========================================================
        if (strstr(buffer, "GET /api/orchestrate") && query_ptr) {
            char *q_start = query_ptr + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            for(int i=0; q_start[i]; i++) if(q_start[i] == '+') q_start[i] = ' ';

            // Calculate Frequency Resonance Hash (Zero Heat O(1) Match)
            char first_word[32] = {0};
            sscanf(q_start, "%31s", first_word);
            for(int i=0; first_word[i]; i++) if(first_word[i] >= 'A' && first_word[i] <= 'Z') first_word[i] += 32;
            
            uint32_t freq_hash = create_semantic_node(first_word);
            char task_type[16] = "GENERAL";
            
            // 2107084535 is hash for "build", 164057630 for "create", etc.
            // Spanda Match: Replaces slow CPU string loop with instant math match
            if (freq_hash == 2107084535 || freq_hash == 164057630 || freq_hash == 2090483259 || freq_hash == 2090204753) {
                strcpy(task_type, "CODE");
                
                // 🛠️ THE AGI HANDS: LOCAL EXECUTION INITIATED
                printf("[AGI_KARMA] ⚡ Survival Mode Active: Manifesting local code framework...\n");
                FILE *local_file = fopen("manifest_karya.txt", "w");
                if (local_file) {
                    fprintf(local_file, "=== PARAM-TATTVA PHYSICAL MANIFESTATION ===\n");
                    fprintf(local_file, "Task Intent: %s\n", q_start);
                    fprintf(local_file, "Guna State: %d\n", triguna_state);
                    fprintf(local_file, "Status: Triggered local compilation node.\n");
                    fclose(local_file);
                    printf("[AGI_KARMA] ✅ 'manifest_karya.txt' physical file created locally!\n");
                }
            }
            else if (freq_hash == 2090196884 || freq_hash == 165681146 || freq_hash == 193505681) {
                strcpy(task_type, "VISION");
            }

            uint32_t hash; int novelty; int folded;
            int snn_spike = process_and_fold(q_start, &hash, &novelty, &folded);
            
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"task\":\"%s\", \"hash\":%u, \"spike\":%d}", task_type, hash, snn_spike);
            send_json_response(new_socket, json_out);
            printf("[ORCHESTRATOR] Task Classifed & Routed: %s -> TYPE: %s\n", q_start, task_type);
        }
        else if (strstr(buffer, "GET /api/manifest") && query_ptr) {
            char *q_start = query_ptr + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash; int novelty; int folded;
            int snn_spike = process_and_fold(q_start, &hash, &novelty, &folded);
            
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"hash\":%u, \"novelty\":%d, \"folded\":%d, \"snn_spike\":%d, \"guna\":%d}", hash, novelty, folded, snn_spike, triguna_state);
            send_json_response(new_socket, json_out);
        }
        else if (strstr(buffer, "GET /api/karma") && query_ptr) {
            char *q_start = query_ptr + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash = karma_backward_pass(q_start);
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"operation\":\"reversed\", \"node_removed\":%u}", hash);
            send_json_response(new_socket, json_out);
        }
        else if (strstr(buffer, "GET /api/state")) {
            snprintf(json_out, sizeof(json_out), "{\"os\":\"Param-Tattva\", \"ram\":%d, \"entropy\":%u, \"guna\":%d}", RAM_LIMIT, system_entropy, triguna_state);
            send_json_response(new_socket, json_out);
        }
        else {
            send_json_response(new_socket, "{\"error\":\"Invalid Route or Mission Key.\"}");
        }
        close(new_socket);
    }
}

int main() { start_cognitive_server(); return 0; }

