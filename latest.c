#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_CHAR 256

struct Node {
    unsigned char data;
    uint32_t freq;
    struct Node *left, *right;
};

struct MinHeap {
    unsigned size;
    struct Node** array;
};

void trimWhitespace(char *str, size_t *len) {
       while (*len > 0 && isspace((unsigned char)str[*len - 1])) {
       str[*len - 1] = '\0';
       (*len)--;
    }
    
    int start = 0;
       while (start < *len && isspace((unsigned char)str[start])) {
       start++;
    }
       if (start > 0) {
       memmove(str, str + start, *len - start + 1);
       *len -= start;
    }
}

void trimQuotes(char *str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    
       if (len > 0 && str[len - 1] == '\n') {
       str[len - 1] = '\0';
       len--;
    }

    trimWhitespace(str, &len);

       if (len > 0 && str[0] == '&') {
       memmove(str, str + 1, len);
       len--;
       trimWhitespace(str, &len);
    }

       if (len >= 2) {
       if ((str[0] == '"' && str[len - 1] == '"') || (str[0] == '\'' && str[len - 1] == '\'')) {
       memmove(str, str + 1, len - 2);
       str[len - 2] = '\0';
       len -= 2;
       trimWhitespace(str, &len);
       }
    }
}

int confirmAction(const char *message) {
    char choice;
    printf("\n[CONFIRM] %s (Y/N): ", message);
    scanf(" %c", &choice);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return (choice == 'y' || choice == 'Y');
}

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
    minHeap->array = (struct Node**)malloc(capacity * sizeof(struct Node*));
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

void freeTree(struct Node* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

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

unsigned char bitBuffer[8];
int bitIndex = 0;

void writeBit(FILE* out, int bit) {
    bitBuffer[bitIndex++] = bit ? '1' : '0';
       if (bitIndex == 8) {
       unsigned char byte = 0;
       for (int i = 0; i < 8; i++) {
       byte = (byte << 1) | (bitBuffer[i] - '0');
       }
       fputc(byte, out);
       bitIndex = 0;
    }
}

void flushBits(FILE* out) {
       if (bitIndex > 0) {
       unsigned char byte = 0;
       for (int i = 0; i < bitIndex; i++) {
       byte = (byte << 1) | (bitBuffer[i] - '0');
       }
       byte <<= (8 - bitIndex);
       fputc(byte, out);
       bitIndex = 0;
    }
}

int readBit(FILE* in, int *bitPos, unsigned char *currentByte) {
       if (*bitPos == 8) {
       int c = fgetc(in);
       if (c == EOF) return -1;
       *currentByte = (unsigned char)c;
       *bitPos = 0;
    }
    int bit = (*currentByte >> (7 - *bitPos)) & 1;
    (*bitPos)++;
    return bit;
}

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
    int arr[256], top = 0;
    
    for (int i = 0; i < MAX_CHAR; i++) huffmanCodes[i] = NULL;
    
    generateCodes(root, arr, top);

    FILE* out = fopen(outputPath, "wb");
    if (!out) { perror("Error opening output file"); fclose(in); return; }

    fwrite(&totalChars, sizeof(uint32_t), 1, out);

    uint16_t uniqueCount = (uint16_t)size;
    fwrite(&uniqueCount, sizeof(uint16_t), 1, out);
       for (int i = 0; i < MAX_CHAR; i++) {
       if (freq[i] > 0) {
       unsigned char ch = (unsigned char)i;
       fwrite(&ch, 1, 1, out);
       fwrite(&freq[i], sizeof(uint32_t), 1, out);
       }
    }

    rewind(in);
    bitIndex = 0;
       while ((c = fgetc(in)) != EOF) {
       char* code = huffmanCodes[(unsigned char)c];
       for (int i = 0; code[i] != '\0'; i++) {
       writeBit(out, code[i] - '0');
       }
    }
    flushBits(out);

    long inSize = ftell(in);
    fseek(out, 0, SEEK_END);
    long outSize = ftell(out);

    printf("\n=== COMPRESSION COMPLETE ===\n");
    printf("Original Size:   %ld bytes\n", inSize);
    printf("Compressed Size: %ld bytes\n", outSize);
    printf("Compression Ratio: %.2f%%\n", ((double)outSize / inSize) * 100);
    printf("Space Saved:      %.2f%%\n", (1.0 - (double)outSize / inSize) * 100);
    printf("=============================\n");

    fclose(in);
    fclose(out);

       if (outSize > inSize) {
       printf("\n[WARNING] Compressed version is larger than original due to header overhead.");
       if (!confirmAction("Keep this inefficient file anyway?")) {
       if (remove(outputPath) == 0) {
       printf("[DELETED] Inefficient compressed file removed.\n");
       } else {
       perror("[ERROR] Could not remove file");
       }
       } else {
       printf("[KEPT] Efficiency warning acknowledged.\n");
       }
    }
    
    for (int i = 0; i < MAX_CHAR; i++) 
    if (huffmanCodes[i]) 
    free(huffmanCodes[i]);
    
    freeTree(root);
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

    struct Node* curr = root;
    uint32_t decodedCount = 0;
    int bitPos = 8;
    unsigned char currentByte = 0;

       while (decodedCount < totalChars) {
       int bit = readBit(in, &bitPos, &currentByte);
       if (bit == -1) break;

       if (bit == 0) curr = curr->left;
       else curr = curr->right;

       if (isLeaf(curr)) {
       fputc(curr->data, out);
       decodedCount++;
       curr = root;
       }
    }

    printf("=== DECOMPRESSION COMPLETE ===\n");
    printf("Decoded %u characters.\n", decodedCount);
    printf("Expected: %u characters.\n", totalChars);
    if (decodedCount != totalChars)
    printf("WARNING - Character count mismatch!\n");
    printf("===============================\n\n");

    fclose(in);
    fclose(out);
    
    freeTree(root);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char *argv[]) {
       if (argc >= 2) {
       char inputPath[512];
       strcpy(inputPath, argv[1]);
       trimQuotes(inputPath);
       
       char outputPath[512];
       char *lastDot = strrchr(inputPath, '.');
       
       int isHuf = (lastDot && (strcmp(lastDot, ".huf") == 0 || strcmp(lastDot, ".HUF") == 0));

       if (isHuf) {
       sprintf(outputPath, "%s_restored", inputPath);
       
       printf("\n[ACTION] Decompressing: %s\n", inputPath);
       decompressFile(inputPath, outputPath);
       } else {
       sprintf(outputPath, "%s.huf", inputPath);
       
       printf("\n[ACTION] Compressing: %s\n", inputPath);
       compressFile(inputPath, outputPath);
       }
       
       printf("Process completed successfully.\n");
       return 0;
    }

    char mode;
    char inputPath[512];
    char outputPath[512];
    char continueChoice;

       do {
       printf("\033[2J\033[H");
       
       printf("\nHUFFMAN COMPRESSION TOOL\n\n");

       printf("Select operation mode:\n");
       printf("  [C] Compress a file\n");
       printf("  [D] Decompress a file\n");
       printf("  [Q] Quit\n\n");
       printf("Enter your choice: ");
       scanf(" %c", &mode);
       clearInputBuffer();

       if (mode >= 'a' && mode <= 'z') {
       mode = mode - 'a' + 'A';
       }

       if (mode == 'Q') {
       printf("\nThank you for using Huffman Compression Tool!\n");
       break;
       }

       if (mode != 'C' && mode != 'D') {
       printf("\nInvalid choice! Please enter C, D, or Q.\n\n");
       continue;
       }

       printf("\nEnter input file path: ");
       if (fgets(inputPath, sizeof(inputPath), stdin) != NULL) {
       trimQuotes(inputPath);
       }

       printf("Enter output file path: ");
       if (fgets(outputPath, sizeof(outputPath), stdin) != NULL) {
       trimQuotes(outputPath);
       }

       if (strlen(inputPath) == 0) {
       printf("\nError: Input file path cannot be empty!\n\n");
       continue;
       }

       if (strlen(outputPath) == 0) {
       printf("\nError: Output file path cannot be empty!\n\n");
       continue;
       }

       printf("\n");

       if (mode == 'C') {
       printf("Compressing file...\n");
       compressFile(inputPath, outputPath);
       } else if (mode == 'D') {
       printf("Decompressing file...\n");
       decompressFile(inputPath, outputPath);
       }

       printf("Do you want to perform another operation? (Y/N): ");
       scanf(" %c", &continueChoice);
       clearInputBuffer();

       if (continueChoice >= 'a' && continueChoice <= 'z') {
       continueChoice = continueChoice - 'a' + 'A';
       }

    } while (continueChoice == 'Y');

    printf("\nGoodbye!\n");
    return 0;
}