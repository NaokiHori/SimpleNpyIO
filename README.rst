###########
SimpleNpyIO
###########

|License|_ |LastCommit|_ |DocDeployment|_ |WorkflowStatus|_

.. |License| image:: https://img.shields.io/github/license/NaokiHori/SimpleNpyIO
.. _License: https://opensource.org/licenses/MIT

.. |LastCommit| image:: https://img.shields.io/github/last-commit/NaokiHori/SimpleNpyIO/main
.. _LastCommit: https://github.com/NaokiHori/SimpleNpyIO/commits/main

.. |DocDeployment| image:: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/documentation.yml/badge.svg?branch=main
.. _DocDeployment: https://naokihori.github.io/SimpleNpyIO/

.. |WorkflowStatus| image:: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml/badge.svg?branch=main
.. _WorkflowStatus: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml

|CBadge| |DockerBadge| |GitHubActionsBadge| |GitHubPagesBadge|

.. |CBadge| image:: https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=C&logoColor=white

.. |DockerBadge| image:: https://img.shields.io/badge/Docker-2CA5E0?style=for-the-badge&logo=docker&logoColor=white

.. |GitHubActionsBadge| image:: https://img.shields.io/badge/GitHub_Actions-2088FF?style=for-the-badge&logo=github-actions&logoColor=white

.. |GitHubPagesBadge| image:: https://img.shields.io/badge/GitHub%20Pages-222222?style=for-the-badge&logo=GitHub%20Pages&logoColor=white

********
Overview
********

A simple script (written in `C99`) to write and read files in `npy format <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_.

=========
Serial IO
=========

In order to write a `npy` file, all you need to do is:

.. code-block:: c

   FILE *fp = fopen("test.npy", "w"); // user should check fp == NULL
   simple_npyio_w_header(ndim, shape, dtype, is_fortran_order, fp);
   fwrite(data, sizeof(int), length, fp);
   fclose(fp);

Of course this file is directly accessible using python as

.. code-block:: python

   import numpy as np

   data = np.load("test.npy")

   # whatever you want

To read a dataset from a file, what you need to do is to prepare a similar simple script:

.. code-block:: c

   FILE *fp = fopen("test.npy", "r"); // user should check fp == NULL
   simple_npyio_r_header(&ndim, &shape, &dtype, &is_fortran_order, fp);
   /* Here prepare buffer to receive data based on ndim, shape etc. */
   fread(data, sizeof(type_of_data), nitems, fp);
   fclose(fp);
   // whatever you want

Please refer to the `documentation <https://naokihori.github.io/SimpleNpyIO>`_ for more detailed usages.

======================
Parallel IO (Advanced)
======================

With `Message Passing Interface (MPI) <https://www.mpi-forum.org>`_, the same thing can be done in parallel.
The following code is much more complicated compared to the serial version, which mainly caused by MPI-IO, not because of the current library.

Writing a two dimensional array which is contiguous in `x (i)` direction while decomposed in `y (j)` direction to a `npy` file consists of two steps,

1. Writing header by main process

   .. code-block:: c

      MPI_Comm_rank(MPI_COMM_WORLD, &mpirank);
      size_t header_size;
      if(mpirank == 0){
        FILE *fp = fopen("test.npy", "w"); // user should check fp == NULL
        header_size = simple_npyio_w_header(ndim, shape, dtype, is_fortran_order, fp);
        fclose(fp);
      }

2. Writing data in parallel

   .. code-block:: c

      // share header size among all processes
      const size_t n_bytes = sizeof(header_size)/sizeof(char);
      MPI_Bcast(&header_size, count, MPI_CHAR, 0, MPI_COMM_WORLD);
      // open file and move file pointer to the end of header
      const int amode = MPI_MODE_APPEND | MPI_MODE_WRONLY;
      MPI_File fh = NULL;
      MPI_File_open(MPI_COMM_WORLD, "test.npy", amode, MPI_INFO_NULL, &fh);
      // offset from the beginning of file
      int offset = sizeof(int)*itot*joffset+(int)header_size;
      // parallel write
      MPI_File_write_at_all(fh, offset, data, jsize*itot, MPI_INT, MPI_STATUS_IGNORE);
      // do not forget to close file
      MPI_File_close(&fh);

***********
Quick start
***********

With `Docker <https://www.docker.com>`_ and `git <https://git-scm.com>`_, one can try this library easily and safely by

.. code-block:: console

   $ mkdir /path/to/your/working/directory

   $ cd /path/to/your/working/directory

   $ git clone https://github.com/NaokiHori/SimpleNpyIO

   $ cd SimpleNpyIO

   $ docker build -t simplenpyio:latest .

   $ docker run -it --rm -v $(PWD):/home simplenpyio:latest

   # make all

   # ./a.out

Without Docker, one needs a C compiler (later than C99), `git`, and `make` locally installed.

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
The output file can be easily inspected by using python, in particular by

.. code-block:: python

   import numpy as np

   data = np.load("example.npy")

   # whatever you want, e.g.,
   print(data.shape) # (3, 5)

Please check the `source file <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/main.c>`_ for details.

*************************
Usage in external library
*************************

Copy a source file ``src/simple_npyio.c`` and a corresponding header file ``include/simple_npyio.h`` to your project.
Now all functions are ready to be used.

Writing and reading examples can be found in ``src/main.c``.
This script exists, however, just for convenience (to see how they work) and not necessary to use this library.

*****
Tests
*****

|UnitTest|_ |IntegrationTest|_ |MemoryLeak|_

.. |UnitTest| image:: https://raw.githubusercontent.com/NaokiHori/SimpleNpyIO/artifacts/.github/workflows/artifacts/badge_unittest.svg
.. _UnitTest: https://github.com/NaokiHori/SimpleNpyIO/tree/artifacts/.github/workflows/artifacts

.. |IntegrationTest| image:: https://raw.githubusercontent.com/NaokiHori/SimpleNpyIO/artifacts/.github/workflows/artifacts/badge_integrationtest.svg
.. _IntegrationTest: https://github.com/NaokiHori/SimpleNpyIO/tree/artifacts/.github/workflows/artifacts

.. |MemoryLeak| image:: https://github.com/NaokiHori/SimpleNpyIO/blob/artifacts/.github/workflows/artifacts/badge_memoryleakcheck.svg
.. _MemoryLeak: https://github.com/NaokiHori/Alpine-Dockerfiles/tree/valgrind

=========
Unit test
=========

Unit tests are performed automatically using a test framework `CUnit <http://cunit.sourceforge.net>`_.
The details can be found under `test/unit <https://github.com/NaokiHori/SimpleNpyIO/tree/main/test/unit>`_.

================
Integration test
================

Integration tests are performed automatically by comparing `npy` files generated (natively) by Python and this library.
The details can be found under `test/integration <https://github.com/NaokiHori/SimpleNpyIO/tree/main/test/integration>`_.

=================
Memory leak check
=================

Generally file-IO routines are called for several times, especially in numerical simulations to dump computed quantities.
If memory leaks exist in the library, they accumulate in time, which may be catastrophic, and thus deallocating all memories used in the library *correctly* is important.
This feature is also tested as well as the unit and integration tests automatically here using `Valgrind <https://valgrind.org>`_.

