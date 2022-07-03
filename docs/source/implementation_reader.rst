***********************
Implementation - reader
***********************

Read header of `npy <https://numpy.org/doc/stable/reference/generated/numpy.lib.format.html>`_ file.

==========
Definition
==========

.. code-block:: c

   size_t simple_npyio_r_header(
     size_t *ndim,
     size_t **shape,
     char **dtype,
     bool *is_fortran_order,
     FILE *fp
   );

================ ====== ================================
Name             intent description
================ ====== ================================
ndim             out    dimension of dataset
shape            out    sizes in each direction
dtype            out    data type of dataset
is_fortran_order out    memory order
fp               in     file stream
header_size      return size of header which is written
================ ====== ================================

.. warning::

   Heap memories of ``shape`` and ``dtype`` are allocated inside this library.
   Deallocating them correctly is the users' responsibility to avoid memory leak.

.. note::

   File should be opened a priori by the user with a readable mode (e.g. ``"r"``).

.. note::

   This function returns ``header_size``, which is the size of header loaded from the given file stream ``fp`` when all succeeded.
   ``0`` is returned, on the other hand, when failed.
   It is recommended to check the returned value.

===========
Description
===========

Loading procedure of a `npy` header is taken care of by a main function ``simple_npyio_r_header``, which consists of the following process:

.. graphviz::

   digraph {

      spline = false;

      start [
         label = "Start";
         shape = oval;
      ];
      magic_string [
         label = "Check\nmagic string";
         shape = diamond;
      ];
      versions [
         label = "Check\nversions";
         shape = diamond;
      ];
      header_len [
         label = "Load\nHEADER_LEN";
         shape = diamond;
      ];
      load_dict_padding [
         label = "Load dictionary\nand padding";
         shape = diamond;
      ];
      extract_dictionary [
         label = "Purify\ndictionary";
         shape = diamond;
      ];
      extract_values [
         label = "Extract values\nfrom dictionary";
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

      start:s              -> magic_string:n;
      magic_string:s       -> versions:n           [label="succeeded"];
      magic_string:e       -> error                [label="failed"]
      versions:s           -> header_len:n         [label="succeeded"];
      versions:e           -> error                [label="failed"]
      header_len:s         -> load_dict_padding:n  [label="succeeded"];
      header_len:e         -> error                [label="failed"]
      load_dict_padding:s  -> extract_dictionary:n [label="succeeded"];
      load_dict_padding:e  -> error                [label="failed"]
      extract_dictionary:s -> extract_values:n     [label="succeeded"];
      extract_dictionary:e -> error                [label="failed"]
      extract_values:s     -> end                  [label="return header_size"];
      extract_values:e     -> error                [label="failed"]
      error:s              -> end                  [label="return 0"];

   }

Detailed descriptions can be found hereafter.

.. note::

   All snippets come from `src/simple_npyio.c <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/simple_npyio.c>`_.

#. Check magic string

   All `npy` files start with a fixed magic characters ``\x93NUMPY``, which is defined as a global variable at the beginning of the source.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: all npy files should start from this magic string

   As a first step of reading procedure, we check whether the currently-opened file really has this signature by comparing the first 6 bytes:

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: check magic string

   .. note::
      ``buf_size`` describes the size (in bytes) which is loaded from the file.

#. Check versions

   For the time being version `1.0`, `2.0`, and `3.0` are accepted, which is checked here.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: check versions

#. Load ``HEADER_LEN``

   As the title says, we load ``HEADER_LEN``, which is the size of dictionary and padding.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: load HEADER_LEN

   .. details:: More details: ``load_header_len``

      The data size of ``HEADER_LEN`` differs depending on the major version, i.e., generally 2 bytes (``unsigned short``) for ``major_version = 1`` while 4 bytes (``unsigned int``) for ``major_version = 2``, which should be taken into account when loaded:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: buffer size differs based on major_version

      Once the data size (interpreted as the number of ``uint8_t`` values: ``nitems``) is specified, information is loaded from file to a buffer ``buf``:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: allocate buffer and load corresponding memory size from file

      Note that the endian should be converted if the architecture is big-endian-based, since `npy` file stores ``HEADER_LEN`` in little-endian form:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: convert endian of loaded buffer when needed

      Finally series of ``uint8_t`` values are interpreted as a single value whose data type is ``uint16_t`` and ``uint32_t`` for ``major_version = 1`` and ``2``, respectively:

      .. myliteralinclude:: /../../src/simple_npyio.c
         :language: c
         :tag: interpret buffer (sequence of uint8_t) as a value having corresponding datatype

#. Load dictionary and padding

   In the previous step, we know the total size of dictionary and padding, namely ``HEADER_LEN``.
   Here ``HEADER_LEN`` bytes are loaded from file to extract dictionary and padding.

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: load dictionary and padding

   Nothing complicated exists here; basically this function simply calls ``fread`` with some error checks.
   Now all information are loaded from file and we do not have to access to the give file stream hereafter.

   .. note::
      Although padding is not necessary to obtain the information of the stored dataset, here we load it as well as the dictionary to move the file pointer, so that users can simply use ``fread`` just after this function to load the main dataset.
      The size of padding is 128 bytes in maximum and the overhead is negligibly small.

#. Purify dictionary

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: step 2: extract dictionary

   In the previous step, we obtained a mixture of dictionary and padding.
   The latter is not useful, which is removed here.
   Also Python dictionaries can hold arbitrary number of spaces in general to improve the readability.
   These spaces are also eliminated here, e.g., when the original mixture of dictionary and padding is

   .. code-block:: python

      {'descr': '<i4','fortran_order': False,'shape': (3, 5, )}[ PADDING ]

   This function converts it to

   .. code-block:: python

      {'descr':'<i4','fortran_order':False,'shape':(3,5,)}

   Notice that padding ``[ PADDING ]`` and unnecessary spaces are removed.
   Here new buffer ``dict``, which only contains these *pure* information, is allocated and thus ``dict_and_padding``, which is the raw mixture of dictionary and padding, is deallocated.

   .. details:: More details: ``extract_dict``

      This function includes two features, 1. removing padding, 2. removing unnecessary spaces

      #. Removing padding

         Because of the definition of Python dictionary, this can be achieved by looking for ``'{'`` and ``'}'``

      #. Removing spaces

         First we mark all essential characters, which are part of dictionary.

         .. note::
            Keys can contain spaces (e.g., ``'my key'``), which should NOT be eliminated.
            Although for now official `npy` parser only allows exactly three keys which do not contain any space, (``'descr'``, ``'fortran_order'``, and ``'shape'``, see `the implementation <https://github.com/numpy/numpy/blob/fd45ab90e9ce6f6724a57f215592fd3066a7436c/numpy/lib/format.py#L605>`_), this special care is considered in this library for future extensions.

#. Extract values from dictionary

   The final task is to extract three values (whose keys are ``'descr'``, ``'fortran_order'``, and ``'shape'``) of the given dictionary, which is done here:

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: extract value of shape

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: extract value of descr

   .. myliteralinclude:: /../../src/simple_npyio.c
      :language: c
      :tag: extract value of fortran_order

   Each function look for the key first, which is followed by the extraction of the counterpart value (``find_dict_value``).
   The results are stored to a buffer ``val``.

   Next, the value is parsed to obtain the final information by ``extract_shape``, ``extract_dtype``, and ``extract_is_fortran_order``, respectively, whose results (``ndim``, ``shape``, ``dtype``, and ``is_fortran_order``) are returned to the user.

.. note::

   This function returns ``header_size``, which is the size of header loaded from the given file stream ``fp`` when all succeeded.
   ``0`` is returned, on the other hand, when failed.
   It is recommended to check the returned value.

.. warning::

   Heap memories of ``shape`` and ``dtype`` are allocated inside this library.
   Deallocating them correctly is the users' responsibility to avoid memory leak.

