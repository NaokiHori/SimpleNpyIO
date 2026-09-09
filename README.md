# SimpleNpyIO

[![License](https://img.shields.io/github/license/NaokiHori/SimpleNpyIO)](https://opensource.org/license/mit/)
[![Last Commit](https://img.shields.io/github/last-commit/NaokiHori/SimpleNpyIO/main)](https://github.com/NaokiHori/SimpleNpyIO/commits/main)
[![CI](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml)

## Overview

A `C99`-compatible, single-header (`snpyio.h`) single-source (`snpyio.c`) library for file reading and writing in the **simplest** [`NPY` format](https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html).

The `NPY` file format is a binary format which can be used to store multi-dimensional arrays.
It consists of a binary dataset preceded by a header containing metadata necessary to identify the dataset: shape, datatype, and memory order (row-major or column-major).

Performing I/O operations on `NPY` files is straightforward: load or dump the metadata, then read or write the dataset.
This library handles the initial step. The format's simplicity allows easy management of parallel I/O operations, supported by `MPI`.

## Caveat

This library focuses on dealing with multi-dimensional arrays (n-th order tensors, including scalars) which frequently appear in scientific computations.
Although `NPY` format is flexible enough to describe unstructured data, they are not supported for brevity.

## Dependency

- C compiler

## Quick Start

### 1. Prepare workspace

```bash
mkdir -p /path/to/your/working/directory
cd /path/to/your/working/directory
```

### 2. Clone repository

For example:

```bash
git clone https://github.com/NaokiHori/SimpleNpyIO
cd SimpleNpyIO
```

### 3. Build and run

```bash
make
./a.out
```

This outputs:

```text
data (dumped)
 0   1   2   3   4
 5   6   7   8   9
10  11  12  13  14
header is successfully dumped (size: 64)
header is successfully loaded (size: 64)
data (loaded)
 0   1   2   3   4
 5   6   7   8   9
10  11  12  13  14
```

This demonstrates simple I/O operations: a two-dimensional dataset (shown above) is written to an `NPY` file (`example.npy`) and then reloaded for display.
The resulting `example.npy` file can be inspected easily using Python:

```python
import numpy as np

data = np.load("example.npy")

print(data.shape)  # (3, 5)
```

See [`src/main.c`](./src/main.c) for more details.

## Functions

- `snpyio_read_header`: read metadata from a file
- `snpyio_write_header`: write metadata to a file

See [`include/snpyio.h`](./include/snpyio.h) for the arguments.

## How to use it in your code

### Recommended: Copy

Since this project is fairly simple, just copy [`src/snpyio.c`](./src/snpyio.c) and [`include/snpyio.h`](./include/snpyio.h) into your project would be the easiest:

```text
include
└── snpyio.h

src
├── main.c
└── snpyio.c
```

Here, `src/main.c` is your code.
All functions provided by this library are now available by including `snpyio.h`:

```c
#include "snpyio.h"
```

For API details and examples, refer to [`src/main.c`](./src/main.c).

### Advanced: Git Submodule

A `submodule` branch is available, designed to be imported into an existing project, for example:

```bash
git submodule add --branch submodule https://github.com/NaokiHori/SimpleNpyIO
```

Include `SimpleNpyIO/include/snpyio.h` and compile `SimpleNpyIO/src/snpyio.c` along with your sources.

See an example [here](https://github.com/NaokiHori/SimpleNSSolver), where this library is imported and used (refer to `.gitmodules`).

