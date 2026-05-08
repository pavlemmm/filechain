# FileChain

FileChain is a project written in **C** that implements a **blockchain-based file versioning system**.

The idea of the project is to use blockchain principles to track file changes inside a folder. Every new or modified version of a file is recorded as a new block in the chain. It also works a bit like Git.

It is not only storing file versions, but also linking those versions through hashes. Each block stores its own hash and the hash of the previous block, which creates a chain of changes.

To keep the system clean and readable, the full file content is not written directly into the history file. Instead, FileChain stores only the important metadata in the chain, while the actual saved versions are kept as separate snapshot objects.

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

After a scan, `index.txt` stores the latest known hash for each tracked file, `chain.txt` stores the history of changes, and `objects/` stores the saved snapshots named by their hash values.

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

When you run `scan`, the program does the following:

1. it goes through all regular files in the selected folder
2. it reads the content of each file
3. it calculates a `SHA-256` hash for that content
4. it compares the new hash with the last saved hash from `index.txt`
5. if the hash is different, it treats the file as a new version
6. it saves a snapshot in `.filechain/objects/`
7. it appends a new block to `chain.txt`

This means that every detected change becomes a new block in the blockchain history. Because each block contains both `prev_hash` and `block_hash`, the history is linked together as a chain rather than as a simple list.

Each block stores:

- time
- file path
- file hash
- previous block hash
- current block hash
- object path

The `restore` command uses the saved object snapshot and copies it back to the selected file path, which makes it possible to return to an older version.

## Build Notes

This project uses **OpenSSL** for `SHA-256`.

The repository also includes `flake.nix` and `flake.lock`, so the easiest way to build it in a consistent environment is:

```bash
nix develop
make
```
