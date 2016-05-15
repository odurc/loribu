ringbuff
========

A simple ring buffer library implemented in C. It's aimed to be used in microcontrollers
and other devices with limited resources. However it should work for any system.


Features
---

* no external dependency
* configurable to not use dynamic memory allocation
* possibility to use user provided static buffer
* useful functions as: read_until, count, search and peek


How to install
---

Simply copy the content of src directory to your work directory.


Configuration
---

The configuration of the library is done by setting defines in the header file,
in the configuration section.

If you don't want to use dynamic memory allocation you have to define both
*RINGBUFF_NO_DYNAMIC_ALLOCATION* and *RINGBUFF_NUM_INSTANCES*.

    #define RINGBUFF_NO_DYNAMIC_ALLOCATION
    #define RINGBUFF_NUM_INSTANCES    4

The definition *RINGBUFF_NUM_INSTANCES* is used to set how many instances of ring buffer
object you want to create. You also have the option to use mixed allocation, static and
dynamic, by defining *RINGBUFF_NUM_INSTANCES* but not *RINGBUFF_NO_DYNAMIC_ALLOCATION*.

To use only dynamic memory allocation keep both defines commented out.


How to use
---

To see details how to use the library, please check the API documentation.

* @ref ringbuff_manipulation
* @ref ringbuff_information


License
---

MIT
