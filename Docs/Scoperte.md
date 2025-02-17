# Documentation on Discoveries Made with the nRF52 DK

## 1. Board Functionality

The nRF52 DK board was successfully configured using the SDK provided by Nordic Semiconductor. We tested the basic Blinky program and confirmed that the LED on the board blinks as expected, demonstrating that the hardware is functional and the minimum configuration is operational.

## 2. Tools Required for Configuration

To configure and use the nRF52 DK board, several tools and software needed to be downloaded and installed. Here's a list of the tools used, along with a brief description of their functions:

- **nRF Connect for Desktop**  
  The main platform for managing Nordic tools. 
  It includes applications like Toolchain Manager, Programmer, and other utilities for development and debugging.

- **nRF Command Line Tools**  
  A set of command-line tools, including nrfjprog, for programming, erasing, and interacting with the board.
  Essential for low-level operations on the board.

- **SEGGER J-Link Software**  
  Software necessary for interfacing with the J-Link debugger integrated into the board.  
  Allows for firmware flashing and debugging via the SWD connection.

- **Visual Studio Code con l'estensione nRF Connect**  
  An integrated development environment (IDE) for writing, compiling, and flashing programs onto the board.  
  Requires additional configuration to connect the Nordic toolchain to the project.

- **CMake e Ninja Build**  
  Tools used by the build system to configure and compile projects.

- **Zephyr RTOS**  
  A real-time operating system integrated into the Nordic SDK to manage complex applications.  
  It requires a steep learning curve for advanced configurations.

## 3. Comparison Between nRF52 DK and ESP-EYE

After completing the configuration and initial tests, several observations emerged from the comparison between the nRF52 DK board and the alternative ESP-EYE:

### Connectivity:

- The nRF52 DK only offers Bluetooth as its primary connectivity option.
- The ESP-EYE includes integrated support for Wi-Fi, which is more suitable for our needs as it allows direct communication via the network without requiring Bluetooth pairing.

### Tools Complexity:

- The nRF52 DK requires a large number of specific tools for configuration, flashing, and debugging.
- In contrast, the ESP-EYE uses the simpler ESP-IDF (Espressif IoT Development Framework), with fewer dependencies and a less steep learning curve.

### Adaptability to the Project:

- The Nordic tools (e.g., nRF Connect, Command Line Tools, and Zephyr) are very powerful and suitable for complex, professional projects.
- However, for our development needs, this complexity is not necessary and presents more of an obstacle than an advantage.

## 4. Conclusion

Despite the potential of the nRF52 DK, we have decided to use the ESP-EYE for our future projects. The main reasons are:

- The presence of Wi-Fi support, which is crucial for our applications.
- The simplicity of the configuration and tools required.
- The lower overall complexity, which allows us to be more productive and focus on software development.

We still thank Nordic Semiconductor for the tools provided, which could be explored further in more complex projects.