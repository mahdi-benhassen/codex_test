# SHA-256 in C

## Project purpose
This project provides a simple command-line SHA-256 implementation written in C.

## Build
Compile with:

```bash
gcc -O2 -std=c11 -Wall -Wextra -o sha256 sha256.c
```

## Usage
Hash a string:

```bash
./sha256 -s "abc"
```

Hash a file:

```bash
./sha256 -f ./sample.bin
```

## Output format
The program prints a SHA-256 digest as lowercase hexadecimal with exactly **64 characters**.

## Verification
Known test vectors:

- `abc` → `ba7816bf...15ad`
- empty string (`""`) → `e3b0c442...b855`
