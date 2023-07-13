# SimpleNpyIO

[![License](https://img.shields.io/github/license/NaokiHori/SimpleNpyIO)](https://opensource.org/license/mit/)
[![Last Commit](https://img.shields.io/github/last-commit/NaokiHori/SimpleNpyIO/main)](https://github.com/NaokiHori/SimpleNpyIO/commits/main)
[![CI](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml)
[![Deploy-Pages](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/pages.yml/badge.svg?branch=main)](https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/pages.yml)

## Overview

A `C99`-compatible, single-header (`snpyio.h`) single-source (`snpyio.c`) library for file reading and writing in the [`NPY` format](https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html).

The `NPY` file format is a binary format used to store multi-dimensional arrays.
It consists of a binary dataset preceded by a header containing metadata necessary to identify the dataset: shape, datatype, and memory order (row-major or column-major).

Performing I/O operations on `NPY` files is straightforward: load or dump the metadata, then read or write the dataset.
This library handles the initial step. The format's simplicity allows easy management of parallel I/O operations, supported by `MPI`.

## Dependency

- C compiler

## Quick Start

1. Prepare the workspace:

   ```console
   mkdir -p /path/to/your/working/directory
   cd /path/to/your/working/directory
   ```

1. Clone this repository, for example:

   ```console
   git clone https://github.com/NaokiHori/SimpleNpyIO
   cd SimpleNpyIO
   ```

1. Build and run:

   ```console
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

   See [`src/main.c`](https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c) for more details.

## Practical Usage

### Recommended: Copy

Copy [`src/snpyio.c`](https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/snpyio.c) and [`include/snpyio.h`](https://github.com/NaokiHori/SimpleNpyIO/blob/main/include/snpyio.h) into your project:

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

For API details and examples, refer to [`src/main.c`](https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c).

### Advanced: Git Submodule

A `submodule` branch is available, designed to be imported into an existing project, for example:

```console
git submodule add --branch submodule https://github.com/NaokiHori/SimpleNpyIO
```

Include `SimpleNpyIO/include/snpyio.h` and compile `SimpleNpyIO/src/snpyio.c` along with your sources.

See an example [here](https://github.com/NaokiHori/SimpleNSSolver), where this library is imported and used (refer to `.gitmodules`).

## Others

For debugging purposes, compile the source code with the `SNPYIO_ENABLE_LOGGING` flag enabled.
Refer to the [documentation](https://naokihori.github.io/SimpleNpyIO).

