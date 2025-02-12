<div id="readme-erpc"></div>

<!--TITLE-->
<br />
<div align="center">

<h3 align="center">ERPC</h3>

 <p align="center">
  Essential Remote PC Controller
  <br />
  <br /><br />
  </p>
  </div> 

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol style="counter-reset: section;">
    <li><a href="#project-introduction">Project introduction</a></li>
    <li><a href="#built-with">Built with</a>
    <ol>
    <li><a href="#software-requirements">Software requirements</a></li>
    <li><a href="#hardware-requirements">Hardware requirements</a></li>
    </ol></li>
    <li><a href="#setting-up-the-hardware">Setting up the hardware</a></li>
    <li><a href="#project-structure">Project structure</a></li>
    <li><a href="#functionalities">Functionalities</a></li>
    <li><a href="#user-guide">User guide</a></li>
    <li><a href="#video-and-presentation">Video and presentation</a></li>
    <li><a href="#references">References</a></li>
    <li><a href="#team-members">Team members</a></li>
  </ol>
</details>
<br>
<!--
<style>
    ol{
        list-style-type: none;
    }
    ol > li {
        counter-increment: section;
    }
    ol > li::before {
        content: counter(section) ". "; 
    }
    ol ol > li {
        counter-increment: subsection;
    }
    ol ol > li::before {
        content: counter(section) "." counter(subsection) " ";
    }
    </style> -->



<!-- PROJECT INTRODUCTION -->
## Project introduction
We've all been there at least once, watching a movie on our computer while sitting on the bed. In these moments, simple actions like adjusting the volume or pausing the movie can be annoying. That's why we came up with the ERPC project (Essential Remote PC Control), a device built with the Texas Instruments MSP432P401R board, the educational boosterpack MKII and the ESP32. By using the joystick and some buttons, the remote control can emulate mouse functions, control the computer's volume, open a virtual keyboard and exit full-screen mode. 
<br>These are just some of the functions the remote control can perform. <br>By adding the appropriate sensors, many other features could be integrated, such as voice search through a microphone or the inclusion of a touch screen.
<br>


<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- BUILT WITH -->
## Built with
C <br>  
Python
 
<br>


<h3 style="margin-left: 20px;" id="software-requirements">Software requirements</h3>
<p style="margin-left: 40px;">Software required to program the boards: <br>
 &nbsp; &nbsp; → visual studio code: https://code.visualstudio.com/download <br>
 &nbsp; &nbsp; → code composer studio: https://www.ti.com/tool/download/CCSTUDIO/12.7.0 <br>
 &nbsp; &nbsp; → arduino ide (for ESP32): https://www.arduino.cc/en/software 
</p>

<h3 style="margin-left: 20px;" id="hardware-requirements">Hardware requirements</h3>
<p style="margin-left: 40px;">Hardware components needed to built the device: 

* Texas Instruments MSP432P401R microcontroller 
* Educational BoosterPack MKII 
* ESP-EYE (ESP32) 
* Computer
* Items for supply (3 batteries AA, copper or tin, insulating tape and electrical wires)
* Additional buttons and potentiometer (welded to one perfboard)
* Items for the box (plywood, screws)

</p>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- SETTING UP THE HARDWARE -->
## Setting up the hardware
The installed microcontroller requires a power supply between 3.5V and 5V to function properly. During testing, it was found that 3.5V is not sufficient to power all components. To solve this issue, three 1.5V batteries were connected in series, resulting in a total voltage of 4.5V. The assembly was carried out using electrical tape, copper wires to connect the batteries, and electrical cables to link them to the board’s power input pin.
<br>There is no need to worry about components that support a maximum of 3.5V, as they are equipped with voltage regulation circuits. In fact, the presence of resistors ensures that the input voltage is reduced when necessary.
<br>As for the ESP32, it is powered directly by the board through the 5V output pins, using electrical wires for the connection.
<br>A button and a potentiometer still need to be connected. They are mounted on a breadboard and soldered with tin to ensure greater stability. Finally, the entire circuit has been placed inside a plywood box, making it more convenient to use and transport.

<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- PROJECT STRUCTURE -->
## Project structure
<pre>
ESP32 project
  ├── esp filippo                      
  ├── SERVERPC                   #PC connection
       ├── Server.py               
       └── client_Esempio.py


MSP432P401R project
  ├── targetConfigs				                                     #MSP432 configuration
  ├── main.c					                                     #main for MSP432
  ├── other files					
        ├── HAL_I2C.c, HAL_I2C.h                                                     #I2C connection management and definition     
     	├── HAL_OPT3001.c, HAL_OPT3001.h                                             #LED management and definition 
     	└── msp432p401r.cmd, startup_msp432p401r_ccs.c, system_msp432p401r.c         #linker configuration, interrupt management, mcu startup 

</pre>
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- FUNCTIONALITIES -->
## Functionalities
<p>This project allows us to control a PC remote by using a socket connection between the ESP32 and the computer. The main functionalities of the remote control are:</p>

<ol>
  <li>Initial connection and network setup</li>
  <li>PC control
    <ol>
      <li>Left button: pin P4.1</li>
      <li>Right button: pin P4.2</li>
      <li>Esc button: pin P4.3</li>
      <li>Joystick: cursor movement</li></ol></li>
      <li>Volume control (with a potentiometer)</li>
      <li>Opening the virtual keyboard: pin P4.5</li>
      <li>Connection monitoring (per ora no schermo, quindi omettere questo punto)</li>
</ol>
<br>Upon system startup, the ESP32 establishes a socket connection to the PC by using a WiFi network. Once the communication is established, the commands sent by the MSP432P401R (mouse buttons, volume control, virtual keyboard opening, esc) are received by the ESP32, which transmits them to the computer to perform the desired actions.

<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- USER GUIDE -->
## User guide
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- VIDEO AND PRESENTATION -->
## Video and presentation
Video: 
<br> Presentation: 
<br> Project link: https://github.com/Fb1234566/ERPC.git
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- REFERENCES -->
## References
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>


<!-- TEAM MEMBERS -->
## Team members
Beatrice Faccioli: <br>
&nbsp; &nbsp; - Contributed on looking for codes that could be useful for the project <br>
&nbsp; &nbsp; - Managed the button for the virtual keyboard and the LCD screen <br>
&nbsp; &nbsp; - Managed the documentation, presentation and video <br>
&nbsp; &nbsp; - Mail: beatricefaccioli.v@gmail.com
<br> <br>
Filippo Benedetti: <br>
&nbsp; &nbsp; - Contributed on the creation of the container for the MSP432P401R and ESP32 <br>
&nbsp; &nbsp; - Managed the configuration of the potentiometer <br>
&nbsp; &nbsp; - Contributed in the development of a protocol to transmit the values through a socket connection <br>
&nbsp; &nbsp; - Mail: filippo.benedetti3@gmail.com
<br><br>
Lucia Pecora: <br>
&nbsp; &nbsp; - Contributed on power supply of the boards <br>
&nbsp; &nbsp; - Worked on the configuration of the pins for the right, esc button and LED <br>
&nbsp; &nbsp; - Managed the documentation, the presentation and the video <br>
&nbsp; &nbsp; - Mail: luciapecora01@gmail.com
<br> <br>
Matteo Marchiori: <br>
&nbsp; &nbsp; - Contributed on searching libraries and code examples in python for socket communication <br>
&nbsp; &nbsp; - Worked on the configuration of the joystick movement and left button <br>
&nbsp; &nbsp; - Contributed in the development of a protocol to transmit the values through a socket connection <br>
&nbsp; &nbsp; - Mail: matteo.marchiori03@gmail.com
<br>


<p align="right">(<a href="#readme-erpc">back to top</a>)</p>
