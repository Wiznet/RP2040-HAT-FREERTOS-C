# Getting Started with FreeRTOS Examples

These sections will guide you through a series of steps from configuring development environment to running FreeRTOS examples using the **WIZnet's ethernet products**.

- [**Development environment configuration**](#development_environment_configuration)
- [**Hardware requirements**](#hardware_requirements)
- [**FreeRTOS example structure**](#freertos_example_structure)
- [**FreeRTOS example testing**](#freertos_example_testing)
- [**How to use port directory**](#how_to_use_port_directory)



<a name="development_environment_configuration"></a>
## Development environment configuration

To test the FreeRTOS examples, the development environment must be configured to use Raspberry Pi Pico, W5100S-EVB-Pico or W5500-EVB-Pico.

The FreeRTOS examples were tested by configuring the development environment for **Windows**. Please refer to the '**9.2. Building on MS Windows**' section of '**Getting started with Raspberry Pi Pico**' document below and configure accordingly.

- [**Getting started with Raspberry Pi Pico**][link-getting_started_with_raspberry_pi_pico]

**Visual Studio Code** was used during development and testing of FreeRTOS examples, the guide document in each directory was prepared also base on development with Visual Studio Code. Please refer to corresponding document.



<a name="hardware_requirements"></a>
## Hardware requirements

The FreeRTOS examples use **Raspberry Pi Pico** and **WIZnet Ethernet HAT** - ethernet I/O module built on WIZnet's [**W5100S**][link-w5100s] ethernet chip, **W5100S-EVB-Pico** - ethernet I/O module built on [**RP2040**][link-rp2040] and WIZnet's [**W5100S**][link-w5100s] ethernet chip or **W5500-EVB-Pico** - ethernet I/O module built on [**RP2040**][link-rp2040] and WIZnet's [**W5500**][link-w5500] ethernet chip.

- [**Raspberry Pi Pico**][link-raspberry_pi_pico]

<p align="center"><img src="https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/raspberry_pi_pico_main.png"></p>

- [**WIZnet Ethernet HAT**][link-wiznet_ethernet_hat]

<p align="center"><img src="https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/wiznet_ethernet_hat_main.png"></p>

- [**W5100S-EVB-Pico**][link-w5100s-evb-pico]

<p align="center"><img src="https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/w5100s-evb-pico_main.png"></p>

- [**W5500-EVB-Pico**][link-w5500-evb-pico]

<p align="center"><img src="https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/w5500-evb-pico_main.png"></p>



<a name="freertos_example_structure"></a>
## FreeRTOS example structure

Examples are available at '**RP2040-HAT-FREERTOS-C/examples/**' directory. As of now, following examples are provided.

- [**DHCP & DNS**][link-dhcp_dns]
- [**MQTT**][link-mqtt]
- [**TCP Client over SSL**][link-tcp_client_over_ssl]

Note that **ioLibrary_Driver**, **FreeRTOS-Kernel**, **mbedtls**, **pico-sdk** are needed to run FreeRTOS examples.

- **ioLibrary_Driver** library is applicable to WIZnet's W5x00 ethernet chip.
- **FreeRTOS-Kernel** is a real-time operating system kernel for embedded devices that contains FreeRTOS kernel source/header files and kernel ports only.
- **mbedtls** library supports additional algorithms and support related to SSL and TLS connections.
- **pico-sdk** is made available by Pico to enable developers to build software applications for the Pico platform.
- **pico-extras** has additional libraries that are not yet ready for inclusion the Pico SDK proper, or are just useful but don't necessarily belong in the Pico SDK.

Libraries are located in the '**RP2040-HAT-FREERTOS-C/libraries/**' directory.

- [**ioLibrary_Driver**][link-iolibrary_driver]
- [**FreeRTOS-Kernel**][link-freertos_kernel]
- [**mbedtls**][link-mbedtls]
- [**pico-sdk**][link-pico_sdk]
- [**pico-extras**][link-pico_extras]

If you want to modify the code that MCU-dependent and use a MCU other than **RP2040**, you can modify it in the **RP2040-HAT-FREERTOS-C/port/** directory.

port is located in the '**RP2040-HAT-FREERTOS-C/port/**' directory.

- [**ioLibrary_Driver**][link-port_iolibrary_driver]
- [**FreeRTOS-Kernel**][link-port_freertos_kernel]
- [**mbedtls**][link-port_mbedtls]
- [**timer**][link-port_timer]



<a name="freertos_example_testing"></a>
## FreeRTOS example testing

1. Download

If the FreeRTOS examples are cloned, the library set as a submodule is an empty directory. Therefore, if you want to download the library set as a submodule together, clone the FreeRTOS examples with the following Git command.

```cpp
/* Change directory */
// change to the directory to clone
cd [user path]

// e.g.
cd D:/RP2040

/* Clone */
git clone --recurse-submodules https://github.com/Wiznet/RP2040-HAT-FREERTOS-C.git
```

With Visual Studio Code, the library set as a submodule is automatically downloaded, so it doesn't matter whether the library set as a submodule is an empty directory or not, so refer to it.

2. Setup ethetnet chip

Setup the ethernet chip in '**CMakeLists.txt**' in '**RP2040-HAT-FREERTOS-C/**' directory according to the evaluation board to be used referring to the following.

- WIZnet Ethernet HAT : W5100S
- W5100S-EVB-Pico : W5100S
- W5500-EVB-Pico : W5500

For example, when using WIZnet Ethernet HAT or W5100S-EVB-Pico :

```cpp
# Set ethernet chip
set(WIZNET_CHIP W5100S)
```

When using W5500-EVB-Pico :

```cpp
# Set ethernet chip
set(WIZNET_CHIP W5500)
```

3. Test

Please refer to 'README.md' in each example directory to find detail guide for testing FreeRTOS examples.

<!--
Link
-->

[link-getting_started_with_raspberry_pi_pico]: https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf
[link-rp2040]: https://www.raspberrypi.org/products/rp2040/
[link-w5100s]: https://docs.wiznet.io/Product/iEthernet/W5100S/overview
[link-w5500]: https://docs.wiznet.io/Product/iEthernet/W5500/overview
[link-raspberry_pi_pico]: https://www.raspberrypi.org/products/raspberry-pi-pico/
[link-raspberry_pi_pico_main]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/raspberry_pi_pico_main.png
[link-wiznet_ethernet_hat]: https://docs.wiznet.io/Product/Open-Source-Hardware/wiznet_ethernet_hat
[link-wiznet_ethernet_hat_main]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/wiznet_ethernet_hat_main.png
[link-w5100s-evb-pico]: https://docs.wiznet.io/Product/iEthernet/W5100S/w5100s-evb-pico
[link-w5100s-evb-pico_main]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/w5100s-evb-pico_main.png
[link-w5500-evb-pico]: https://docs.wiznet.io/Product/iEthernet/W5500/w5500-evb-pico
[link-w5500-evb-pico_main]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/blob/main/static/images/getting_started/w5500-evb-pico_main.png
[link-dhcp_dns]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/examples/dhcp_dns
[link-mqtt]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/examples/mqtt
[link-tcp_client_over_ssl]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/examples/tcp_client_over_ssl
[link-iolibrary_driver]: https://github.com/Wiznet/ioLibrary_Driver
[link-freertos_kernel]: https://github.com/FreeRTOS/FreeRTOS-Kernel
[link-mbedtls]: https://github.com/ARMmbed/mbedtls
[link-pico_sdk]: https://github.com/raspberrypi/pico-sdk
[link-pico_extras]: https://github.com/raspberrypi/pico-extras
[link-port_iolibrary_driver]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/port/ioLibrary_Driver
[link-port_freertos_kernel]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/port/FreeRTOS-Kernel
[link-port_mbedtls]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/port/mbedtls
[link-port_timer]: https://github.com/Wiznet/RP2040-HAT-FREERTOS-C/tree/main/port/timer
