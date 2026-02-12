# 🗜️ Huffman File Compressor (`dhunju`)

A robust, low-level file compression utility implemented in C. This project leverages the **Huffman Coding** algorithm to provide lossless data compression at the bit-level. It is designed to be highly user-friendly with automated mode detection and path handling.

---

## 🚀 Key Features

- **Smart Auto-Detection**: Drag and drop any file onto the `.exe` to automatically compress it. If the file is already a `.huf` archive, it automatically decompresses it.
- **One-Click Decompression**: Associate `.huf` extensions with `dhunju.exe` in Windows for instant "double-click" decompression.
- **Windows Path Polish**: Automatically handles double quotes (e.g., `"C:\path\to\file.txt"`) copied from Windows "Copy as Path", making manual path entry seamless.
- **True Bit-Packing**: Uses a custom **BitWriter** to pack codes into actual single bits, resulting in physical file size reduction.
- **Self-Contained Header**: Compressed `.huf` files include a serialized frequency table, allowing for decompression anywhere.
- **Detailed Analytics**: Reports compression ratios, space saved, and character frequency distributions.

---

## 📑 How It Works

### The Huffman Algorithm
The project follows a greedy approach to construct an optimal code tree:
1.  **Frequency Scan**: Counts occurrences of each unique byte (0-255).
2.  **Min-Heap & Tree**: Constructs a binary tree where more frequent characters get shorter bit-codes.
3.  **Bit Manipulation**: Codes are packed into bytes using bitwise operations to ensure minimum storage.

### The Technical Stack
- **Languages**: C (Standard C99)
- **Data Structures**: Min-Priority Queue (Binary Heap), Binary Tree, Hash Map (Frequency Array).

---

## �️ Usage

### 1. The "Easy" Way (Automatic)
Simply drag your file onto `dhunju.exe`.
- **Input**: `data.txt` → **Output**: `data.txt.huf` (Compressed)
- **Input**: `data.txt.huf` → **Output**: `data_decompressed.txt` (Decompressed)

### 2. The Interactive Way
Run `dhunju.exe` directly to enter the menu-driven interface:
```bash
Select operation mode:
  [C] Compress a file
  [D] Decompress a file
  [Q] Quit

Enter your choice: C
Enter input file path: "C:\Documents\test.txt"  <-- (Quotes are auto-cleaned!)
```

### 3. Command Line Usage
```bash
# General syntax
./dhunju <file_path>
```

---

## 🔍 Binary Header Format
The serialized header follows this structure:
- `Total Characters` (4 bytes): Original byte count.
- `Unique Entries` (2 bytes): Number of unique characters.
- `Frequency Table`: Sequence of pairs `[Byte (1 byte)][Frequency (4 bytes)]`.

---

## 🤝 Contributing
Feel free to fork this project and submit pull requests. Future improvements could include multi-byte coding or canonical Huffman codes for smaller headers.
