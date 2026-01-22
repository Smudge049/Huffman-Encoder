# 🗜️ Huffman File Compressor

A robust, low-level file compression utility implemented in C. This project leverages the **Huffman Coding** algorithm to provide lossless data compression at the bit-level. It is designed to demonstrate the bridge between theoretical computer science (trees, heaps, greediness) and practical software engineering (file I/O, bitwise manipulation).

---

## 🚀 Key Features

- **Optimal Prefix Coding**: Implements the classic Huffman algorithm to ensure no code is a prefix of another, allowing for unambiguous decoding.
- **True Bit-Packing**: Most tutorial implementations use ASCII '0' and '1' characters (8 bits each) to represent bits. This tool uses a custom **BitWriter** to pack codes into actual single bits, resulting in physical file size reduction.
- **Self-Contained Header**: The compressed `.huff` file includes a serialized frequency table, enabling decompression on any machine without external dependencies.
- **Dynamic Tree Rebuild**: Reconstructs the exact Huffman tree during decompression using the embedded header.
- **Detailed Analytics**: Reports original size, compressed size, and the calculated efficiency/compression ratio.

---

## 📑 How It Works

### The Huffman Algorithm
The project follows a "Greedy" algorithmic approach to construct an optimal code tree:

1.  **Frequency Scan**: Reads the entire input file and counts the occurrences of each unique byte (0-255).
2.  **Node Creation**: Every unique byte becomes a leaf node in a collection.
3.  **Min-Priority Queue**: All leaf nodes are inserted into a **Min-Heap**.
4.  **Tree Construction**:
    - While there’s more than one node in the heap:
        - Extract the two nodes with the lowest frequencies.
        - Create a new internal node with a frequency equal to the sum of the two.
        - Make the two nodes its children and insert it back into the heap.
5.  **Path Traversal**: Traverses the tree from root to leaf to assign codes (Left = `0`, Right = `1`).

### The Technical Stack
- **Languages**: C (Standard C99)
- **Data Structures**: Min-Priority Queue (Binary Heap), Binary Tree, Hash Map (implemented as a frequency array for O(1) byte-access).
- **Core logic**: Recursive Depth-First Search (DFS) for code generation.

---

## 📂 Performance Expectations

| Content Type | Redundancy Level | Est. Compression Ratio |
| :--- | :--- | :--- |
| **English Text** | High | 30% - 50% |
| **Log Files** | Very High | 50% - 70% |
| **Uncompressed Images (.bmp)** | High | 40% - 60% |
| **Compressed Files (.zip, .jpg)** | Low | < 5% (or size increase) |

> [!NOTE]
> **Why do compressed files fail?**
> Files like `.zip` are already at the limit of entropy. Attempting to compress them again is like trying to dry water; there's no air left in the file to "squeeze out."

---

## 🛠️ Installation & Usage

### Prerequisites
- A C compiler (GCC, Clang, or MSVC).
- Git (optional, for repository management).

### Execution
1.  **Build**:
    ```bash
    gcc -o huffman huffman.c
    ```
2.  **Compress**:
    ```bash
    ./huffman -c my_document.txt compressed.huff
    ```
3.  **Decompress**:
    ```bash
    ./huffman -d compressed.huff restored_document.txt
    ```

---

## 🔍 Internal Structure

### Binary Header Format
The serialized header is crucial for decompression. It follows this structure:
- `Total Characters` (4 bytes): The original byte count.
- `Unique Entries` (2 bytes): Number of active characters in the frequency table.
- `Frequency Table`: A sequence of pairs `[Byte (1 byte)][Frequency (4 bytes)]`.

### Memory Management
The program carefully manages memory by using `malloc` for the Huffman tree and `free` to prevent leaks during the code generation and cleanup phases.

---

## 🤝 Contributing
Feel free to fork this project and submit pull requests. Future improvements could include:
- Multi-byte (Word-based) Huffman coding.
- Canonical Huffman codes for even smaller headers.
- Multi-threaded frequency analysis.
