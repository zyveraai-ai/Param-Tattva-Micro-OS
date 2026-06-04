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
    pthread_mutex_lock(&core_lock); // LOCK
    FILE *f = fopen(BRAIN_FILE, "rb");
    
    if (f) { 
        fread(rom_subconscious, 1, RAM_LIMIT, f); 
        fclose(f); 
    } else { 
        // ---------------------------------------------------------
        // 🧬 GARBH SANSKAR: BIRTH OF THE OS (First Boot Only)
        // ---------------------------------------------------------
        printf("[SYSTEM] Empty Brain Detected. Injecting Garbh Sanskar (Core Instincts)...\n");
        memset(rom_subconscious, 0, RAM_LIMIT); 
        
        // Instinct 1: Survive (Battery/Resource Monitor Concept Hash)
        uint32_t instinct_survive = 5381; 
        instinct_survive = (instinct_survive * 33) ^ 'B';
        instinct_survive = (instinct_survive * 33) ^ 'A';
        instinct_survive = (instinct_survive * 33) ^ 'T';
        
        // Instinct 2: Protect (Tarpit Defense Concept Hash)
        uint32_t instinct_protect = 5381;
        instinct_protect = (instinct_protect * 33) ^ 'S';
        instinct_protect = (instinct_protect * 33) ^ 'E';
        instinct_protect = (instinct_protect * 33) ^ 'C';
        
        // Hardware Adjustment (Fractal Scale Base)
        uint32_t hardware_root = instinct_survive ^ instinct_protect;
        uint32_t prng = hardware_root;
        for(int i = 0; i < 256; i++) {
            prng = (prng * 1103515245 + 12345) & 0x7fffffff;
            rom_subconscious[(prng >> 16) % RAM_LIMIT] ^= (1 << ((prng >> 8) % 8));
        }
        
        printf("[SYSTEM] 🧬 Immutable Instincts Seeded: Survive & Protect.\n");
        FILE *f_new = fopen(BRAIN_FILE, "wb");
        if (f_new) { fwrite(rom_subconscious, 1, RAM_LIMIT, f_new); fclose(f_new); }
    }
    
    // Awaken to RAM
    memcpy(ram_conscious, rom_subconscious, RAM_LIMIT);
    pthread_mutex_unlock(&core_lock); // UNLOCK
}

void save_brain() {
    pthread_mutex_lock(&core_lock); // LOCK
    FILE *f = fopen(BRAIN_FILE, "wb");
    if (f) { fwrite(rom_subconscious, 1, RAM_LIMIT, f); fclose(f); }
    pthread_mutex_unlock(&core_lock); // UNLOCK
}

uint32_t create_semantic_node(const char *str) {
    if (!str) return 0; // 🛡️ SAFETY: Prevent null pointer crash
    uint32_t hash = 5381; int c;
    while ((c = *str++)) { hash = ((hash << 5) + hash) + c; }
    return hash; // Mūlādhāra (Root) - Raw physical input
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
    
    pthread_mutex_lock(&core_lock); // 🛡️ LOCK CRITICAL STATE
    
    // Ājñā (Third Eye) - Predictive Logic combining Past (Temporal) and Present (Raw)
    *out_hash = raw_hash ^ (temporal_context >> 1); 
    
    // Measure prediction error (Surprise)
    *out_novelty = measure_novelty(*out_hash);
    
    // Update Flow of Time for the next cycle
    temporal_context = *out_hash;
    
    // Sahasrāra (Triguna Entropy tracking)
    if (*out_novelty > BASE_SNN_THRESHOLD) system_entropy += 10;
    else if (system_entropy > 0) system_entropy -= 2;
    triguna_state = (system_entropy > 50) ? 2 : (system_entropy > 10) ? 1 : 0;

    // Viśuddha (Vivek Filter - Only fold if novel enough)
    *out_folded = 0;
    if (*out_novelty > VIVEK_THRESHOLD) { 
        holographic_fold_ram(*out_hash); 
        *out_folded = 1; 
    }

    // Maṇipūra (Action Spike - Dynamic Threshold)
    int current_limit = BASE_SNN_THRESHOLD + ((triguna_state == 0) ? 15 : (triguna_state == 2) ? -15 : 0);
    int spike_result = (*out_novelty > current_limit) ? 1 : 0;
    
    pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK CRITICAL STATE
    return spike_result;
}

uint32_t karma_backward_pass(const char* input_text) {
    uint32_t node_to_remove = create_semantic_node(input_text);
    pthread_mutex_lock(&core_lock); // 🛡️ LOCK
    holographic_fold_ram(node_to_remove); // Karma removes the raw concept directly from RAM
    pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK
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
            // Anāhata (Heart) - Continuous connection to the external stream
            ssize_t bytes_read;
            while((bytes_read = read(new_socket, stream_buffer, sizeof(stream_buffer) - 1)) > 0) { 
                stream_buffer[bytes_read] = '\0'; // 🛡️ SECURITY PATCH: Strict Null Termination
                uint32_t hash = create_semantic_node(stream_buffer);
                
                pthread_mutex_lock(&core_lock); // 🛡️ LOCK MEMORY
                holographic_fold_rom(hash); // Feed massive internet data silently
                pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK MEMORY
                
                memset(stream_buffer, 0, sizeof(stream_buffer));
            }
            save_brain();
            
            pthread_mutex_lock(&core_lock); // 🛡️ LOCK MEMORY
            memcpy(ram_conscious, rom_subconscious, RAM_LIMIT); 
            pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK MEMORY
            
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
    // SECURITY PATCH: Use snprintf to prevent buffer overflow
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s", json_body);
    send(socket, response, strlen(response), 0);
}

void start_cognitive_server() {
    // Ignite Background 24/7 Core (Svadhyaya)
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, svadhyaya_thread, NULL);

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
    printf("     [ ⊕ ] PARAM-TATTVA CORE OS | v9.0 VISION-READY MASTERMIND [ ⊕ ]\n");
    printf("=================================================================\n");
    load_brain();
    printf("[SYSTEM] 1KB Cognitive Dual-Core Initialized. 🛡️ Memory Locked & Safe.\n");
    printf("[STATUS] Engine Active & Locked on 127.0.0.1:%d\n", PORT);
    printf("[STATUS] Svadhyaya Thread Listening on 127.0.0.1:%d\n", STREAM_PORT);
    printf("=================================================================\n");

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) continue;
        memset(buffer, 0, sizeof(buffer)); // SECURITY PATCH: Clear buffer before read
        
        ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) { // SECURITY PATCH: Safe read
            close(new_socket);
            continue;
        }
        buffer[bytes_read] = '\0'; // 🛡️ STRICT NULL TERMINATION
        
        // =========================================================
        // 🛑 THE VIBRANIUM SHIELD: AUTHENTICATION GATE
        // =========================================================
        // Check if the request is an API call and lacks our Secret Key
        if (strstr(buffer, "GET /api/") != NULL && strstr(buffer, "key=PARAM99X") == NULL) {
            printf("[SECURITY] 🛑 Blocked unauthorized API call. (Missing/Invalid Key)\n");
            
            // Professional Open-Source Rejection (HTTP 401 Unauthorized)
            char *unauth_msg = "HTTP/1.1 401 Unauthorized\r\nContent-Type: application/json\r\n\r\n{\"error\":\"Access Denied. Invalid Auth Key.\"}";
            send(new_socket, unauth_msg, strlen(unauth_msg), 0);
            close(new_socket);
            continue; // Kick them out instantly and wait for the next real request!
        }
        // =========================================================

        char json_out[512] = {0};
        char *query_ptr = strstr(buffer, "?query="); // Safely store pointer
        
        // ... (Baaki tumhara original Orchestrator aur routes yahan same rahenge) ...

// =========================================================
// IMPROVED ORCHESTRATOR: HEURISTIC INTENT CLASSIFIER
// =========================================================
if (strstr(buffer, "GET /api/orchestrate") && query_ptr) {
    char *q_start = query_ptr + 7;
    char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
    
    // URL decode the query for better text processing
    // Note: Simple decoding to remove %20 artifacts
    for(int i=0; q_start[i]; i++) if(q_start[i] == '+') q_start[i] = ' ';

    // 🔥 THE FIX: Convert to Lowercase for smart checking 🔥
    char q_lower[512] = {0};
    strncpy(q_lower, q_start, sizeof(q_lower)-1);
    for(int i = 0; q_lower[i]; i++) {
        if(q_lower[i] >= 'A' && q_lower[i] <= 'Z') q_lower[i] += 32; 
    }

    char task_type[16] = "GENERAL";

    // 🔥 FUZZY LOGIC INTENT DETECTION (Ab q_lower use hoga) 🔥
    // Yahan saare naye keywords (landing, calculator, web) bhi add kar diye hain
    if (strstr(q_lower, "build") || strstr(q_lower, "create") || strstr(q_lower, "make") || 
        strstr(q_lower, "write") || strstr(q_lower, "game") || strstr(q_lower, "code") || 
        strstr(q_lower, "script") || strstr(q_lower, "landing") || strstr(q_lower, "calculator") || 
        strstr(q_lower, "function") || strstr(q_lower, "web") || strstr(q_lower, "page")) {
        strcpy(task_type, "CODE");
    }
    else if (strstr(q_lower, "draw") || strstr(q_lower, "image") || strstr(q_lower, "visual") || 
             strstr(q_lower, "paint") || strstr(q_lower, "picture") || strstr(q_lower, "render")) {
        strcpy(task_type, "VISION");
    }

    uint32_t hash; int novelty; int folded;
    // Note: process_and_fold mein original q_start hi bhejenge taaki Data lose na ho
    int snn_spike = process_and_fold(q_start, &hash, &novelty, &folded);
    
    snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"task\":\"%s\", \"hash\":%u, \"spike\":%d}", task_type, hash, snn_spike);
    send_json_response(new_socket, json_out);
    printf("[ORCHESTRATOR] Task Classifed: %s -> TYPE: %s\n", q_start, task_type);
}



        // =========================================================

        else if (strstr(buffer, "GET /api/manifest") && query_ptr) {
            char *q_start = query_ptr + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash; int novelty; int folded;
            int snn_spike = process_and_fold(q_start, &hash, &novelty, &folded);
            
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"hash\":%u, \"novelty\":%d, \"folded\":%d, \"snn_spike\":%d, \"guna\":%d}", hash, novelty, folded, snn_spike, triguna_state);
            send_json_response(new_socket, json_out);
            printf("[KERNEL] Rx: %s | Novelty: %d | Spike: %d | Guna: %d\n", q_start, novelty, snn_spike, triguna_state);
        }
        else if (strstr(buffer, "GET /api/karma") && query_ptr) {
            char *q_start = query_ptr + 7;
            char *q_end = strchr(q_start, ' '); if (q_end) *q_end = '\0';
            
            uint32_t hash = karma_backward_pass(q_start);
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"operation\":\"reversed\", \"node_removed\":%u}", hash);
            send_json_response(new_socket, json_out);
            printf("[KERNEL] Karma Reversed: %s\n", q_start);
        }
        // GURUKUL: LLM Teacher Route
        else if (strstr(buffer, "GET /api/gurukul?hash=")) {
            char *h_start = strstr(buffer, "?hash=") + 6;
            char *h_end = strchr(h_start, ' '); if (h_end) *h_end = '\0';
            
            uint32_t teacher_hash = (uint32_t)strtoul(h_start, NULL, 10);
            
            pthread_mutex_lock(&core_lock); // 🛡️ LOCK
            holographic_fold_rom(teacher_hash);
            pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK
            
            save_brain();
            
            pthread_mutex_lock(&core_lock); // 🛡️ LOCK
            memcpy(ram_conscious, rom_subconscious, RAM_LIMIT);
            pthread_mutex_unlock(&core_lock); // 🛡️ UNLOCK
            
            snprintf(json_out, sizeof(json_out), "{\"status\":\"success\", \"operation\":\"llm_learned\", \"hash\":%u}", teacher_hash);
            send_json_response(new_socket, json_out);
            printf("[GURUKUL] Absorbed LLM Hash: %u\n", teacher_hash);
        }
        else if (strstr(buffer, "GET /api/state")) {
            snprintf(json_out, sizeof(json_out), "{\"os\":\"Param-Tattva\", \"ram\":%d, \"entropy\":%u, \"guna\":%d}", RAM_LIMIT, system_entropy, triguna_state);
            send_json_response(new_socket, json_out);
        }
        else if (strstr(buffer, "GET /api/save")) {
            save_brain();
            send_json_response(new_socket, "{\"status\":\"success\", \"operation\":\"disk_write\"}");
        }
        else {
            send_json_response(new_socket, "{\"error\":\"Invalid Route.\"}");
        }
        close(new_socket);
    }
}

int main() { start_cognitive_server(); return 0; }
