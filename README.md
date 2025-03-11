# ColorchordArduinoCoreEsp32s3
Demacritize. Thank you to https://github.com/cnlohr/colorchord. It's pretty. Go star his repo and check out his youtube channel.

first build pass Mar 6 2025 4pm

go follow https://github.com/Charlatt :)

## Current state
it is currently built for esp32s3 but passes compile for esp32 and probably a lot of other boards.
there is a version for i2s pdm microphone and one for adc input on pin 1. interrupt safe.
please contribute because it is very much a work in progress

Demo: https://youtu.be/9l3OOd5NJVg

Major improvement over esp8266 embedded is 1. 240mhz dual core vs 80mhz single core. 2. support more than 255 leds. 3. compatable with arduino libraries and use fastled for ws2812 - so it is easy to support other addressable leds like rgb-yellow 4 color hue wheel

## Get started
get vs code and install platform io, open folder of choice(downloaded from this repo above), it should auto-include all libraries so all you need is just click the -> arrow.
tinker and post your improvements here
ask chat for help

### If you want multiple simultaneous audio outputs on windows computer with better control
Download voicemeter banana or similar audio routing tool
Set up A1 as your speaker, A2 as colorchord, and more as needed
Go to MENU -> System settings -> delay - and type in approperate audioStreamDelay in ms.

## Note to self
### Don't cook your microcontroller
### Add a few strong blue or green led between ADC pin 1 and GND as overvoltage protection. If not - you *will* fry your micro.  
Also, Do not attempt to parallel off signal from your exsisting audio setup such as a speaker. If not - you *will* fry your micro. Always use mic or VoiceMeter windows app for audio stream dulication. If you're set to use exsisting audio, the input signal must be either a differencial amplifier or galvanicly isolated with a transformer. LED Vcc should be on a different power supply. Idealy your micro is powered by a seperate usb power adapter.
### Should Read
1. it is very easy to crash due to race condition by refreshing LEDs faster than the transmission time of ws2812b led protocal - 300 leds is alreay maxed *below* 120fps(which is 16khz sampling, 1024 sample, 400Khz div clock the way it is rn). Loop and interrupts can - and must - run well above 300 fps. For that to work, we shall run wires to the middle of the led strip and drive the whole strip as 3 seperate strips of 100 leds, for example. Faliure looks like crazy jumping led color looping output or looping for a few seconds and get stuck and crash - even when input is constent zero. This is however easy to implement since we are already on FastLED library.
   Note this has nothing to do with CPU time. The leds bits are managed asyncronously. ESP32S3 has 8 dedicated RMTs supporting 8 strips in hardware - FASTled Library seems to have managed to make that number 16.
   Realistic max pixels supported to run 120fps is around 1000 for 8 strips or 2000 for 16 strips - if no overclock. It might have just been me but overclocking LEDs seems to kill ws2812b as well as ws2811 chips.
2. When prototyping, constently check output with pure frequencies: https://www.szynalski.com/tone-generator/
3. Timing for adc interrupt needs to be validated. I will setup a oscilioscope test for real world timings.
4. Right now the i2s mic version seems to have bad artifacts for unknown reasons - i.e. jumpy colors when input pure tones. Still need investigation.
5. Right now the ADC mic version seems to do best with 15-900Hz range - not right. Possible reason is any faster settings with sampling cause 300LEDs to race - solution is split strip add injection data wires.
6. Download a spectrometer and observe useful data range in the music you listen to: 100Hz to 4000Hz. Wider may actully make it worse.
7. Make it run RGB-Yellow asap.

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

