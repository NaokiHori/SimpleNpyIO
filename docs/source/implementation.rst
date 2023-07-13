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

   ``shape`` should have ``ndim`` elements.
   It is the responsibility of the users to allocate memory correctly in order to prevent undefined behaviors.

.. note::

   The user should open the file beforehand with a writable mode (such as ``"w"``).

.. note::

   This function returns a value, which represents the size of the header that was successfully written to the file stream ``fp``.
   If the function fails, it returns ``0``.
   It is recommended to always check the value returned by the function.

The writing procedure for a ``NPY`` header is handled by the main function ``snpyio_w_header``, which includes the following process:

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
      output:s        -> end             [label="return header_size"];
      output:e        -> error           [label="failure"];
      error:s         -> end             [label="return 0"];

   }

******
Reader
******

.. mydeclare:: /../../include/snpyio.h
   :language: c
   :tag: snpyio_r_header

.. warning::

   ``shape`` and ``dtype`` are allocated inside this library and returned to the user.
   It is the responsibility of the users to allocate memory correctly in order to prevent undefined behaviors.

.. note::

   The user should open the file beforehand with a writable mode (such as ``"r"``).

.. note::

   This function returns a value, which represents the size of the header that was successfully loaded from the file stream ``fp``.
   If the function fails, it returns ``0``.
   It is recommended to always check the value returned by the function.

The reading procedure for a ``NPY`` header is handled by the main function ``snpyio_r_header``, which includes the following process:

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
      extract_values:s     -> end                  [label="return header_size"];
      extract_values:e     -> error                [label="failure"]
      error:s              -> end                  [label="return 0"];

   }

**************
Error handling
**************

File-IO operations sometimes fail due to external reasons.
This library gracefully handles these errors without terminating the program by invoking the ``exit()`` function.
In both normal and error exit scenarios, both writer and reader free all internally-allocated memories before returning an error code to the user to prevent memory leaks.

Two exceptions exist: the reader returns the data type (e.g. ``'"<f8"'``) and the shape of the data (e.g. ``[123, 456]``) to the user.
After use, it is important that the user deallocates them properly to prevent memory leaks.

