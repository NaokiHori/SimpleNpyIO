#############
Documentation
#############

Simple library to read / write `npy` files written in `C99`.

.. note::

   This project is still under development.

************
Introduction
************

============
Dependencies
============

   * C compiler which can compile C99 or later

   The following libraries are not necessary but recommended to :ref:`understand the flow <quick_start>`.

      * `GNU make <https://www.gnu.org/software/make/>`_

      * `Python3 <https://www.python.org>`_ with `Numpy <https://numpy.org/doc/stable/index.html>`_

      * `Git <https://git-scm.com>`_

.. _quick_start:

===========
Quick start
===========

   #. Clone the repository (Git is needed)

      .. code-block:: console

         $ git clone https://github.com/NaokiHori/SimpleNpyIO

   #. Compile sources

      .. code-block:: console

         $ cc -std=c99 -Iinclude simple_npyio.c example_reader.c  example_writer.c  main.c -o a.out

      or simply (make is needed)

      .. code-block:: console

         $ make

   #. Execute

      .. code-block:: console

         $ ./a.out

      to find

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

      indicating that a two-dimensional array is written to a `npy` file and loaded again.
      Of course write and read procedures are totally independent, namely array shape is recovered from the file.

   We also see (:sh:`ls -l`) a file ``example.npy`` is created, in which a two-dimensional array is stored.
   This can be easily confirmed by (python3 and numpy are needed)

   .. code-block:: console

      $ python3 check.py

   giving

   .. code-block:: text

      data
      [[ 0  1  2  3  4]
       [ 5  6  7  8  9]
       [10 11 12 13 14]]
      data.shape
      (3, 5)
      data.dtype
      int32

=====
Usage
=====

------------
Installation
------------

   Installation procedure (:sh:`./configure; make; make install`) does not exist; you can copy a header file `simple_npyio.h <https://github.com/NaokiHori/SimpleNpyIO/blob/master/include/simple_npyio.h>`_ and a source file `simple_npyio.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/simple_npyio.c>`_ to your project.

   After including the header file

   .. code-block:: c

      #include "simple_npyio.h"

   at the top of your source file ``my_awesome_function.c``, you can easily call writer and reader directly.

   For instance, imagine this is the original structure of your project:

   .. code-block:: text

      include
      └── my_awesome_functions.h
      Makefile
      src
      ├── my_awesome_function1.c
      ├── my_awesome_function2.c
      └── main.c

   and now you would like to use the library.
   Then what you need to do is to copy two files:

   .. code-block:: text

      include
      ├── my_awesome_functions.h
      └── simple_npyio.h
      Makefile
      src
      ├── my_awesome_function1.c
      ├── my_awesome_function2.c
      ├── main.c
      └── simple_npyio.c

   and compile them together.

   .. note::
      Although this library does not depend on any other external libraries, `C99` is used to write sources.
      Thus you might need to explicitly specify :sh:`-std=c99` (or later, e.g., :sh:`-std=c11` or :sh:`-std=c17`) as an option when compiling them.

--------------------
What we need to know
--------------------

   A `npy` file roughly consists of two parts: 1. header and 2. data.
   The latter part *data* simply stores our data (scalar, vector, array) as binary format.
   When we try to extract it from the file, we should know (at least) the following two additional information:

   #. Data type

      In most programming languages, several data types are available, e.g., floating number, integer, and character.
      It is non-trivial to know the original data type only by inspecting the data.

   #. Shape

      Let's assume the total size of the data is 32 bytes, and fortunately we remember that the type is 4-byte integer.
      It is, however, still non-trivial to recover the shape of the array, i.e., this data might have :math:`\left( 2 \times 4 \right)` shape (two-dimensional array) or :math:`\left( 1 \times 8 \right)` (vector).

   In order to retrieve the original data structure easily, these information should be attached to the data, which is the main purpose of the *header* part.

   In a `npy` file, the header part is located at the top, which is followed by the main data (other formats might use different forms, e.g., zip format).

   The implementation of the writer and reader shown blow simply create and load this header part, respectively.

^^^^^^
Writer
^^^^^^

   Roughly speaking, this library creates a `npy` file ``test.npy`` as

   .. code-block:: c

      // 1. open file
      FILE *fp = fopen("test.npy", "w");
      // 2. write header, which is the core of this library
      simple_npyio_w_header(ndim, shape, dtype,
            is_fortran_order, fp);
      // 3. write data
      fwrite(data, sizeof(int), length, fp);
      // 4. close file
      fclose(fp);

   A similar and complete script can be found in `src/example_writer.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/example_writer.c>`_, which was used in the :ref:`quick start <quick_start>`.

   .. details:: A bit more description can be found here.

      We consider the following example.

      .. code-block:: c

            int data[5] = {3, 1, 4, 1, 5};
            size_t ndim = 1;
            size_t shape[] = {5};
            char dtype[] = {"'i4'"};
            bool is_fortran_order = false;
            FILE *fp = fopen("test.npy", "w");
            simple_npyio_w_header(ndim, shape, dtype,
                  is_fortran_order, fp);
            fwrite(data, sizeof(int), 5, fp);
            fclose(fp);

      Imagine we have a data

      .. code-block:: c

         int data[] = {3, 1, 4, 1, 5};

      which is a vector (**1** -dimensional array) and its length is **5**, and want to write this to a `npy` file ``test.npy``.

      First we need to open the file as

      .. code-block:: c

         FILE *fp = fopen("test.npy", "w");

      In order to complete the header, we need five information:

      #. :c-lang:`ndim`

         Dimension of the data, :c-lang:`0` for a scalar, and :c-lang:`n` for :math:`n`-dimensional array.
         :c-lang:`1` is assigned in the above example since a vector is of our interest.

      #. :c-lang:`shape`

         Size of the data in each dimension.
         Note that :c-lang:`NULL` for a scalar.
         Only one number :c-lang:`5` is assigned in the above example since we are trying to dump a vector (:c-lang:`ndim = 1`) whose length is :c-lang:`5`.

      #. :c-lang:`dtype`

         Type of the data, e.g., :c-lang:`"'f8'"` or :c-lang:`"'float64'"` for double precision floating numbers under `normal` environments.
         4-byte interger :c-lang:`'i4'` is assumed in the above example.
         See `the documentation of Numpy <https://numpy.org/doc/stable/user/basics.types.html>`_ for all available data types.

         .. warning::
            User is responsible for specifying a consistent data type.
            Note that single (or double) quotations should be included as :c-lang:`dtype`, i.e., **NOT** :c-lang:`const char dtype[] = {"i4"}` but :c-lang:`const char dtype[] = {"'i4'"}` or :c-lang:`{"\"i4\""}` should be used.

      #. :c-lang:`is_fortran_order`

         This exists because the memory alignment is different from C to Fortran.
         Usually the user of this library can simply specify :c-lang:`false`.

      #. :c-lang:`fp`

         File stream to be written, which should be already opened with writable mode :c-lang:`"w"` (or sometimes :c-lang:`"a"`).

      Based on these information, the header part is first written to the file:

      .. code-block:: c

         simple_npyio_w_header(ndim, shape, dtype, is_fortran_order, fp);

      which is followed by writing the data itself:

      .. code-block:: c

         fwrite(data, sizeof(int), 5, fp);

      Finally, it is very important to close the file stream correctly:

      .. code-block:: c

         fclose(fp);

      to save the file properly.

^^^^^^
Reader
^^^^^^

   Similarly, this library loads a `npy` file ``test.npy`` as

   .. code-block:: c

      // 1. open file
      FILE *fp = fopen("test.npy", "r");
      // 2. read header, which is the core of this library
      //   NOTE: of course addresses should be passed,
      //   which is different from writer
      simple_npyio_r_header(&ndim, &shape, &dtype,
            &is_fortran_order, fp);
      // 3. prepare buffer to receive data
      //   using information obtained by "simple_npyio_r_header"
      //   ... you need to fill this part properly ...
      // 4. load data
      fread(data, sizeof(int), length, fp);
      // 5. close file
      fclose(fp);

   A similar and complete script can be found in `src/example_reader.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/example_reader.c>`_, which was used in the :ref:`quick start <quick_start>`.

