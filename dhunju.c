#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <ctype.h>

#define MAX_CHAR 256

// Huffman Tree Node
typedef struct Node {
    unsigned char data;
    uint32_t freq;
    struct Node *left, *right;
} Node;

// Min Heap for priority queue
typedef struct {
    unsigned size;
    Node** array;
} MinHeap;

char* codes[MAX_CHAR];

// --- Utilities ---
void trim(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) str[--len] = '\0';
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    len = strlen(str);
    if (len >= 2 && ((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\'' && str[len - 1] == '\''))) {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

int is_dir(const char *path) {
    struct stat s;
    return (stat(path, &s) == 0 && (s.st_mode & S_IFDIR));
}

int confirm(const char *msg) {
    char c;
    printf("\n[CONFIRM] %s (Y/N): ", msg);
    scanf(" %c", &c);
    while (getchar() != '\n'); 
    return (toupper(c) == 'Y');
}

// --- Min Heap logic ---
Node* createNode(unsigned char d, uint32_t f) {
    Node* n = (Node*)calloc(1, sizeof(Node));
    n->data = d; n->freq = f;
    return n;
}

void heapify(MinHeap* h, int i) {
    int s = i, L = 2 * i + 1, R = 2 * i + 2;
    if (L < h->size && h->array[L]->freq < h->array[s]->freq) s = L;
    if (R < h->size && h->array[R]->freq < h->array[s]->freq) s = R;
    if (s != i) {
        Node* t = h->array[i]; h->array[i] = h->array[s]; h->array[s] = t;
        heapify(h, s);
    }
}

Node* extract(MinHeap* h) {
    Node* t = h->array[0];
    h->array[0] = h->array[--h->size];
    heapify(h, 0);
    return t;
}

void insert(MinHeap* h, Node* n) {
    int i = h->size++;
    while (i && n->freq < h->array[(i - 1) / 2]->freq) {
        h->array[i] = h->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    h->array[i] = n;
}

// --- Huffman Logic ---
Node* buildTree(uint32_t f[]) {
    MinHeap h = {0, (Node**)malloc(MAX_CHAR * sizeof(Node*))};
    for (int i = 0; i < MAX_CHAR; i++)
        if (f[i]) insert(&h, createNode(i, f[i]));

    while (h.size > 1) {
        Node *L = extract(&h), *R = extract(&h);
        Node *p = createNode('$', L->freq + R->freq);
        p->left = L; p->right = R;
        insert(&h, p);
    }
    Node* root = extract(&h);
    free(h.array);
    return root;
}

void gen(Node* r, int a[], int t) {
    if (r->left) { a[t] = 0; gen(r->left, a, t + 1); a[t] = 1; gen(r->right, a, t + 1); }
    else {
        codes[r->data] = (char*)malloc(t + 1);
        for (int i = 0; i < t; i++) codes[r->data][i] = a[i] + '0';
        codes[r->data][t] = '\0';
    }
}

void freeTree(Node* r) { if (r) { freeTree(r->left); freeTree(r->right); free(r); } }

// --- File Handling ---
void compress(const char* inP, const char* outP) {
    if (is_dir(inP)) { printf("Error: Input is a directory.\n"); return; }
    FILE *in = fopen(inP, "rb");
    if (!in) { perror("Input error"); return; }

    uint32_t f[MAX_CHAR] = {0}, total = 0;
    int c; while ((c = fgetc(in)) != EOF) { f[(unsigned char)c]++; total++; }
    if (total == 0) { printf("File is empty.\n"); fclose(in); return; }

    Node* root = buildTree(f);
    int bitA[MAX_CHAR];
    for (int i = 0; i < MAX_CHAR; i++) codes[i] = NULL;
    gen(root, bitA, 0);

    FILE *out = fopen(outP, "wb");
    if (!out) { perror("Output error"); fclose(in); freeTree(root); return; }

    fwrite("DHJ1", 1, 4, out); // Magic Number
    fwrite(&total, 4, 1, out);
    uint16_t u = 0; for (int i = 0; i < MAX_CHAR; i++) if (f[i]) u++;
    fwrite(&u, 2, 1, out);
    for (int i = 0; i < MAX_CHAR; i++) if (f[i]) {
        unsigned char ch = i; fwrite(&ch, 1, 1, out); fwrite(&f[i], 4, 1, out);
    }

    rewind(in);
    uint8_t buf = 0; int b = 0;
    while ((c = fgetc(in)) != EOF) {
        char *s = codes[(unsigned char)c];
        for (int i = 0; s[i]; i++) {
            if (s[i] == '1') buf |= (1 << (7 - b));
            if (++b == 8) { fputc(buf, out); buf = 0; b = 0; }
        }
    }
    if (b > 0) fputc(buf, out);

    long inS = ftell(in); fseek(out, 0, SEEK_END); long outS = ftell(out);
    printf("\n=== STATISTICS ===\nOriginal: %ld bytes\nCompressed: %ld bytes\nRatio: %.2f%%\n", 
           inS, outS, ((double)outS/inS)*100);

    fclose(in); fclose(out); freeTree(root);
    for (int i = 0; i < MAX_CHAR; i++) if (codes[i]) free(codes[i]);

    if (outS > inS) {
        printf("[WARNING] Output is larger than input.");
        if (!confirm("Keep the compressed file anyway?")) {
            remove(outP); printf("File deleted.\n");
        }
    }
}

void decompress(const char* inP, const char* outP) {
    FILE *in = fopen(inP, "rb");
    if (!in) { perror("Input error"); return; }
    char sig[4]; if (fread(sig, 1, 4, in) != 4 || memcmp(sig, "DHJ1", 4) != 0) {
        printf("Error: Not a valid Huffman file.\n"); fclose(in); return;
    }

    uint32_t total; uint16_t unique;
    fread(&total, 4, 1, in); fread(&unique, 2, 1, in);
    uint32_t f[MAX_CHAR] = {0};
    for (int i = 0; i < unique; i++) {
        unsigned char ch; fread(&ch, 1, 1, in); fread(&f[ch], 4, 1, in);
    }

    Node* root = buildTree(f), *curr = root;
    FILE *out = fopen(outP, "wb");
    uint32_t count = 0; uint8_t buf; int b = 8;
    while (count < total) {
        if (b == 8) { buf = fgetc(in); b = 0; }
        curr = (buf & (1 << (7 - b++))) ? curr->right : curr->left;
        if (!curr->left && !curr->right) { fputc(curr->data, out); count++; curr = root; }
    }
    printf("Decompression successful. Restored %u bytes.\n", count);
    fclose(in); fclose(out); freeTree(root);
}

// --- Main ---
int main(int argc, char *argv[]) {
    if (argc >= 2) {
        char inP[512], outP[512]; strcpy(inP, argv[1]); trim(inP);
        FILE *peek = fopen(inP, "rb"); int isC = 0;
        if (peek) { char s[4]; if (fread(s, 1, 4, peek) == 4 && !memcmp(s, "DHJ1", 4)) isC = 1; fclose(peek); }

        if (isC) { 
            strcpy(outP, inP); char *p = strrchr(outP, '.'); if (p) *p = '\0';
            strcat(outP, "_restored.png"); decompress(inP, outP); 
        } else { sprintf(outP, "%s.huf", inP); compress(inP, outP); }
        return 0;
    }

    char mode, inP[512], outP[512];
    do {
        printf("\033[2J\033[H");
        printf("ΓòöΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòù\n");
        printf("Γòæ   HUFFMAN ENCODER (BALANCED VERSION)   Γòæ\n");
        printf("ΓòÜΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓò¥\n\n");
        printf("Select Mode:\n  [C] Compress\n  [D] Decompress\n  [Q] Quit\n\nEnter choice: ");
        scanf(" %c", &mode); while (getchar() != '\n');
        mode = toupper(mode);

        if (mode == 'Q') break;
        if (mode != 'C' && mode != 'D') continue;

        printf("Input File path: "); fgets(inP, 512, stdin); trim(inP);
        printf("Output File path: "); fgets(outP, 512, stdin); trim(outP);

        if (strlen(inP) == 0 || strlen(outP) == 0) { printf("Error: Paths cannot be empty.\n"); }
        else if (mode == 'C') compress(inP, outP);
        else decompress(inP, outP);

        printf("\nPress Enter to continue..."); getchar();
    } while (1);

    return 0;
}
