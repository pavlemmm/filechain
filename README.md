# FileChain

FileChain is a simple file versioning project written in **C**.

It scans a folder, calculates a `SHA-256` hash for each file, stores changed file snapshots in `.filechain/objects/`, and records every change in a blockchain-style history file. It works a bit like Git.

## Files

```text
src/
  main.c
  chain.c
  history.c
  storage.c
  util.c
  chain.h
  util.h
  internal.h
```

Generated data:

```text
.filechain/
  index.txt
  chain.txt
  objects/
```

## Commands

Build:

```bash
make
```

Scan a folder:

```bash
./filechain scan <folder>
```

Show full history:

```bash
./filechain history
```

Show history for one file:

```bash
./filechain history <file>
```

Restore an older version:

```bash
./filechain restore <file> <hash>
```

## Example

```bash
./filechain scan demo
./filechain history
./filechain history demo/a.txt
./filechain restore demo/a.txt <hash>
```

Example history block:

```text
---
time 2026-05-08 11:43:09
file demo/a.txt
data_hash 2c8b08da5ce60398e1f19af0e5dccc744df274b826abe585eaba68c525434806
prev_hash 0
block_hash fb8344fca4242cd2903385227ea5d22b94576f1c5377df5780dfea35fff0ae47
object .filechain/objects/2c8b08da5ce60398e1f19af0e5dccc744df274b826abe585eaba68c525434806
```

## How It Works

When you run `scan`:

1. the program reads all regular files in the selected folder
2. it calculates a `SHA-256` hash for each file
3. it compares the new hash with the last saved hash from `index.txt`
4. if the file changed, it saves a snapshot in `.filechain/objects/`
5. it appends a new block to `chain.txt`

Each block stores:

- time
- file path
- file hash
- previous block hash
- current block hash
- object path

## Build Notes

This project uses **OpenSSL** for `SHA-256`.

The repository also includes `flake.nix` and `flake.lock`, so the easiest way to build it in a consistent environment is:

```bash
nix develop
make
```
