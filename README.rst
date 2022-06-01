###########
SimpleNpyIO
###########

|License|_

|DocDeployment|_

|WorkflowStatus|_ |UnitTest|_ |MemoryLeak|_

|LastCommit|_

.. |License| image:: https://img.shields.io/github/license/NaokiHori/SimpleNpyIO
.. _License: https://opensource.org/licenses/MIT

.. |DocDeployment| image:: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/documentation.yml/badge.svg
.. _DocDeployment: https://naokihori.github.io/SimpleNpyIO/

.. |WorkflowStatus| image:: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml/badge.svg?branch=master
.. _WorkflowStatus: https://github.com/NaokiHori/SimpleNpyIO/actions/workflows/ci.yml

.. |UnitTest| image:: https://github.com/NaokiHori/SimpleNpyIO/blob/artifacts/.github/workflows/artifacts/badge_cunit.svg
.. _UnitTest: https://github.com/NaokiHori/Alpine-Dockerfiles/tree/cunit

.. |MemoryLeak| image:: https://github.com/NaokiHori/SimpleNpyIO/blob/artifacts/.github/workflows/artifacts/badge_valgrind.svg
.. _MemoryLeak: https://github.com/NaokiHori/Alpine-Dockerfiles/tree/valgrind

.. |LastCommit| image:: https://img.shields.io/github/last-commit/NaokiHori/SimpleNpyIO/master
.. _LastCommit: https://github.com/NaokiHori/SimpleNpyIO/commits/master

This library contains a few simple scripts (written in `C99`) to write and read files in `npy format <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_.

************
Installation
************

Copy a source file ``src/simple_npyio.c`` and a corresponding header file ``include/simple_npyio.h`` to your project.
Now all functions are ready to be used.

Writing and reading examples can be found in ``src/example_writer.c`` and ``src/example_reader.c``, respectively.
They are, however, just for convenience (to see how they work) and not necessary to use this library.

*****
Usage
*****

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
   // prepare buffer to receive data
   //   for instance, if ndim == 2,
   //     size_t nitems = shape[0]*shape[1]
   //     size_t *data = calloc(nitems, sizeof(int));
   // user should specify appropriate data type from "dtype"
   fread(data, sizeof(int), nitems, fp);
   fclose(fp);
   // whatever you want

Please refer to the `documentation <https://naokihori.github.io/SimpleNpyIO>`_ for more detailed usages.

*************
Tests (CUnit)
*************

Based on a test frame work `CUnit <http://cunit.sourceforge.net>`_, unit tests are performed to keep the quality of the library, which can be found under ``cunit`` directory.

Although normal users do not have to look into it, it is possible to extend this library and try more extensive tests by themselves.
Please refer to `this page <https://github.com/NaokiHori/Alpine-Dockerfiles/tree/cunit>`_ for more details.

