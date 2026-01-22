# Huffman File Compressor

A complete Huffman Encoder/Decoder implemented in C that supports actual file compression using bit-packing and frequency table headers.

## Features
- **Bit-Packing**: Achieves real space savings by packing Huffman codes into bits.
- **Header Management**: Stores frequency tables in the compressed file for self-contained decompression.
- **Stat Generation**: Displays compression ratio and original/compressed sizes.

## Usage
### 1. Simple Build
```bash
gcc -o huffman huffman.c
```

### 2. Compress a file
```bash
./huffman -c sample.txt compressed.huff
```

### 3. Decompress a file
```bash
./huffman -d compressed.huff restored.txt
```
