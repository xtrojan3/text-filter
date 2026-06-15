# 🔤 Text Filter (C)

A command-line text processing tool written in C. Reads lines from stdin, applies character filtering, case conversion, and word replacement based on the provided flags — then prints the result to stdout.

---

## Build

Requires a C11-compatible compiler (e.g. gcc or clang).

```bash
gcc -o textfilter main.c
```

Or with CMake if a `CMakeLists.txt` is present:

```bash
cmake -S . -B build && cmake --build build
```

---

## Usage

```
./textfilter [-d] [-p] [-l] [-u] [-r REPLACEMENT] [-R REPLACEMENT] [PREFIX ...]
```

Input is read from **stdin**, one line at a time. Processing stops on an empty line or EOF.

### Flags

| Flag | Description |
|------|-------------|
| `-d` | Keep digits (0–9) in output |
| `-p` | Keep punctuation characters |
| `-l` | Convert all letters to lowercase |
| `-u` | Convert all letters to uppercase |
| `-r STR` | Replace matching words with STR (case-sensitive prefix match) |
| `-R STR` | Replace matching words with STR (case-insensitive, preserves original casing) |

Without `-d` or `-p`, only letters and spaces are kept.

### Prefix filtering

Positional arguments after the flags are treated as **prefixes**. Only words starting with one of these prefixes are replaced. If no prefixes are given, all words are replaced.

---

## Processing order

Each line is processed in this sequence:

1. **Character filtering** — strips characters not allowed by flags
2. **Case conversion** — applies `-l` or `-u`
3. **Word replacement** — applies `-r` or `-R` with optional prefix matching

---

## Examples

```bash
# Keep only letters and spaces, convert to uppercase
echo "Hello, World! 123" | ./textfilter -u
# → HELLO WORLD

# Keep digits and punctuation too
echo "foo.bar 42" | ./textfilter -d -p
# → foo.bar 42

# Replace all words with "***"
echo "secret data here" | ./textfilter -r "***"
# → *** *** ***

# Replace only words starting with "sec" (case-insensitive, preserve case)
echo "Secret data Secured" | ./textfilter -R "xxxxx" sec
# → Xxxxx data Xxxxx
```

---

## Return values

| Code | Meaning |
|------|---------|
| `0` | Success |
| `1` | Unknown flag |
| `2` | Flag requires an argument but none was given |

---

## Notes

- Maximum line length is 1000 characters
- `-l` and `-u` can be combined but `-u` takes no special priority — whichever runs last wins (both are applied per character in sequence, so `-u` always wins)
- `-r` does a direct overwrite character-by-character up to the length of the shorter of the word or replacement string; the rest of the word is unchanged
- `-R` preserves the case pattern of the original word (uppercase letter → uppercase replacement letter)
