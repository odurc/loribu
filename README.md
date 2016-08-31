loribu - Lord of the Ring Buffers
=================================

A simple ring buffer library implemented in C. The library can be built to use only static
memory allocation, what make it interesting to employ in microcontrollers projects or other
devices used in embedded system.


Features
---

* no external dependency
* configurable to use only static memory allocation
* allows user to provide external variable as buffer
* useful functions as: read_until, count, search and peek


How to install
---

Simply copy the content of src directory to your work directory.


Configuration
---

The configuration of the library is done by setting 'define' macros in the header file,
under the configuration section.

In order to use only static memory allocation, both 'define' macros need to be declared,
*LORIBU_ONLY_STATIC_ALLOCATION* and *LORIBU_MAX_INSTANCES*.

    #define LORIBU_ONLY_STATIC_ALLOCATION
    #define LORIBU_MAX_INSTANCES    4

The definition *LORIBU_MAX_INSTANCES* is used to set the maximum number of ring buffer instances
that can be requested. As expected all instances are previously allocated as static variables.
Note that only the object instances are statically allocated and not the buffer itself. Which means,
in this setup, the user must to provide the buffer.

The default configuration, i.e. when no 'define' macros is declared (or they are kept commented out),
will use dynamic memory allocation as any regular ring buffer library.


How to use
---

To see details how to use the library, please check the online [API documentation](http://ricardocrudo.github.io/loribu).

* [Manipulation Functions](@ref loribu_manipulation)
* [Information Functions](@ref loribu_information)


License
---

MIT
