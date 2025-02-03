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
These are just some of the functions the remote control can perform. By adding the appropriate sensors, many other features could be integrated, such as voice search through a microphone or the inclusion of a touch screen.
<br>

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
<ul>
<li> Texas Instruments MSP432P401R microcontroller </li>
<li> Educational BoosterPack MKII </li>
<li> ESP-EYE (ESP32) </li>
<li> 3 batteries AA (or a 4V5) </li>
<li> Potentiometer </li>
<li> Button </li>
<li> Electrical wires </li>
<li> Insulating tape </li>
<li> Copper or tin </li>
<li> Plywood </li>
</ul>
</p>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- SETTING UP THE HARDWARE -->
## Setting up the hardware
<br>

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>

<!-- PROJECT STRUCTURE -->
## Project structure
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
&nbsp;Contributed on looking for codes that could be useful for the project <br>
Managed the button for the virtual keyboard and the LCD screen <br>
Managed the documentation, presentation and video <br>
MAIL: beatricefaccioli.v@gmail.com
<br> <br>
Filippo Benedetti: <br>
&nbsp;Contributed on the creation of the container for the MSP432P401R and ESP32 <br>
Managed the configuration of the potentiometer <br>
Contributed in the development of a protocol to transmit the values through a socket connection <br>
MAIL: filippo.benedetti3@gmail.com
<br><br>
Lucia Pecora: <br>
&nbsp;Contributed on power supply of the boards <br>
Worked on the configuration of the pins for the right, esc button and LED <br>
Managed the documentation, the presentation and the video <br>
MAIL: luciapecora01@gmail.com
<br> <br>
Matteo Marchiori: <br>
&nbsp;Contributed on searching libraries and code examples in phyton for the interaction with the OS for managing tasks related to socket communication <br>
Worked on the configuration of the joystick movement and left button <br>
Contributed in the development of a protocol to transmit the values through a socket connection <br>
MAIL: matteo.marchiori03@gmail.com
<br>

Project Link: https://github.com/Fb1234566/ERPC.git

<p align="right">(<a href="#readme-erpc">back to top</a>)</p>
