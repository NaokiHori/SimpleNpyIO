********************************
Implementation - error handlings
********************************

When error is detected, writer and reader should free all allocated memories up to then before returning a corresponding error code to the user, which is treated by this function.

In order to remember all memories allocated, a linked list whose definition is

.. myliteralinclude:: /../../src/simple_npyio.c
   :language: c
   :tag: memory pool storing all allocated pointers in this library

is used.
All buffers are registered to this linked list once a memory allocator ``smt_calloc`` is called, while the information is removed when a deallocator ``smt_free`` is called.

At the end of reader, the data type and shape of the data set are returned to the user, whose memories are allocated by this library since the lengths of these buffers are not known a priori.
Thus, at the end of the reader, these two memories are not freed but returned to the user (after being detached from the memory pool by calling ``smt_detach``), whose deallocations should be done properly by the user.

.. seealso::
   `SimpleMemoryTracker <https://github.com/NaokiHori/SimpleMemTracker>`_

