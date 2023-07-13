####
Misc
####

************
Installation
************

.. note::

   This is optional.

I recommend copying a source and a header to your project to make things as simple as possible.
However, you can build and install this library on your machine using the script ``install.sh``, so that you can link this library externally.

Checkout this repository using Git.

To install,

.. code-block:: console

   bash install.sh install

To uninstall,

.. code-block:: console

   bash install.sh uninstall

By default, a dynamic library and a header file are installed under ``~/.local``.
You can change the prefix by modifying ``install.sh``.

Remember to set paths and link it properly, e.g. ``-I~/.local/include`` (or set ``C_INCLUDE_PATH``) and ``-L~/.local/lib`` (or set ``LD_LIBRARY_PATH``) with ``-lsnpyio``.

*******
Logging
*******

Logging can be enabled by compiling with ``-DSNPYIO_ENABLE_LOGGING``.

