# Patricia Trie Address Search

This project implements a **Patricia trie (Practical Algorithm to Retrieve Information Coded in Alphanumeric)** to efficiently store and search address records by their `EZI_ADD` field.  
It supports **exact lookups** as well as **fuzzy lookups** (closest-match search using edit distance).

---

## ✨ Features

- **Patricia trie insertion**
  - Keys are stored as binary strings (bit-level branching, MSB-first).
  - Duplicate addresses with the same key are stored in the same leaf, preserving file input order.
- **Exact search**
  - Traverses the trie by branching bits until a leaf is reached.
  - Performs **one string comparison** at the leaf, charging bit comparisons correctly.
- **Closest-match (fuzzy) search**
  - If no exact match is found, computes the **edit distance** between the query and all candidate keys in the relevant subtree.
  - Returns the key with minimum edit distance (ties broken alphabetically).
- **Statistics tracked**
  - `b` = number of **bit comparisons** charged.
  - `n` = number of **node comparisons** (count of nodes visited along the descent path).
  - `s` = number of **string comparisons** (always `1`).
- **Memory management**
  - Trie nodes and dynamic record arrays are properly allocated and freed.

---

## 📂 File Overview

- `src/patricia.c`  
  Core Patricia trie implementation: node structure, insert, search, and free logic.
- `include/patricia.h`  
  Header for the Patricia trie API (`create_patricia_tree`, `insert_into_patricia`, `search_patricia`, etc.).
- `src/bit.c` / `include/bit.h`  
  Bit-level utilities (`getBit`) for MSB-first branching.
- `src/search.c` / `include/search.h`  
  Search utilities including `strcmp_bits_firstdiff` (counts differing bits) and result handling.
- `src/utils.c`  
  Miscellaneous helpers, including `editDistance` (Levenshtein distance).
- `src/main.c`  
  Example driver program to read input, build the trie, and execute searches.

---

## 🛠️ Build Instructions

This project uses a **Makefile**. To compile:

```bash
make -B
```

To clean build artifacts:
```bash
make clean
```

---

## Running

The program takes three arguments:

```bash
./dict2 <stage> <input.csv> <output.txt>
```

- `<stage>`  
  - `1` → linked-list search (baseline)  
  - `2` → Patricia trie search  

- `<input.csv>`  
  CSV file of address records  

- `<output.txt>`  
  File to write the results  

**Example:**

```bash
./dict2 2 tests/dataset_22.csv output.txt < tests/testpart22.in
```

---

## 📊 Output Format

Each query produces a line:

```lua
48 ROYAL --> 3 records found - comparisons: b67 n11 s1
```

**Where:**

- `48 ROYAL` → the query string  
- `3 records found` → number of matching records returned  
- `b67` → 67 bit comparisons charged  
- `n11` → 11 node comparisons performed  
- `s1` → 1 string comparison  

---

## 🔎 Counting Rules

**Bit comparisons (`b`)**  
Counted via `strcmp_bits_firstdiff`, which charges one unit for each bit examined until the first difference or string end.

**Node comparisons (`n`)**  
Only nodes on the **descent path** are counted:
- Each internal node traversed  
- The landing leaf  
- ✅ DFS for edit-distance candidates does **not** affect `n`

**String comparisons (`s`)**  
Always `1`, for the single landing-leaf string comparison.