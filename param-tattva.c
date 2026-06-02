#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <ctype.h>
#include <math.h>

#define PORT 8108
#define DIMS 256

// --- CORE MEMORY (BINDU) ---
int32_t BINDU[DIMS] = {0}; 
uint32_t total_samskaras = 0; 
char kernel_state[256] = "System Secure & Optimal";

void load_brain() {
    FILE *f = fopen("brain.bin", "rb");
    if(f) { 
        fread(BINDU, sizeof(int32_t), DIMS, f); 
        fread(&total_samskaras, sizeof(uint32_t), 1, f);
        fclose(f); 
        printf("[SYSTEM] brain.bin loaded successfully. Nodes: %d\n", total_samskaras);
    } else {
        printf("[SYSTEM] No previous memory found. Initializing new Bindu.\n");
    }
}

void save_brain() {
    FILE *f = fopen("brain.bin", "wb");
    if(f) { 
        fwrite(BINDU, sizeof(int32_t), DIMS, f); 
        fwrite(&total_samskaras, sizeof(uint32_t), 1, f);
        fclose(f); 
    }
}

int check_SNN_spike(float voltage, float threshold) { return (voltage >= threshold) ? 1 : 0; }

void quantum_fold_knowledge(uint32_t hash) {
    for(int i = 0; i < DIMS; i++) { BINDU[i] ^= (hash % (i + 1 + total_samskaras)); }
    total_samskaras++;
    save_brain(); 
}

// --- FRONTEND UI (MAYA) ---
const char *MANIFEST_UI = 
"<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"<title>Param-Tattva OS v1.0</title>"
"<style>"
"body{background:#000;color:#0ff;font-family:monospace;margin:0;overflow:hidden; height:100vh;}"
"#hud{position:absolute;top:15px;left:15px;background:rgba(0,10,10,0.85);padding:15px;border:1px solid #0f0;border-radius:8px;z-index:999;box-shadow:0 0 10px #0f0; width:85%; max-width:350px;}"
"#prompt_box{position:absolute;bottom:30px;left:50%;transform:translateX(-50%);width:95%;max-width:600px;display:flex;gap:10px;z-index:999;}"
"input{flex:1;padding:12px;background:rgba(0,0,0,0.9);border:2px solid #0ff;color:#0ff;border-radius:5px;outline:none;font-size:16px;}"
"button{padding:12px 20px;background:#0ff;color:#000;font-weight:bold;border:none;border-radius:5px;cursor:pointer;}"
"video{position:absolute;top:0;left:0;width:100vw;height:100vh;object-fit:cover;z-index:1;opacity:0.25;}" 
"canvas{position:absolute;top:0;left:0;width:100vw;height:100vh;z-index:2;mix-blend-mode:screen;}" 
"#semantic_data{font-size:11px; color:#aaa; margin-top:10px; border-top:1px dashed #0f0; padding-top:10px; max-height:100px; overflow-y:auto;}"
"</style></head><body>"
"<video id='cam' autoplay playsinline muted></video>"
"<canvas id='canvas'></canvas>"
"<div id='hud'>"
"  <b style='font-size:18px;color:#0f0'>⚛️ PARAM-TATTVA OS</b><br><br>"
"  <div id='os_status'>Initializing Core...</div>"
"  <div id='math_string' style='color:#ff0; margin-top:10px; font-size:12px;'>Awaiting Input.</div>"
"  <div id='semantic_data'>Hardware: Online<br>Memory: Mounted<br>Security: Localhost Locked</div>"
"</div>"
"<div id='prompt_box'>"
"  <input type='text' id='prompt' placeholder='Enter concept (e.g. Brain, Galaxy)...' onkeypress='if(event.key===\"Enter\") manifest()'>"
"  <button onclick='manifest()'>EXECUTE</button>"
"</div>"

"<script>"
"const video = document.getElementById('cam');"
"navigator.mediaDevices.getUserMedia({video:{facingMode:'environment'}}).then(s=>{video.srcObject=s;}).catch(e=>console.log('No Cam'));"
"const canvas = document.getElementById('canvas'); const ctx = canvas.getContext('2d');"
"canvas.width = window.innerWidth; canvas.height = window.innerHeight;"
"let particles = []; let animationId = null;"

"function spawnOmniscientMaya(mathArr) {"
"  if(animationId) cancelAnimationFrame(animationId);"
"  particles = []; ctx.clearRect(0,0,canvas.width,canvas.height);"
"  let p1 = mathArr[0]/255; let p2 = mathArr[1]/255; let p3 = mathArr[2]/255;"
"  for(let i=0; i<150; i++) {"
"    particles.push({ angle: i * 0.1, radius: i * p1 * 3, speed: (p2 * 0.05) + 0.01, size: (Math.random() * p3 * 5) + 1, hue: (i * p2 * 100) % 360 });"
"  }"
"  function draw() {"
"    ctx.fillStyle = 'rgba(0, 0, 0, 0.15)'; ctx.fillRect(0, 0, canvas.width, canvas.height);"
"    particles.forEach(p => {"
"      p.angle += p.speed; let px = canvas.width/2 + Math.cos(p.angle) * p.radius; let py = canvas.height/2 + Math.sin(p.angle) * p.radius;"
"      ctx.beginPath(); ctx.arc(px, py, p.size, 0, Math.PI*2); ctx.fillStyle = `hsl(${p.hue + (p.angle*50)}, 100%, 50%)`; ctx.fill();"
"    });"
"    animationId = requestAnimationFrame(draw);"
"  }"
"  draw();"
"}"

"function manifest() {"
"  let val = document.getElementById('prompt').value; if(!val) return;"
"  document.getElementById('math_string').innerHTML = '<i>Processing via C-Kernel...</i>';"
"  fetch('/api/manifest?query=' + encodeURIComponent(val) + '&t=' + Date.now())"
"  .then(r => r.json()).then(data => {"
"    if(data.error) return;"
"    document.getElementById('math_string').innerHTML = '<b>Hash Signature:</b> [' + data.string_math + ']';"
"    document.getElementById('semantic_data').innerHTML = '<b>Cognized Data:</b><br>' + data.meaning;"
"    spawnOmniscientMaya(data.string_math.split(',').map(Number));"
"  }).catch(e => { document.getElementById('math_string').innerHTML = '<b style=\"color:red\">Kernel Timeout.</b>'; });"
"}"

"setInterval(() => {"
"  fetch('/api/status?t=' + Date.now()).then(r=>r.json()).then(d=>{"
"    document.getElementById('os_status').innerHTML = `<b>RAM:</b> ${d.ram_locked_bytes} Bytes<br><b>Nodes:</b> ${d.folded_samskaras}<br><b>Status:</b> <span style=\"color:#0f0\">${d.cpu_state}</span>`;"
"  }).catch(e=>{});"
"}, 1000);"
"</script></body></html>";

// --- UTILS ---
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
            if(a>='a') a-='a'-'A'; if(a>='A') a-='A'-10; else a-='0';
            if(b>='a') b-='a'-'A'; if(b>='A') b-='A'-10; else b-='0';
            *dst++ = 16*a+b; src+=3;
        } else if (*src == '+') { *dst++ = ' '; src++; } else { *dst++ = *src++; }
    } *dst++ = '\0';
}
void clean_str(char* str) { char* p=strchr(str,'&'); if(p)*p=0; p=strchr(str,'?'); if(p)*p=0; }

// --- MICRO-THREAD ROUTER ---
void* handle_client(void* arg) {
    int new_socket = *(int*)arg;
    free(arg);
    struct timeval tv; tv.tv_sec = 6; tv.tv_usec = 0; 
    setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    char buffer[4096] = {0}; 
    if(read(new_socket, buffer, 4095) > 0) {
        if(strncmp(buffer, "GET /api/status", 15) == 0) {
            char response[2048];
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nConnection: close\r\nCache-Control: no-store\r\nContent-Type: application/json\r\n\r\n{\"ram_locked_bytes\": 1024, \"folded_samskaras\": %d, \"cpu_state\": \"%s\"}", total_samskaras, kernel_state);
            send(new_socket, response, strlen(response), 0);
        }
        else if(strncmp(buffer, "GET /api/manifest?query=", 24) == 0) {
            char query[256] = {0}; char *m_start = buffer + 24;
            char *http_end = strstr(m_start, " HTTP/1.1"); if(http_end) *http_end = '\0';
            url_decode(query, m_start); clean_str(query); 
            
            char url_query[256]; strcpy(url_query, query);
            for(int i=0; url_query[i]; i++) { if(url_query[i] == ' ') url_query[i] = '_'; }

            char cmd[1024]; 
            snprintf(cmd, sizeof(cmd), "curl -s -m 5 \"https://en.wikipedia.org/api/rest_v1/page/summary/%s\"", url_query);
            FILE *pf = popen(cmd, "r");
            char meaning[4096] = {0};
            if(pf) { fread(meaning, 1, 4095, pf); pclose(pf); }

            char clean_meaning[1024] = "Concept processed via local Quantum Math.";
            char *ext = strstr(meaning, "\"extract\":\"");
            if(ext) { 
                ext += 11; char *end = strchr(ext, '"'); if(end) *end = '\0'; 
                strncpy(clean_meaning, ext, 1000); 
            }

            uint32_t hash1 = 5381;
            for(int i=0; clean_meaning[i] != '\0'; i++) { hash1 = ((hash1 << 5) + hash1) ^ clean_meaning[i]; }

            if(check_SNN_spike((rand() % 100) / 100.0, 0.2)) {
                quantum_fold_knowledge(hash1); // Save to brain.bin
            }

            uint8_t d1 = hash1 & 0xFF; uint8_t d2 = (hash1 >> 8) & 0xFF; uint8_t d3 = (hash1 >> 16) & 0xFF;
            char math_string[256]; snprintf(math_string, sizeof(math_string), "%d,%d,%d", d1, d2, d3);
            
            char safe_meaning[2048] = {0}; int j = 0;
            for(int i = 0; clean_meaning[i] != '\0'; i++) {
                if(clean_meaning[i] == '"' || clean_meaning[i] == '\\' || clean_meaning[i] == '\n') continue;
                safe_meaning[j++] = clean_meaning[i];
            }

            char response[8192]; 
            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nConnection: close\r\nCache-Control: no-store\r\nContent-Type: application/json\r\n\r\n{\"query\": \"%s\", \"string_math\": \"%s\", \"meaning\": \"%s\"}", query, math_string, safe_meaning);
            send(new_socket, response, strlen(response), 0);
        }
        else if(strncmp(buffer, "GET / ", 6) == 0 || strncmp(buffer, "GET /?", 6) == 0) {
            char hdr[512]; snprintf(hdr, sizeof(hdr), "HTTP/1.1 200 OK\r\nConnection: close\r\nCache-Control: no-store\r\nContent-Length: %zu\r\nContent-Type: text/html\r\n\r\n", strlen(MANIFEST_UI));
            send(new_socket, hdr, strlen(hdr), 0); send(new_socket, MANIFEST_UI, strlen(MANIFEST_UI), 0);
        }
        else {
            const char *not_found = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
            send(new_socket, not_found, strlen(not_found), 0);
        }
    }
    close(new_socket); 
    return NULL;
}

int main() {
    printf("\n==================================================\n");
    printf("[PARAM-TATTVA OS v1.0 - SECURE LOCALHOST BUILD]\n");
    printf("==================================================\n");
    load_brain(); 
    printf("Server running securely on 127.0.0.1:%d...\n", PORT);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0), opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr; 
    addr.sin_family = AF_INET; 
    
    // 🔥 THE GOD-LEVEL SECURITY PATCH 🔥
    // Iski wajah se bahar ka koi bhi WiFi ya Hacker OS ko access nahi kar sakta!
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    
    addr.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)); 
    listen(server_fd, 50);
    
    while(1) {
        int new_socket = accept(server_fd, NULL, NULL); 
        if(new_socket < 0) continue;
        int* sock_ptr = malloc(sizeof(int)); *sock_ptr = new_socket;
        pthread_t thread_id; pthread_create(&thread_id, NULL, handle_client, sock_ptr); pthread_detach(thread_id);
    } return 0;
}

