####
Misc
####

************
Installation
************

.. note::

   This step is optional.

For simplicity, I recommend copying the `source file <https://github.com/NaokiHori/SimpleNpyIO/blob/main/src/snpyio.c>`_ and `header file <https://github.com/NaokiHori/SimpleNpyIO/blob/main/include/snpyio.h>`_ directly into your project.
Alternatively, you can build and install this library on your machine using the provided ``install.sh`` script, allowing you to link it as an external library.

First, clone this repository using Git.

To install the library, run:

.. code-block:: console

   bash install.sh install

To uninstall the library, run:

.. code-block:: console

   bash install.sh uninstall

By default, the dynamic library and header file are installed under ``~/.local``.
You can change the installation prefix by modifying the ``install.sh`` script.

Remember to correctly set the paths and link the library, for example:

* Include path: ``-I~/.local/include`` (or set the ``C_INCLUDE_PATH`` environment variable).
* Library path: ``-L~/.local/lib`` (or set the ``LD_LIBRARY_PATH`` environment variable).
* Link with: ``-lsnpyio``.

*******
Logging
*******

Logging (monitoring detailed internal procedures) can be enabled by compiling the library with the ``-DSNPYIO_ENABLE_LOGGING`` flag.

