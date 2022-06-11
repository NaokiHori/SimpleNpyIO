*****
Usage
*****

============
Installation
============

Installation procedure (e.g., ``./configure; make; make install``) does not exist; you can copy a header file `simple_npyio.h <https://github.com/NaokiHori/SimpleNpyIO/blob/master/include/simple_npyio.h>`_ and a source file `simple_npyio.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/simple_npyio.c>`_ to your project.

After including the header file

.. code-block:: c

   #include "simple_npyio.h"

at the top of your source file, you can easily call writer and reader directly.

For instance, imagine this is the original structure of your project:

.. code-block:: text

   include
   └── my_awesome_functions.h
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
   src
   ├── my_awesome_function1.c
   ├── my_awesome_function2.c
   ├── main.c
   └── simple_npyio.c

and compile them together.

.. note::
   Although this library does not depend on any other external libraries, `C99` is used to write sources.
   Thus you might need to explicitly specify ``-std=c99`` (or later, e.g., ``-std=c11`` or ``-std=c17``) as an option when compiling them.

======================
What users should know
======================

A `npy` file roughly consists of two parts: 1. header and 2. data.
The latter part *data* simply stores data (scalar, vector, array) to be written in binary form.
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

------
Writer
------

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

A similar and complete script can be found in `src/main.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/main.c>`_, which was used in the :ref:`quick start <quick_start>`.

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

   #. ``ndim``

      Dimension of the data, ``0`` for a scalar, and ``n`` for :math:`n`-dimensional array.
      ``1`` is assigned in the above example since a vector is of our interest.

   #. ``shape``

      Size of the data in each dimension.
      Note that ``NULL`` for a scalar.
      Only one number ``5`` is assigned in the above example since we are trying to dump a vector (``ndim = 1``) whose length is ``5``.

   #. ``dtype``

      Type of the data, e.g., ``"'f8'"`` or ``"'float64'"`` for double precision floating numbers under `normal` environments.
      4-byte interger ``'i4'`` is assumed in the above example.
      See `the documentation of Numpy <https://numpy.org/doc/stable/user/basics.types.html>`_ for all available data types.

      .. warning::
         User is responsible for specifying a consistent data type.
         Note that single (or double) quotations should be included as ``dtype``, i.e., **NOT** ``const char dtype[] = {"i4"}`` but ``const char dtype[] = {"'i4'"}`` or ``{"\"i4\""}`` should be used.

   #. ``is_fortran_order``

      This exists because the memory alignment is different from C to Fortran.
      Usually the user of this library can simply specify ``false``.

   #. ``fp``

      File stream to be written, which should be already opened with writable mode ``"w"`` (or sometimes ``"a"``).

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

------
Reader
------

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

A similar and complete script can be found in `src/main.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/main.c>`_, which was used in the :ref:`quick start <quick_start>`.

