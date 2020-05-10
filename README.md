# PIC32MX LED PWM

Introductory project for [Microstick II][PIC Microstick II] 
with PIC32MX.

It does two things:

1. control on-board LED D6 (on OC1, PIN2) brightness from 0% to 100% 
   (and again starting from 0%) using PWM (OCMP1 peripheral)
1. blinks external LED (on PIN3, RA1) with 200ms period - it works
   as primitive "watchdog" that everything is running. Note: this
   LED is not part of [Microstick II][PIC Microstick II]

Used peripherals:
- on-chip oscillator 8MHz scaled down to 4MHz peripheral clock
- TMR1 - 100ms interval, flashing external LED on PIN3 (for debug)
- TMR2 - 1ms interval, PR2=62, clock source for PWM (OCMP1)
- OCMP1 - PWM output to on-board LED D6

NOTE: controlling PWM is easy. Its period is equal to source timer period
(in our example TMR2 with period PR2=62). PWM duty is controlled
with OC2RS register. Here are few examples:
- OC2RS=0  - Duty 0% (LED off)
- OC2RS=31 - Duty 51% 
- OC2RS=62 - (=PR2) - Duty nearly 100% - 1clock tick
- OC2RS=64 - (=PR2+1) - Duty 100%

More information is available on [PIC32 Output Compare][PIC32 Output Compare Data Sheet].

# Requirements

## Hardware requirements

* [Microstick II][PIC Microstick II]  demo board
* [PIC32MX250F128B SPDIP][PIC32MX250F128B] inserted into socket U5
  (included with board, should be default)
* programming pins switch S1 in position `A`

## Software requirements

* [XC32 compiler][XC compilers] - tested version v2.30
* [MPLAB X IDE][MPLAB X IDE] - tested version v5.25
  - installed  [MPLAB Harmony Configurator 3 Plugin][Harmony] - tested
    version 3.3.0.1

[PIC32 Output Compare]: http://ww1.microchip.com/downloads/en/devicedoc/61111e.pdf
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
