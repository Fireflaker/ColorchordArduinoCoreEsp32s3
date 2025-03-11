# ColorchordArduinoCoreEsp32s3
Demacritize. Thank you to https://github.com/cnlohr/colorchord. It's pretty

first build pass Mar 6 2025 4pm

go follow https://github.com/Charlatt :)

## Current state
it is currently built for esp32s3 but passes compile for esp32 and probably a lot of other boards.
there is a version for i2s pdm microphone and one for adc input on pin 1. interrupt safe.
please contribute because it is very much a work in progress

Demo: https://youtu.be/9l3OOd5NJVg

Major improvement over esp8266 embedded is 1. 240mhz dual core vs 80mhz single core. 2. support more than 255 leds. 3. compatable with arduino libraries and use fastled for ws2812 - so it is easy to support other addressable leds like rgb-yellow 4 color hue wheel

## Get started
get vs code and install platform io, open folder of choice, click the -> arrow.
tinker and post your improvements here
ask chat for config help

### If you want multiple simultaneous audio outputs on windows computer with better control
Download voicemeter banana or similar audio routing tool
Set up A1 as your speaker, A2 as colorchord, and more as needed
Go to MENU -> System settings -> delay - and type in approperate audioStreamDelay in ms.

## license
same license copied from charles' repo
Copyright (c) 2012-2022, Charles Lohr (CNLohr), et. al.

This license covers the core of ColorChord, but not the ESP8266 or STM32
embedded builds, for them, they have a separate license.

All major contributors have agreed to allowing ColorChord to be distributed
in either CC0, MIT-x11 or NewBSD license terms as of September, 2022.
Depending on the user's preferences and needs.  You may feel free to copy
and use ColorChord in your project under ther terms listed below (NewBSD
license) or any of the afore-mentioned licenses.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Any devices that implement ColorChord, or significant subportions have some
  indication on device or associated materials includes mention of ColorChord.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

