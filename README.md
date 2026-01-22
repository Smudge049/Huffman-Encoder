# Huffman File Compressor

A high-performance Huffman Encoder/Decoder implemented in C. This tool bridges the gap between theoretical data structures and practical utility by providing real bit-level file compression.

## 🚀 Features
- **True Bit-Packing**: Unlike basic implementations that store '0' and '1' as characters, this tool uses bitwise operators to pack data into individual bits, achieving actual space savings.
- **Header Management**: Automatically embeds a frequency table header in every `.huff` file for self-contained decompression.
- **Binary Support**: Treats files as 8-bit byte streams, allowing it to process text, source code, and uncompressed binary files.
- **Efficiency Metrics**: Displays original vs. compressed size and the compression ratio upon completion.

## 📂 What can it compress?
Huffman coding is a lossless compression algorithm most effective for files with high redundancy:
- **Best Results**: Plain text (`.txt`), Source code (`.c`, `.html`), and uncompressed images (`.bmp`).
- **Limited Results**: Already compressed files like `.jpg`, `.png`, `.mp3`, or `.zip` (these may even increase in size due to header overhead).

## 🛠️ Usage

### 1. Build the Tool
Compile using GCC or any standard C compiler:
```bash
gcc -o huffman huffman.c
```

### 2. Compress a File
```bash
./huffman -c input.txt output.huff
```

### 3. Decompress a File
```bash
./huffman -d output.huff restored.txt
```

## 🧠 How it Works
1. **Frequency Analysis**: Scans the input file to count the occurrences of each byte (0-255).
2. **Min-Heap & Huffman Tree**: Uses a priority queue to build an optimal prefix tree based on frequencies.
3. **Code Generation**: Traverses the tree to assign short binary codes to frequent bytes and long codes to rare ones.
4. **Bit-Streaming**: Translates characters into codes and writes them to the disk using a custom bit-buffer.
