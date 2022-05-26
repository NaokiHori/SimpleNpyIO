###########
SimpleNpyIO
###########

This library contains a few simple scripts (written in `C99`) to write and read files in `npy format <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_.

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

Please refer to the `documentation <https://naokihori.github.io/SimpleNpyIO>`_ for usages.
