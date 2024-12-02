##############
Implementation
##############

******
Writer
******

.. mydeclare:: /../../include/snpyio.h
   :language: c
   :tag: snpyio_w_header

.. warning::

   The ``shape`` parameter must contain exactly ``ndim`` elements.
   Users are responsible for correctly allocating memory to prevent undefined behavior.

.. note::

   The user must open the file in writable mode (e.g., ``"w"``) before calling this function.

.. note::

    The last argument ``header_size`` denotes the size of the header written to the file stream `fp`.
    This can be used to know the offset from the top of the file pointer after which the main dataset is stored.

The procedure for writing a ``NPY`` header is managed by the main function ``snpyio_w_header``, which includes the following steps:

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
      create_dict:s   -> major_version:n [label="success"];
      create_dict:e   -> error           [label="failure"];
      major_version:s -> header_len:n    [label="success"];
      major_version:e -> error           [label="failure"];
      header_len:s    -> output:n        [label="success"];
      header_len:e    -> error           [label="failure"];
      output:s        -> end             [label="return 0"];
      output:e        -> error           [label="failure"];
      error:s         -> end             [label="return non-zero value"];

   }

******
Reader
******

.. mydeclare:: /../../include/snpyio.h
   :language: c
   :tag: snpyio_r_header

.. warning::

   The ``shape`` and ``dtype`` parameters are allocated within this library and returned to the user.
   Users are responsible for managing (deallocating) their memory properly to prevent memory leaks.

.. note::

   The user must open the file in readable mode (e.g., ``"r"``) before calling this function.

.. note::

    The last argument ``header_size`` denotes the size of the header loaded from the file stream `fp`.
    This can be used to know the offset from the top of the file pointer after which the main dataset is stored.

The procedure for reading a ``NPY`` header is managed by the main function ``snpyio_r_header``, which includes the following steps:

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
      magic_string:s       -> versions:n           [label="success"];
      magic_string:e       -> error                [label="failure"]
      versions:s           -> header_len:n         [label="success"];
      versions:e           -> error                [label="failure"]
      header_len:s         -> load_dict_padding:n  [label="success"];
      header_len:e         -> error                [label="failure"]
      load_dict_padding:s  -> extract_dictionary:n [label="success"];
      load_dict_padding:e  -> error                [label="failure"]
      extract_dictionary:s -> extract_values:n     [label="success"];
      extract_dictionary:e -> error                [label="failure"]
      extract_values:s     -> end                  [label="return 0"];
      extract_values:e     -> error                [label="failure"]
      error:s              -> end                  [label="return non-zero value"];

   }

**************
Error handling
**************

File-IO operations can fail due to external factors.
This library gracefully handles such errors without terminating the program by avoiding calls to the ``exit()`` function, except the occurrence of fatal errors such as memory allocation failure.
In both normal and error exit scenarios, the writer and reader functions free all internally allocated memory before returning an error code to the user, ensuring no memory leaks occur.

There are two exceptions: the reader function returns the data type (e.g., ``'"<f8"'``) and the shape of the data (e.g., ``[123, 456]``) to the user.
It is crucial for users to deallocate these resources properly after use to prevent memory leaks.

