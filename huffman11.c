#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_TREE_HT 256
#define MAX_CHAR 256

// A Huffman tree node
struct Node {
    unsigned char data;
    uint32_t freq;
    struct Node *left, *right;
};

// A Min Heap: Collection of min-heap nodes
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct Node** array;
};

// --- Min Heap Functions ---

struct Node* createNode(unsigned char data, uint32_t freq) {
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct Node**)malloc(minHeap->capacity * sizeof(struct Node*));
    return minHeap;
}

void swapNode(struct Node** a, struct Node** b) {
    struct Node* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

struct Node* extractMin(struct MinHeap* minHeap) {
    struct Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(struct MinHeap* minHeap, struct Node* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

int isLeaf(struct Node* root) {
    return !(root->left) && !(root->right);
}

struct Node* buildHuffmanTree(unsigned char data[], uint32_t freq[], int size) {
    struct Node *left, *right, *top;
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }
    struct Node* root = extractMin(minHeap);
    free(minHeap->array);
    free(minHeap);
    return root;
}

// --- Code Generation ---

char* huffmanCodes[MAX_CHAR];

void generateCodes(struct Node* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1);
    }
    if (isLeaf(root)) {
        huffmanCodes[root->data] = (char*)malloc(top + 1);
        for (int i = 0; i < top; i++) {
            huffmanCodes[root->data][i] = arr[i] + '0';
        }
        huffmanCodes[root->data][top] = '\0';
    }
}

// --- Bit Manipulation Buffers ---

typedef struct {
    uint8_t buffer;
    int bitCount;
    FILE* file;
} BitWriter;

void initBitWriter(BitWriter* bw, FILE* f) {
    bw->buffer = 0;
    bw->bitCount = 0;
    bw->file = f;
}

void writeBit(BitWriter* bw, int bit) {
    if (bit) bw->buffer |= (1 << (7 - bw->bitCount));
    bw->bitCount++;
    if (bw->bitCount == 8) {
        fputc(bw->buffer, bw->file);
        bw->buffer = 0;
        bw->bitCount = 0;
    }
}

void flushBitWriter(BitWriter* bw) {
    if (bw->bitCount > 0) {
        fputc(bw->buffer, bw->file);
    }
}

typedef struct {
    uint8_t buffer;
    int bitCount;
    FILE* file;
} BitReader;

void initBitReader(BitReader* br, FILE* f) {
    br->buffer = 0;
    br->bitCount = 8; // Force read on first call
    br->file = f;
}

int readBit(BitReader* br) {
    if (br->bitCount == 8) {
        int c = fgetc(br->file);
        if (c == EOF) return -1;
        br->buffer = (uint8_t)c;
        br->bitCount = 0;
    }
    int bit = (br->buffer >> (7 - br->bitCount)) & 1;
    br->bitCount++;
    return bit;
}

// --- File Operations ---

void compressFile(const char* inputPath, const char* outputPath) {
    FILE* in = fopen(inputPath, "rb");
    if (!in) { perror("Error opening input file"); return; }

    uint32_t freq[MAX_CHAR] = {0};
    int c;
    uint32_t totalChars = 0;
    while ((c = fgetc(in)) != EOF) {
        freq[(unsigned char)c]++;
        totalChars++;
    }

    if (totalChars == 0) {
        printf("File is empty.\n");
        fclose(in);
        return;
    }

    unsigned char uniqueChars[MAX_CHAR];
    uint32_t uniqueFreqs[MAX_CHAR];
    int size = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i] > 0) {
            uniqueChars[size] = (unsigned char)i;
            uniqueFreqs[size] = freq[i];
            size++;
        }
    }

    struct Node* root = buildHuffmanTree(uniqueChars, uniqueFreqs, size);
    int arr[MAX_TREE_HT], top = 0;
    for (int i = 0; i < MAX_CHAR; i++) huffmanCodes[i] = NULL;
    generateCodes(root, arr, top);

    FILE* out = fopen(outputPath, "wb");
    if (!out) { perror("Error opening output file"); fclose(in); return; }

    // Write Header
    // 1. Total Characters (4 bytes)
    fwrite(&totalChars, sizeof(uint32_t), 1, out);
    // 2. Number of unique entries (2 bytes)
    uint16_t uniqueCount = (uint16_t)size;
    fwrite(&uniqueCount, sizeof(uint16_t), 1, out);
    // 3. Frequency table
    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i] > 0) {
            unsigned char ch = (unsigned char)i;
            fwrite(&ch, 1, 1, out);
            fwrite(&freq[i], sizeof(uint32_t), 1, out);
        }
    }

    // Write Data
    rewind(in);
    BitWriter bw;
    initBitWriter(&bw, out);
    while ((c = fgetc(in)) != EOF) {
        char* code = huffmanCodes[(unsigned char)c];
        for (int i = 0; code[i] != '\0'; i++) {
            writeBit(&bw, code[i] - '0');
        }
    }
    flushBitWriter(&bw);

    long inSize = ftell(in);
    fseek(out, 0, SEEK_END);
    long outSize = ftell(out);

    printf("Compression Complete!\n");
    
    printf("Original Size:   %ld bytes\n", inSize);
    printf("Compressed Size: %ld bytes\n", outSize);
    printf("Efficiency:      %.2f%%\n", (1.0 - (double)outSize / inSize) * 100);
  

    fclose(in);
    fclose(out);
    for (int i = 0; i < MAX_CHAR; i++) if (huffmanCodes[i]) free(huffmanCodes[i]);
}

void decompressFile(const char* inputPath, const char* outputPath) {
    FILE* in = fopen(inputPath, "rb");
    if (!in) { perror("Error opening compressed file"); return; }

    uint32_t totalChars;
    if (fread(&totalChars, sizeof(uint32_t), 1, in) != 1) {
        printf("Error reading header (total chars).\n");
        fclose(in);
        return;
    }

    uint16_t uniqueCount;
    if (fread(&uniqueCount, sizeof(uint16_t), 1, in) != 1) {
        printf("Error reading header (unique count).\n");
        fclose(in);
        return;
    }

    unsigned char uniqueChars[MAX_CHAR];
    uint32_t uniqueFreqs[MAX_CHAR];
    for (int i = 0; i < uniqueCount; i++) {
        fread(&uniqueChars[i], 1, 1, in);
        fread(&uniqueFreqs[i], sizeof(uint32_t), 1, in);
    }

    struct Node* root = buildHuffmanTree(uniqueChars, uniqueFreqs, uniqueCount);

    FILE* out = fopen(outputPath, "wb");
    if (!out) { perror("Error opening output file"); fclose(in); return; }

    BitReader br;
    initBitReader(&br, in);
    struct Node* curr = root;
    uint32_t decodedCount = 0;

    while (decodedCount < totalChars) {
        int bit = readBit(&br);
        if (bit == -1) break;

        if (bit == 0) curr = curr->left;
        else curr = curr->right;

        if (isLeaf(curr)) {
            fputc(curr->data, out);
            decodedCount++;
            curr = root;
        }
    }

    printf("Decompression Complete!\n");
   
    printf("Decoded %u characters.\n", decodedCount);
  

    fclose(in);
    fclose(out);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    char mode;
    char inputPath[512];
    char outputPath[512];
    char continueChoice;

    do {
        // Clear screen (optional, works on most terminals)
        printf("\033[2J\033[H");
        
        printf("   HUFFMAN COMPRESSION TOOL\n");
    

        // Get mode
        printf("Select operation mode:\n");
        printf("  C- Compress a file\n");
        printf("  D - Decompress a file\n");
        printf("  Q - Quit\n\n");
        printf("Enter your choice: ");
        scanf(" %c", &mode);
        clearInputBuffer();

        // Convert to uppercase for easier comparison
        if (mode >= 'a' && mode <= 'z') {
            mode = mode - 'a' + 'A';
        }

        if (mode == 'Q') {
            printf("\nThank you for using Huffman Compression Tool!\n");
            break;
        }

        if (mode != 'C' && mode != 'D') {
            printf("\nInvalid choice! Please enter C, D, or Q.\n");
            printf("\nPress Enter to continue...");
            getchar();
            continue;
        }

        // Get input file path
        printf("\nEnter input file path: ");
        if (fgets(inputPath, sizeof(inputPath), stdin) != NULL) {
            // Remove trailing newline
            size_t len = strlen(inputPath);
            if (len > 0 && inputPath[len-1] == '\n') {
                inputPath[len-1] = '\0';
            }
        }

        // Get output file path
        printf("Enter output file path: ");
        if (fgets(outputPath, sizeof(outputPath), stdin) != NULL) {
            // Remove trailing newline
            size_t len = strlen(outputPath);
            if (len > 0 && outputPath[len-1] == '\n') {
                outputPath[len-1] = '\0';
            }
        }

        printf("\n");

        // Perform operation
        if (mode == 'C') {
            printf("Compressing file...\n");
            compressFile(inputPath, outputPath);
        } else if (mode == 'D') {
            printf("Decompressing file...\n");
            decompressFile(inputPath, outputPath);
        }

        // Ask if user wants to continue
        printf("\nDo you want to perform another operation? (Y/N): ");
        scanf(" %c", &continueChoice);
        clearInputBuffer();

        if (continueChoice >= 'a' && continueChoice <= 'z') {
            continueChoice = continueChoice - 'a' + 'A';
        }

    } while (continueChoice == 'Y');

    printf("\nOK\n");
    return 0;
}