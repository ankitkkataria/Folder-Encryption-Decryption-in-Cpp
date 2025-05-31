# File Encryptor/Decrypter

A high-performance file encryption/decryption tool written in C++ with multiprocessing support.

## Features

- **Multi-threaded processing** for handling large numbers of files efficiently
- **Tested performance** up to 10,000 files
- **Memory-mapped semaphores** for inter-process communication

## Quick Start

### 1. Generate Test Files

Create random text files for testing:

```bash
python3 makeTestDir.py
```

> **Note:** Test files are not included in the repository as they contain random generated content.

### 2. Build the Project

```bash
make clean && make
```

### 3. Run the Encryptor

```bash
./encrypt_decrypt
```

## Performance & Limitations

| File Count | Status |
|------------|---------|
| ≤ 10,000   | ✅ Tested & Working |
| 20,000+    | ⚠️ May hang/freeze |

## Troubleshooting

### Program Stuck or Frozen?

If the program becomes unresponsive with large file counts (20,000+ files), you may need to force-quit it.

### Important Cleanup Step

⚠️ **Critical:** After forcefully exiting the program, you must clean up semaphore files:

```bash
sudo rm -rf /dev/shm/*
```

**Why this is necessary:**
- The program uses memory-mapped semaphore files stored in `/dev/shm`
- Abrupt termination leaves these files behind
- Leftover semaphore files will prevent the program from working properly on subsequent runs, even with fewer files

### Recovery Process

If you encounter issues after force-quitting:

1. Clear semaphore files: `sudo rm -rf /dev/shm/*`
2. Try running with a smaller number of test files

## Architecture

This tool leverages:
- **C++ multiprocessing** for parallel file operations
- **Memory-mapped semaphores** for process synchronization
- **Shared memory** (`/dev/shm`) for inter-process communication
