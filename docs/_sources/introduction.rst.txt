************
Introduction
************

============
Dependencies
============

* C compiler (which can compile C99 or later).

===========
Quick start
===========

.. _quick_start:

In order to understand how things are handled, the following procedures would be helpful, in which some external libraries are very useful.

* `Git <https://git-scm.com>`_, which is to clone this project from GitHub.

Apart from Git, there are mainly two options.

#. `Docker <https://www.docker.com>`_ (recommended)

   .. code-block:: console

      $ mkdir /path/to/your/working/directory

      $ cd /path/to/your/working/directory

      $ git clone https://github.com/NaokiHori/SimpleNpyIO

      $ cd SimpleNpyIO

      $ docker build -t simplenpyio:latest .

      $ docker run -it --rm -v $(PWD):/home simplenpyio:latest

      # make all

      # ./a.out

#. C compiler which can compile C99 or later (e.g., `Clang <https://clang.llvm.org>`_), `GNU make <https://www.gnu.org/software/make/>`_, `Python3 <https://www.python.org>`_ with `Numpy <https://numpy.org>`_.

   .. code-block:: console

      $ mkdir /path/to/your/working/directory

      $ cd /path/to/your/working/directory

      $ git clone https://github.com/NaokiHori/SimpleNpyIO

      $ cd SimpleNpyIO

      $ make

      $ ./a.out

Both gives the same results:

.. code-block:: text

   data (written):
     0   1   2   3   4
     5   6   7   8   9
    10  11  12  13  14
   header is successfully written (size: 64)
   header is successfully loaded  (size: 64)
   data (loaded):
     0   1   2   3   4
     5   6   7   8   9
    10  11  12  13  14

where a two-dimensional data (shown above) is written to a `npy` file ``example.npy``, which is loaded again to show the second array.
The output file can be easily inspected by using python, in particular with this kind of script:

.. code-block:: python

   import numpy as np

   data = np.load("example.npy")

   # whatever you want, e.g.,
   print(data.shape) # (3, 5)

Please refer to `a sample script <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c>`_ which generated the above results.

