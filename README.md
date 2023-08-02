STM32 Toolbox
=============

Executive Summary
-----------------

STM32 Toolbox (aka `stm32-toolbox`) is a collection of C++ classes intended to support rapid development of firmware
for STM32 microcontrollers. Its focus is on keeping code as small as possible to allow sophisticated functionality
on MCUs with limited ROM and RAM resources. It also includes C++ wrappers for commonly used features allowing 
programmers to focus on functionality instead of constantly referencing HAL documentation.

Overview
--------

STM32 Toolbox (aka `stm32-toolbox`) is a collection of C++ classes intended to support rapid development of firmware
for STM32 microcontrollers. Its focus is on keeping code as small as possible to allow sophisticated functionality
on MCUs with limited ROM and RAM resources. It does this by

* minimizing the number of dependencies on standard libraries that can add significant bloat. For example, including
  printf, especially with floating-point support can add 10kB or more to the binary footprint.
* offering reduced functionality by excluding features that are less used or require significant resources.
* not enrolling the Arduino ecosystem, with its many mandatory dependencies.

Almost all classes run on bare metal, but are compatible with FreeRTOS, and probably other RTOS'.

The functionality offered by the toolbox is quite varied, and increasing everyday. At the same time, the maturity of
each class is directly proportional to how often it was used by the author, since for the most part these have been
developed for specific projects with the hope of future reuse.

Since the risk that dynamic memory allocation (`malloc`, `new`) is unacceptable for many applications, it is easy to
completely exclude classes depending on `malloc` or to offer alternative functionality that uses user-supplied buffers.

Classes build upon the STM32 HAL library, and typically assume that you have preconfigured devices. Often a device
handle is passed to the constructor of a class; for example the `CanBus` class expects an `hcan` instance handle.

(Almost) all of the classes are based around the C++ concept of inlining, basically each class lives entirely in a
header file, with functionality implemented by methods inside the class. While many purists will turn up their noses,
I chose this strategy because:

* One file is easier to handle/digest/understand than two for someone trying to understand what a class does and how it
  works.
* The argument for two files was to keep the header "clean" for declaration and the implementation in a separate file.
  My background as a C# programmer biases me toward thinking you can do everything in a single file and still keep it
  clean.
* Code is only compiled if you use (#include) it. You don't have to manage a directory of files and hope that if you
  missed something, the compiler will optimize out any unused code.


Usage
-----

You can copy individual classes to your project, but they easiest way it to `git submodule add <repo-url>`. This allows
you to easily keep the libraries up-to-date.

Some classes refer to `toolbox.h` for configuration parameters.

Classes
-------

Each of the classes is listed here by category. In time, each class will get its own documentation that includes
usage, example, and dependencies.

### Utility

`PrintLite` is an abstract class for adding `printf`-style functionality to classes that output text, such as serial
communications, LED and LCD displays, logging, HTTP output, etc.

`Rtc` abstracts the system real-time clock, allowing it to be read and set, either as a date or time structure or
as a UNIX timestamp.

`Timer` is a general purpose microsecond timer for timing how long something takes to execute, executing code at
regular intervals, and returning to execution after an amount of time. By using the processor's DWT (data watchpoint
timer), it does not require configuration or use of one of the system's timers.

`Tokenism` enables searching of strings for tokens. For example given a string and one or more delimiters it can return
the 5th token, the token following the "filename" token, whether the 3rd token is "goat", or whether the token "ugly"
is present at all.

`StringBuilder` allows you to build a string in steps or stages with `printf`-like functionaity.

### Generics

`List` implements a list of a specified type. You can add items, request an item at a specific index, and iterate
through the list.

`Queue` implements a FIFO queue of a specified type. You can add items to the end of the queue, dequeue them from the 
front of the queue, and peek at members of the queue.

`Ring` implements a ring buffer of a specified type. It maintains state, so you can used `previous()`, `current()`,
and `next()` syntax to navigate the items in the ring.

### Diagnostics

`Watchdog` is an easy-to-use implementation of a watchdog that thinly veils the controllers IWDG (independent
watchdog).

`Log` allows logging to `Serial` device using `printf`-style notation and priority assignment. It can optionally be
completely excluded from release builds.

`Fault` allows firmware to enumerate all possible faults, and have its code maintain and report fault states.
Optionally an `Led` can be associated with the class and be illuminated when a fault is present.

### Communications

`Serial` is a UART abstraction, offering `printf`-style syntax.

`SPI` is an SPI abstraction, allowing simple reading and writing with most devices.

`OneWire` implements the Dallas Semiconduction 1-wire interface.

`CanBus` is a CAN abstraction allowing the programmer to quickly be in communication with CAN devices.

`CanOpen` leverages `CanBus` to implement an interface with devices implementing CanOpen (CiA) protocols, including
nmt, pdo, sdo, and tpdo. Classes can implement `ICanOpenCallback` to implement asychronous message handling without
polling.

`ethernet/*` is a large collection of classes to implement a TCP/IP stack on top of W5500 hardware. This is all
borrowed from Arduino-land, except I have gone though and removed most of the dependencies on the Arduino ecosystem.
I fully intend to rewrite all of this to make it leaner and fix the messy confusion of OSI layers in which concepts
including PHY, sockets, ICMP, TCP/IP, TCP and UDP, Ethernet, IP addressing, and application layer are all
interwoven. Sure the W5500 implements a TCP/IP stack but, especially in C++, one can still keep the OSI layers
mostly distinct.

`http/*` is my implementation of an `HttpServer` and `HttpHandler` which makes it easy to implement an application
or API layer based on URI query string parsing.


### Basic Devices

`devices/basic/Pwm` aids configuration of a PWM timer and is inherited by some other classes.

`devices/basic/Led` is a simple LED abstraction. It knows whether your logic is inverted (`false` state is *ON*). 
You can `set()` the device's state, turn it `on()` or `off()`, or `flip()` it's state.

`devices/basic/Relay` is a simple relay abstraction, however it is particularly useful for contactors or relays in 
which you want to
monitor secondary contacts for feedback.

`devices/basic/Encoder` implements a simple pushbutton/rotary encoder device.

### Batteries

`devices/batteries/Nec12V35i` interfaces with an NEC 12V35i battery that communicates using CANOpen. 
This is probably broken.

`devices/batteries/Inventus` interfaces the the PROTRXion line of batteries from Inventus.

### Internal Memory Devices

`devices/flash/internal/FlashMemory` is an abstract class for reading and writing the FLASH memory that is embedded 
into STM32 MCUs.

`devices/flash/internal/Sector` is implementation of `FlashMemory` specifically for those MCUs that organize memory 
into sectors.

`devices/flash/internal/FlashFileSystem` is an abstract class for organizing FLASH memory into a simple filesystem, 
using `Directory`, `DirectoryEntry`, and `DirectoryHeader`.

`devices/flash/internal/SectorFlashFileSystem` is an implementation of a filesystem for MCUs that organize FLASH memory
into sectors.

`devices/flash/internal/OneTimeProgrammable` is an interface to read and write one-time programmable memory.

### External Memory Devices

`devices/flash/external/SpiFlashMemory` is an interface to read and write commodity SPI NOR FLASH memory.
`devices/flash/external/SpiFlashMemoryFileSystem` allows SPI flash memory to be used like a filesystem.

### Displays

`devices/displays/Hd44780` is an interface to write to commodity Hitachi 2-line or 4-line alphanumeric displays.

`devices/displays/OledSsd1306` is an interface to write to commodity 320x240 pixel displays.

`devices/displays/NeoPixel` is an interface to output to addressible RGB and RGBW LEDs.

`devices/displays/Ili9488` is an interface to write to 3 to 4 inch RGB TFT LCD panels. It includes some basic drawing
functionality, but is intended to be used with the `PicoGFX` class.

### Graphics

`PicoFGX` is a minimalist graphics drawing library for drawing lines, rectangles, circles, text, etc. to a dot matrix
display. It is designed to be hardware agnostic. As long as you can write an interface that turns on a pixel for any
hardware, you can hook it up to `PicoGFX` and draw to that canvas.


Class Summary
-------------

This table lists the classes included in the library, the last time they were updated, and their subjective maturity.



Class                        | Functionality                         | Size    | Updated   | Maturity
-----------------------------|---------------------------------------|---------|-----------|-------------