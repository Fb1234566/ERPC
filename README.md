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
    <li><a href="#software requirements">Software requirements</a></li>
    <li><a href="#hardware-requirements">Hardware requirements</a></li>
    </ol></li>
    <li><a href="#setting-up-the-hardware">Setting up the hardware</a></li>
    <li><a href="#project-structure">Project structure</a></li>
    <li><a href="#functionalities">Functionalities</a></li>
    <li><a href="#user-guide">User guide</a></li>
    <li><a href="#video-and-presentation">Video and presentation</a></li>
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
We've all been there at least once, watching a movie on our computer while sitting on the bed. In these moments, simple actions like adjusting the volume or pausing the movie can be annoying. That's why we came up with the ERPC project (Essential Remote PC Control), a device built with the Texas Instruments MSP432P401R board, the educational boosterpack MKII and the ESP32. By using the joystick and some buttons, the remote control can emulate mouse functions, control the computer's volume, and exit full-screen mode (ESC). 
These are just some of the functions the remote control can perform. By adding the appropriate sensors, many other features could be integrated, such as voice search through a microphone or the inclusion of a touch screen. <br>
<br>
Project Link: https://github.com/Fb1234566/ERPC.git <br>


<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- BUILT WITH -->
## Built with
C <br>  
Python
 
<br>


<h3 style="margin-left: 20px;" id="software-requirements">Software requirements</h3>
<p style="margin-left: 40px;">Software required to program the boards: <br>
  → visual studio code: https://code.visualstudio.com/download <br>
  → code composer studio: https://www.ti.com/tool/download/CCSTUDIO/12.7.0 <br>
  → arduino id (per ESP32): https://www.arduino.cc/en/software 
</p>

<h3 style="margin-left: 20px;" id="hardware-requirements">Hardware requirements</h3>
<p style="margin-left: 40px;">Hardware components needed to built the device: 

* Texas Instruments MSP432P401R microcontroller 
* Educational BoosterPack MKII 
* ESP-EYE (ESP32) 
* 3 batteries AA (or a 4V5) 
* Potentiometer 
* Button 
* Electrical wires
* Insulating tape 
* Copper or tin 
* Plywood 
</p>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- SETTING UP THE HARDWARE -->
## Setting up the hardware
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- PROJECT STRUCTURE -->
## Project structure
MSP432_Project
    ├── Debug                  # Compiled files 
    ├── Hardware               # Hardware related files
    │   ├── Wifi               # Manages Wifi connection
    │   ├── Graphics           # Manages all the graphics required
    │   └── Lcd                # Manages the LCD setup
    ├── Logic                  # Control logic and program testing
    │   └── Interrupts         # Contains all the interrupts required
    ├── Release
    ├── targetConfigs          # Target configuration files
    ├── main.c                 # Main source code
    └── README.md

<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- FUNCTIONALITIES -->
## Functionalities
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- USER GUIDE -->
## User guide
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- VIDEO AND PRESENTATION -->
## Video and presentation
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
&nbsp; &nbsp; - Contributed on searching libraries and code examples in phyton for socket communication <br>
&nbsp; &nbsp; - Worked on the configuration of the joystick movement and left button <br>
&nbsp; &nbsp; - Contributed in the development of a protocol to transmit the values through a socket connection <br>
&nbsp; &nbsp; - Mail: matteo.marchiori03@gmail.com
<br>


<p align="right">(<a href="#readme-erpc">back to top</a>)</p>
