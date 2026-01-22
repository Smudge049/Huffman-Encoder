#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 256

// A Huffman tree node
struct Node {
    char data;
    unsigned freq;
    struct Node *left, *right;
};

// A Min Heap: Collection of min-heap nodes (or Huffman tree nodes)
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct Node** array;
};

// Function to allocate a new Huffman tree node
struct Node* createNode(char data, unsigned freq) {
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Function to create a min heap of given capacity
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct Node**)malloc(minHeap->capacity * sizeof(struct Node*));
    return minHeap;
}

// Function to swap two min heap nodes
void swapNode(struct Node** a, struct Node** b) {
    struct Node* t = *a;
    *a = *b;
    *b = t;
}

// The standard minHeapify function.
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

// Function to check if size of heap is 1 or not
int isSizeOne(struct MinHeap* minHeap) {
    return (minHeap->size == 1);
}

// Function to extract minimum value node from heap
struct Node* extractMin(struct MinHeap* minHeap) {
    struct Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Function to insert a new node to Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct Node* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

// Function to build min heap
void buildMinHeap(struct MinHeap* minHeap) {
    int n = minHeap->size - 1;
    int i;
    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// Function to check if this node is leaf
int isLeaf(struct Node* root) {
    return !(root->left) && !(root->right);
}

// Create and build min heap
struct MinHeap* createAndBuildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = createNode(data[i], freq[i]);
    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

// The main function that builds Huffman tree
struct Node* buildHuffmanTree(char data[], int freq[], int size) {
    struct Node *left, *right, *top;
    struct MinHeap* minHeap = createAndBuildMinHeap(data, freq, size);

    while (!isSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Store Huffman codes in an array for easy lookup
char* codes[MAX_CHAR];

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
        codes[(unsigned char)root->data] = (char*)malloc(top + 1);
        for (int i = 0; i < top; i++) {
            codes[(unsigned char)root->data][i] = arr[i] + '0';
        }
        codes[(unsigned char)root->data][top] = '\0';
    }
}

void printTable(char data[], int freq[], int size) {
    printf("\n--- Frequency Table ---\n");
    printf("Char | Freq | Code\n");
    printf("--------------------\n");
    for (int i = 0; i < size; i++) {
        printf(" '%c' | %4d | %s\n", data[i], freq[i], codes[(unsigned char)data[i]]);
    }
}

void encode(char* text, char* encodedText) {
    encodedText[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        strcat(encodedText, codes[(unsigned char)text[i]]);
    }
}

void decode(struct Node* root, char* encodedText) {
    struct Node* curr = root;
    printf("\nDecoded String: ");
    for (int i = 0; encodedText[i] != '\0'; i++) {
        if (encodedText[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        if (isLeaf(curr)) {
            printf("%c", curr->data);
            curr = root;
        }
    }
    printf("\n");
}

int main() {
    char text[1000];
    printf("Enter a string to compress: ");
    if (fgets(text, sizeof(text), stdin) == NULL) return 0;
    
    // Remove newline if present
    text[strcspn(text, "\n")] = 0;

    int n = strlen(text);
    if (n == 0) {
        printf("Empty input.\n");
        return 0;
    }

    int freq[MAX_CHAR] = {0};
    for (int i = 0; i < n; i++) {
        freq[(unsigned char)text[i]]++;
    }

    char uniqueData[MAX_CHAR];
    int uniqueFreq[MAX_CHAR];
    int size = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i] > 0) {
            uniqueData[size] = (char)i;
            uniqueFreq[size] = freq[i];
            size++;
        }
    }

    struct Node* root = buildHuffmanTree(uniqueData, uniqueFreq, size);

    int arr[MAX_TREE_HT], top = 0;
    for(int i=0; i<MAX_CHAR; i++) codes[i] = NULL;
    
    generateCodes(root, arr, top);

    printTable(uniqueData, uniqueFreq, size);

    char encodedText[10000] = "";
    encode(text, encodedText);
    printf("\nEncoded (Compressed) Bits: %s\n", encodedText);

    int originalBits = n * 8;
    int compressedBits = strlen(encodedText);
    printf("\n--- Statistics ---\n");
    printf("Original Bits:   %d\n", originalBits);
    printf("Compressed Bits: %d\n", compressedBits);
    printf("Compression Ratio: %.2f%%\n", (1.0 - (float)compressedBits / originalBits) * 100);

    decode(root, encodedText);

    // Cleanup (simplified)
    for(int i=0; i<MAX_CHAR; i++) if(codes[i]) free(codes[i]);

    return 0;
}
