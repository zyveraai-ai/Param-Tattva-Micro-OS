
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <stdint.h>

#include <unistd.h>

#include <pthread.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <time.h>



#define PORT 8108

#define RAM_LIMIT 1024

#define NEURON_BITS 2048

#define U64_BLOCKS  (NEURON_BITS / 64)

#define MAX_SUTRAS  256

#define SMRITI_FILE "smriti.bin"



#define TAMAS  0

#define SATTVA 1

#define RAJAS  2



typedef struct {

    uint64_t blocks[U64_BLOCKS];

    uint16_t active_count;

} BinduSDR_t;



typedef struct {

    BinduSDR_t sdr;

    char       category[16];

    char       key[128];

    char       reply[384];

    uint32_t   karma_hits;

} SutraNode_t;



typedef struct {

    SutraNode_t vault[MAX_SUTRAS];

    uint16_t    total_sutras;

    uint32_t    spanda_spikes;

    uint32_t    zero_heat_hits;

    uint16_t    prana_level;

    int         triguna;

} ParamTattvaBrain_t;



ParamTattvaBrain_t brain;

pthread_mutex_t core_lock = PTHREAD_MUTEX_INITIALIZER;



int is_stop_word(const char* w) {

    static const char* stops[] = {

        "what","is","the","a","an","of","in","to","for","how","why",

        "kya","hai","kaise","ka","ki","ke","ko","mein","me","batao","bhai",NULL

    };

    for (int i = 0; stops[i]; i++) if (strcmp(w, stops[i]) == 0) return 1;

    return 0;

}



uint32_t hash_stem(const char* word, int max_len) {

    uint32_t h = 5381;

    for (int i = 0; word[i] && i < max_len; i++) {

        char c = word[i];

        if (c >= 'A' && c <= 'Z') c += 32;

        h = ((h << 5) + h) ^ (uint8_t)c;

    }

    return h;

}



static inline void set_neuron_bit(BinduSDR_t* sdr, uint16_t bit_pos) {

    uint16_t blk = (bit_pos % NEURON_BITS) >> 6;

    uint16_t off = (bit_pos % NEURON_BITS) & 63;

    uint64_t mask = (1ULL << off);

    if (!(sdr->blocks[blk] & mask)) {

        sdr->blocks[blk] |= mask;

        sdr->active_count++;

    }

}



void bindu_encode(const char* text, BinduSDR_t* out_sdr) {

    memset(out_sdr, 0, sizeof(BinduSDR_t));

    if (!text || !*text) return;



    char clean[512] = {0};

    int idx = 0;

    for (int i = 0; text[i] && idx < 510; i++) {

        char c = text[i];

        if (c >= 'A' && c <= 'Z') c += 32;

        clean[idx++] = ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) ? c : ' ';

    }



    char *saveptr;

    char *word = strtok_r(clean, " ", &saveptr);

    while (word) {

        int len = strlen(word);

        if (len >= 2 && !is_stop_word(word)) {

            uint32_t wh = hash_stem(word, len);

            for (int k = 0; k < 4; k++) {

                set_neuron_bit(out_sdr, (uint16_t)((wh ^ (k * 2654435761UL)) % NEURON_BITS));

            }

            if (len >= 4) {

                uint32_t sh = hash_stem(word, 4);

                set_neuron_bit(out_sdr, (uint16_t)(sh % NEURON_BITS));

            }

            for (int t = 0; t <= len - 3; t++) {

                set_neuron_bit(out_sdr, (uint16_t)(hash_stem(word + t, 3) % NEURON_BITS));

            }

        }

        word = strtok_r(NULL, " ", &saveptr);

    }

}



static inline int bindu_overlap(const BinduSDR_t* a, const BinduSDR_t* b) {

    int matched = 0;

    for (int i = 0; i < U64_BLOCKS; i++) {

        matched += __builtin_popcountll(a->blocks[i] & b->blocks[i]);

    }

    return matched;

}



void save_smriti_disk() {

    FILE *f = fopen(SMRITI_FILE, "wb");

    if (f) { fwrite(&brain, sizeof(ParamTattvaBrain_t), 1, f); fclose(f); }

}



void smriti_add_sutra(const char* cat, const char* key, const char* reply) {

    if (brain.total_sutras >= MAX_SUTRAS) return;

    SutraNode_t* n = &brain.vault[brain.total_sutras++];

    char combined[512] = {0};

    snprintf(combined, sizeof(combined), "%s %s", key, reply);

    bindu_encode(combined, &n->sdr);

    strncpy(n->category, cat, sizeof(n->category) - 1);

    strncpy(n->key, key, sizeof(n->key) - 1);

    strncpy(n->reply, reply, sizeof(n->reply) - 1);

    n->karma_hits = 1;

}



void init_brain() {

    pthread_mutex_lock(&core_lock);

    FILE *f = fopen(SMRITI_FILE, "rb");

    if (f && fread(&brain, sizeof(ParamTattvaBrain_t), 1, f) == 1) {

        fclose(f);

    } else {

        if (f) fclose(f);

        memset(&brain, 0, sizeof(ParamTattvaBrain_t));

        brain.prana_level = 500;

        brain.triguna = SATTVA;



        smriti_add_sutra("SANATAN_AI", "param tattva bindu spanda triguna",

            "Param-Tattva 2048-bit Bindu SDR aur __builtin_popcountll par chalta hai, jisse 98% neurons shant rehte hain aur zero heat banti hai.");

        smriti_add_sutra("PHYSICS", "black hole singularity gravity",

            "Black hole space ka woh Bindu (Singularity) hai jahan gravity itni tez hoti hai ki light bhi bahar nahi nikal sakti.");

        smriti_add_sutra("ZYVERA_TAX", "gst tax laptop electronics 18 percent",

            "Electronics aur Laptop par 18% GST lagta hai (9% CGST + 9% SGST).");

        smriti_add_sutra("ZYVERA_AUDIT", "staff fraud chori bill edit kaccha chitha",

            "Z-Sheet mein agar koi staff purana bill amount kam karta hai, toh Kaccha Chitha Audit lock ho jata hai aur 2 second mein Boss ko WhatsApp Siren jata hai.");

        smriti_add_sutra("FOUNDER", "zyvera founder ujjwal prayagraj creator",

            "Zyvera aur Param-Tattva ke creator Ujjwal hain jo Prayagraj se is Next-Gen Edge AGI ko build kar rahe hain.");

        save_smriti_disk();

    }

    pthread_mutex_unlock(&core_lock);

}



void classify_task(const char* q, char* out_task) {

    char buf[512] = {0};

    strncpy(buf, q, sizeof(buf) - 1);

    for (int i = 0; buf[i]; i++) if (buf[i] >= 'A' && buf[i] <= 'Z') buf[i] += 32;



    if (strstr(buf, "paid") || strstr(buf, "udhaar") || strstr(buf, "pending") || strstr(buf, "jama") || strstr(buf, "bill"))

        strcpy(out_task, "LEDGER");

    else if (strstr(buf, "draw") || strstr(buf, "image") || strstr(buf, "photo") || strstr(buf, "picture"))

        strcpy(out_task, "VISION");

    else if (strstr(buf, "build") || strstr(buf, "code") || strstr(buf, "python") || strstr(buf, "script") || strstr(buf, "create"))

        strcpy(out_task, "CODE");

    else

        strcpy(out_task, "GENERAL");

}



void url_decode(char* str) {

    char *src = str, *dst = str;

    while (*src) {

        if (*src == '+') { *dst++ = ' '; src++; }

        else if (*src == '%' && src[1] && src[2]) {

            char hex[3] = { src[1], src[2], 0 };

            *dst++ = (char)strtol(hex, NULL, 16);

            src += 3;

        } else { *dst++ = *src++; }

    }

    *dst = '\0';

}



void send_json(int sock, const char* body) {

    char resp[1536];

    snprintf(resp, sizeof(resp),

        "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n%s", body);

    send(sock, resp, strlen(resp), 0);

}



int main() {

    init_brain();

    int server_fd, sock; struct sockaddr_in addr; int opt = 1; socklen_t alen = sizeof(addr);

    char buffer[2048];



    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET; addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) return 1;

    listen(server_fd, 5);



    while (1) {

        if ((sock = accept(server_fd, (struct sockaddr *)&addr, &alen)) < 0) continue;

        memset(buffer, 0, sizeof(buffer));

        ssize_t n = read(sock, buffer, sizeof(buffer) - 1);

        if (n <= 0) { close(sock); continue; }



        char json_out[1024] = {0};



        if (strstr(buffer, "GET /api/orchestrate") || strstr(buffer, "GET /api/manifest")) {

            char *q_ptr = strstr(buffer, "query=");

            char query[512] = {0};

            if (q_ptr) {

                strncpy(query, q_ptr + 6, sizeof(query) - 1);

                char *end = strpbrk(query, " &\r\n");

                if (end) *end = '\0';

                url_decode(query);

            }



            pthread_mutex_lock(&core_lock);

            BinduSDR_t q_sdr;

            bindu_encode(query, &q_sdr);

            uint32_t q_hash = hash_stem(query, strlen(query));



            int best_idx = -1, max_ov = 0;

            for (int i = 0; i < brain.total_sutras; i++) {

                int ov = bindu_overlap(&q_sdr, &brain.vault[i].sdr);

                if (ov > max_ov) { max_ov = ov; best_idx = i; }

            }



            char task_type[16] = "GENERAL";

            classify_task(query, task_type);



            if (best_idx >= 0 && max_ov >= 4 && strcmp(task_type, "CODE") != 0 && strcmp(task_type, "VISION") != 0) {

                brain.vault[best_idx].karma_hits++;

                brain.zero_heat_hits++;

                brain.prana_level = (brain.prana_level > 10) ? brain.prana_level - 5 : 0;

                brain.triguna = (brain.prana_level > 650) ? RAJAS : (brain.prana_level > 250) ? SATTVA : TAMAS;



                snprintf(json_out, sizeof(json_out),

                    "{\"status\":\"success\",\"cached_hit\":1,\"task\":\"%s\",\"hash\":%u,\"overlap\":%d,\"spike\":0,\"guna\":%d,\"smriti_reply\":\"%s\"}",

                    task_type, q_hash, max_ov, brain.triguna, brain.vault[best_idx].reply);

            } else {

                brain.spanda_spikes++;

                brain.prana_level = (brain.prana_level < 970) ? brain.prana_level + 25 : 1000;

                brain.triguna = (brain.prana_level > 650) ? RAJAS : (brain.prana_level > 250) ? SATTVA : TAMAS;



                snprintf(json_out, sizeof(json_out),

                    "{\"status\":\"success\",\"cached_hit\":0,\"task\":\"%s\",\"hash\":%u,\"overlap\":%d,\"spike\":1,\"guna\":%d,\"smriti_reply\":\"\"}",

                    task_type, q_hash, max_ov, brain.triguna);

            }

            pthread_mutex_unlock(&core_lock);

            send_json(sock, json_out);

        }

        else if (strstr(buffer, "GET /api/learn")) {

            char *k_ptr = strstr(buffer, "key=");

            char *a_ptr = strstr(buffer, "ans=");

            if (k_ptr && a_ptr) {

                char key_buf[128] = {0}, ans_buf[384] = {0};

                strncpy(key_buf, k_ptr + 4, sizeof(key_buf) - 1);

                char *ke = strpbrk(key_buf, "& \r\n"); if (ke) *ke = '\0';

                strncpy(ans_buf, a_ptr + 4, sizeof(ans_buf) - 1);

                char *ae = strpbrk(ans_buf, "& \r\n"); if (ae) *ae = '\0';

                url_decode(key_buf);

                url_decode(ans_buf);

                for (int i = 0; ans_buf[i]; i++) if (ans_buf[i] == '"' || ans_buf[i] == '\n' || ans_buf[i] == '\r') ans_buf[i] = ' ';



                pthread_mutex_lock(&core_lock);

                smriti_add_sutra("GURUKUL", key_buf, ans_buf);

                save_smriti_disk();

                pthread_mutex_unlock(&core_lock);

                snprintf(json_out, sizeof(json_out), "{\"status\":\"learned\",\"total_sutras\":%u}", brain.total_sutras);

            } else {

                snprintf(json_out, sizeof(json_out), "{\"error\":\"Missing key or ans\"}");

            }

            send_json(sock, json_out);

        }

        else if (strstr(buffer, "GET /api/state")) {

            pthread_mutex_lock(&core_lock);

            snprintf(json_out, sizeof(json_out),

                "{\"os\":\"Param-Tattva v3.5\",\"ram\":%d,\"entropy\":%u,\"sutras\":%u,\"zero_heat_hits\":%u,\"spikes\":%u,\"guna\":%d,\"prana\":%u}",

                RAM_LIMIT, brain.spanda_spikes * 5, brain.total_sutras, brain.zero_heat_hits, brain.spanda_spikes, brain.triguna, brain.prana_level);

            pthread_mutex_unlock(&core_lock);

            send_json(sock, json_out);

        }

        else {

            send_json(sock, "{\"status\":\"online\"}");

        }

        close(sock);

    }

    return 0;

}

