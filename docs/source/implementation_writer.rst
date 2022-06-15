***********************
Implementation - writer
***********************

Write header of `npy <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_ file.

==========
Definition
==========

.. code-block:: c

   size_t simple_npyio_w_header(
     const size_t ndim,
     const size_t *shape,
     const char dtype[],
     const bool is_fortran_order,
     FILE *fp
   );

================ ====== ================================
Name             intent description
================ ====== ================================
ndim             in     dimension of dataset
shape            in     sizes in each direction
dtype            in     data type of dataset
is_fortran_order in     memory order (usually ``false``)
fp               in/out file stream
header_size      return size of header which is written
================ ====== ================================

.. warning::

   ``shape`` should have ``ndim`` elements.
   Allocating its memory correctly is the users' responsibility to avoid undefined behaviours.

.. note::

   File should be opened a priori by the user with a writable mode (e.g. ``"w"``).

.. note::

   This function returns ``header_size``, which is the size of header written to the given file stream ``fp`` when all succeeded.
   ``0`` is returned, on the other hand, when failed.
   It is recommended to check the returned value.

===========
Description
===========

Wiring procedure of a `npy` header is taken care of by a main function ``simple_npyio_w_header``, which consists of the following process:

.. graphviz::

   digraph {

      spline = false;

      start [
         label = "Start";
         shape = oval;
      ];
      magic_string [
         label = "Prepare\nmagic string";
         shape = rect;
      ];
      minor_version [
         label = "Decide\nminor version";
         shape = rect;
      ];
      create_dict [
         label = "Create\ndictionary";
         shape = diamond;
      ];
      major_version [
         label = "Decide major version\ncreate padding";
         shape = diamond;
      ];
      header_len [
         label = "Compute\nHEADER_LEN";
         shape = diamond;
      ];
      output [
         label = "Output\nheader";
         shape = diamond;
      ];
      end [
         label = "End";
         shape = oval;
      ];
      error [
         label = "Error\nhandlings"
         shape = rect;
      ];

      start:s         -> magic_string:n;
      magic_string:s  -> minor_version:n;
      minor_version:s -> create_dict:n;
      create_dict:s   -> major_version:n [label="succeeded"];
      create_dict:e   -> error           [label="failed\n(return 0)"];
      major_version:s -> header_len:n    [label="succeeded"];
      major_version:e -> error           [label="failed\n(return 0)"];
      header_len:s    -> output:n        [label="succeeded"];
      header_len:e    -> error           [label="failed\n(return 0)"];
      output:s        -> end             [label="succeeded\n(return header_size)"];
      output:e        -> error           [label="failed\n(return 0)"];
      error:s         -> end             [label="failed\n(return 0)"];

   }

Detailed descriptions can be found hereafter.

.. note::

   All snippets come from `src/simple_npyio.c <https://github.com/NaokiHori/SimpleNpyIO/blob/master/src/simple_npyio.c>`_.

#. Prepare magic string

   All `npy` files start with a fixed magic characters ``\x93NUMPY``, which is defined as a global variable at the beginning of the source.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: all npy files should start from this magic string

#. Decide minor version

   For the time being minor version is fixed to 0.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: minor_version, always 0

#. Create dictionary

   Dictionary contains three components, data type, memory order, and shape of the dataset, which are taken care of by a function ``create_dict``.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: dictionary (and its size)

   .. note::

      As can be seen, this function returns 0, indicating the function failed.
      The total size of the created dictionary ``n_dict`` is returned on the other hand, when all procedures succeed.
      This behaviour is consistent for all functions below.

   .. details:: More details: ``create_dict``

      Three keys are needed to be a correct `npy` file, whose values and corresponding dictionary are generated in this function.

      * data type of the dataset

         .. details:: More details

            Key is ``'descr'``, whose value is given by ``create_descr_value``.
            For now user input is simply copied and used as a data type, and as a result users are responsible for giving a correct data type.
            For proper datatypes which can be interpreted by Numpy, please refer to `the documentation <https://numpy.org/doc/stable/reference/arrays.dtypes.html>`_.

            .. myliteralinclude:: /../../src/simple_npyio.c
               :language: c
               :tag: create value of data type

            Note that ``-1`` is used as a returned value representing failure of a function, while ``0`` is used to say the function is terminated successfully.

      * memory order of the dataset

         .. details:: More details

            Key is ``'fortran_order'``, whose value is given by ``create_fortran_order_value``.
            This function takes a boolean input ``is_fortran_order``, which is converted to a corresponding string ``True`` or ``False``.
            Since this library is written in C, the value is ``False`` normally.

            .. myliteralinclude:: /../../src/simple_npyio.c
               :language: c
               :tag: create value of memory order

      * data sizes of the dataset

         .. details:: More details

            Key is ``'shape'``, whose value is given by ``create_shape_value``.
            It should be a Python tuple, e.g.,

            ================== ===========================================================================
            value (``char *``) description
            ================== ===========================================================================
            ``()``             scalar
            ``(3,)``           one-dimensional array (vector) whose size is 3
            ``(3,5,)``         two-dimensional array whose outer and inner sizes are 3 and 5, respectively
            and so on
            ================== ===========================================================================

            This function generates this kind of data from the given ``ndim`` and ``shape``.

            .. myliteralinclude:: /../../src/simple_npyio.c
               :language: c
               :tag: create value of data sizes

      These results are assigned to a variable ``elements``

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: 2. assign all elements (strings) which compose dict

      to build up the final dictionary (``char *dict``) such as

      .. code-block:: python

         {'descr':'<i4','fortran_order':False,'shape':(3,5,)}

      .. note::

         Arbitray number of spaces (outside quotations since keys should be as they are) can be added to the dictionary (as we normally do to improve visibility).
         Also pair of single quotations can be replaced by pair of double quotations.
         These differences are trivial for writing process, while the reader should take care of them.

#. Decide major version and padding

   Major version depends on the size of the dictionary, which is the reason why deciding it is postponed.

   The total size of the header excluding metadata (magic_string and versions) needs to be stored in ``HEADER_LEN`` later, whose data type is ``unsigned short`` (normally 2 bytes) by default (major version is 1).
   Thus, when ``HEADER_LEN`` exceeds 65535 (``USHRT_MAX``: maximum value for an unsigned short value to hold), it fails to keep the correct data size.
   In order to avoid this issue, major version of 2 must be used for datasets whose header size is moderately large, when the datatype of ``HEADER_LEN`` is *upgraded* to ``unsigned int`` (normally 4 bytes).

   Also total header size should be divisible by 64; in order to achieve this, some paddings are added at the end of the header.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: major_version and padding (and its size)

   .. details:: More details: ``create_padding``

      Sizes of elements which are already known are computed first:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: size of each element is computed

      Even when the major version is 2, header size larger than ``UINT_MAX`` cannot be stored, which is checked here:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: reject too large dict

      Similarly, ``USHRT_MAX`` (a bit looser for safety) is used as a threshold to decide the major version as well as ``size_header_len`` (datasize of ``HEADER_LEN``)

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: decide major version and datatype of HEADER_LEN

      Now we know the data sizes of all elements in the header *except the padding*:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: compute size of all data except padding

      Padding exists to make the total header size divisible by 64, which is achieved by

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: decide total size of the header, which should be 64 x N

      Finally the padding is created, which consists of many spaces ``0x20`` and last single newline character ``0x0a``:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: create padding

#. Compute ``HEADER_LEN``

   ``HEADER_LEN`` is well explained in the documentation of the `npy` format.
   Roughly speaking, it is the total size of dictionary and padding.
   This number, however, needs to be written in little-endian form, and can have different data size depending on the major version.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: comptue header_len

   .. details:: More details: ``create_header_len``

      Dictionaries having too large size (and padding just in case) are rejected for safety:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: reject too large dict / padding sizes

      Note that this is not talking about the size of **data**, but size of **header**.

      ``HEADER_LEN`` is computed here:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: compute header_len and store as an array of unit8_t

      It is first computed naively, e.g., for ``major_version = 1``,

      .. code-block:: c

          uint16_t header_len_uint16_t = (uint16_t)n_dict+(uint16_t)n_padding;

      which is stored as an array of ``uint8_t``, e.g., for ``major_version = 1``,

      .. code-block:: c

          *n_header_len = sizeof(uint16_t)/sizeof(uint8_t);
          *header_len = my_calloc(*n_header_len, sizeof(uint8_t));
          memcpy(*header_len, &header_len_uint16_t, *n_header_len);

      where a buffer ``header_len``, whose data type is ``uint8_t`` and number of items is ``n_header_len``, is allocated and assigned.

      Finally the endian of this array is converted if the architecture is big-endian:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: convert endian of buffer which will be written if needed

