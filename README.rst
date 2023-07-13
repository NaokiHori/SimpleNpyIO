###########
SimpleNpyIO
###########

|License|_ |LastCommit|_ |CIStatus|_

|Test|_ |Memory|_

.. |License| image:: https://img.shields.io/github/license/NaokiHori/SimpleNpyIO
.. _License: https://opensource.org/license/mit/

.. |LastCommit| image:: https://img.shields.io/github/last-commit/NaokiHori/SimpleNpyIO/main
.. _LastCommit: https://github.com/NaokiHori/SimpleNpyIO/commits/main

.. |CIStatus| image:: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml/badge.svg?branch=main
.. _CIStatus: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml

.. |Test| image:: https://naokihori.github.io/SimpleNpyIO/_images/badge_test.svg
.. _Test: https://naokihori.github.io/SimpleNpyIO/test.html

.. |Memory| image:: https://naokihori.github.io/SimpleNpyIO/_images/badge_memoryleakcheck.svg
.. _Memory: https://naokihori.github.io/SimpleNpyIO/test.html

********
Overview
********

A ``C99``-compatible, single-header (``snpyio.h``) single-source (``snpyio.c``) library for file reading and writing in the `NPY format <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_.

The ``NPY`` file format is a binary format utilised for storing multi-dimensional arrays.
It comprises a binary dataset preceded by a header that contains the essential metadata to identify the dataset: shape, datatype, and memory order (either row-major or column-major).

Performing I/O operations on ``NPY`` files is extremely straightforward: load or dump the metadata, and then read or write the dataset.
This library handles the initial step.
Due to the simple format, parallel I/O, supported by ``MPI``, can be easily managed.

**********
Dependency
**********

* C compiler

***********
Quick start
***********

#. Prepare workspace:

   .. code-block:: console

      mkdir -p /path/to/your/working/directory
      cd /path/to/your/working/directory

#. Checkout this repository, e.g.:

   .. code-block:: console

      git clone https://github.com/NaokiHori/SimpleNpyIO
      cd SimpleNpyIO

#. Build and run:

   .. code-block:: console

      make
      ./a.out

This gives you:

.. code-block:: text

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

Very simple I/O operations were performed here as a showcase: a two-dimensional data (shown above) is written to a ``NPY`` file ``example.npy``, which is reloaded to show the second part.
The resulting file ``example.npy`` is easily inspected by using ``Python``:

.. code-block:: python

   import numpy as np

   data = np.load("example.npy")

   print(data.shape) # (3, 5)

Please check `src/main.c <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c>`_ for details.

***************
Practical Usage
***************

=================
Recommended: copy
=================

Copy `src/snpyio.c <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/snpyio.c>`_ and `include/snpyio.h <https://github.com/NaokiHori/SimpleNpyIO/blob/main/include/snpyio.h>`_ into your project:

.. code-block:: text

   include
   └── snpyio.h

   src
   ├── main.c
   └── snpyio.c

Here, ``main.c`` is your code.
Now all functions this library offers are ready.

Again, please check `src/main.c <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c>`_ for APIs.

=======================
Advanced: Git submodule
=======================

A ``submodule`` branch is available, which is specialised to be imported into an existing library, e.g.:

.. code-block:: console

   git submodule add --branch submodule https://github.com/NaokiHori/SimpleNpyIO

Make sure to include ``SimpleNpyIO/include/snpyio.h`` and compile ``SimpleNpyIO/src/snpyio.c`` with your sources.

You can find an example `here <https://github.com/NaokiHori/SimpleNSSolver>`_, where this library is imported and used (check ``.gitmodules``).

******
Others
******

For debugging purpose, please compile the source with ``SNPYIO_ENABLE_LOGGING`` flag.
Please refer to the `documentation <https://naokihori.github.io/SimpleNpyIO>`_.

