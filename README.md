# ox: a compact toolkit for sequence data analysis

`ox` is a modular command-line toolkit written in C for compressing, encoding, analyzing, and profiling symbolic sequences. 
It includes bit-packing, entropy metrics, histogramming, finite-context modeling, and CRC-based hashing.

---

## 🔧 Build

```bash
make
```

---

## 🚀 Usage

```bash
./ox <command> [options]
```

---

## 🧰 Available Commands

### `generate`

Generate random symbolic sequences.

```bash
./ox generate [-s <size>] [-c <cardinality>] [-e <seed>] <filename>
```

- `-s`: sequence size
- `-c`: alphabet cardinality (0–255)
- `-e`: random seed

---

### `pack2` / `pack4`

Bit-pack sequences using 2-bit (`ABCD`) or 4-bit (`A–P`) encodings.

```bash
./ox pack2 pack <input> <output>
./ox pack2 unpack <input> <output>

./ox pack4 pack <input> <output>
./ox pack4 unpack <input> <output>
```

---

### `xrc-256`

Encode/decode sequences with a custom XRC-256 codec: order-0 followed by a Range Coder.

```bash
./ox xrc-256 encode <input> <output>
./ox xrc-256 decode <input> <output>
```

---

### `entropy`

Compute Shannon entropy of binary input.

```bash
./ox entropy [-v] <filename>
```

- `-v`: verbose output (byte frequencies and count)

---

### `histogram`

Analyze distribution of values in a file (supports 8 and 16 bits).

```bash
./ox histogram [-h] [-t 8|16] [-w <width>] [-p] <filename>
```

- `-t`: data type (8 or 16 bits)
- `-w`: histogram width
- `-p`: plot instead of raw values
- `-h`: hide zero-count bins

---

### `distance`

Measure pattern distances in a sequence.

```bash
./ox distance -t <pattern> <filename>
```

- `-t`: pattern (e.g., `RRR`, `EXFGGHH`)

---

### `crc32-hash`

Compute CRC32 checksum.

```bash
./ox crc32-hash <filename>
```

---

### `profile`

Estimate local complexity using a finite-context model.

```bash
./ox profile [-k <ctx>] [-a <alphaDen>] [-w <window>] <filename>
```

- `-k`: model context order
- `-a`: smoothing parameter (1/a)
- `-w`: sliding window size

---

### `pipelines`

Print predefined analysis pipelines.

```bash
./ox pipelines
```

Example pipeline for DNA compression and decompression:

```bash
#!/bin/bash
grep -v '>' DNA.fa | tr -d -c 'ACGT' | tr 'ACGT' 'ABCD' > A.seq
./ox pack2 pack A.seq A.packed
./ox xrc-256 encode A.packed A.encoded

./ox xrc-256 decode A.encoded A.decoded
./ox pack2 unpack A.decoded A.unpacked
cmp A.unpacked A.seq
```

---

### `version`

Print program version.

```bash
./ox version
```

---

## 📦 Input Assumptions

- `pack2`: expects sequence with only `'A'`, `'B'`, `'C'`, `'D'`
- `pack4`: expects symbols from `'A'` to `'P'`

---

## 🧪 Example Workflow

```bash
# Prepare sequence
grep -v '>' input.fa | tr -d -c 'ACGT' | tr 'ACGT' 'ABCD' > seq.txt

# Pack using 2-bit
./ox pack2 pack seq.txt packed.bin

# Encode with custom codec
./ox xrc-256 encode packed.bin encoded.bin

# Decode
./ox xrc-256 decode encoded.bin decoded.bin

# Unpack to original
./ox pack2 unpack decoded.bin unpacked.txt

# Validate
cmp seq.txt unpacked.txt
```

---

## ⚖️ License

GPLv3 License

---
