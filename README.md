# PIC32MX LED PWM

Introductory project for [Microstick II][PIC Microstick II] 
with PIC32MX.

It glows on-board LED using PWM.

Here is example output from [Digilent AD2][Digilent AD2] scope/analyzer:

![PWM Example output](digilentad2/PIC32MX-PWM-TMR2-OC1-BUS.gif)

There are these important signals:
- Yellow Channel 1: Toggles on Timer1 interrupt where we change `OC1RS`, in our
  example it changed from 15 to 20 (see decoded number on Bus)
- Blue CHannel 2 is actual PWM output to on-board LED. You can see various measures on 
  the measurements tab - PWM frequency is 1 kHz, and current duty (coresponding to OC1RS=15) is 27%
- `TMR2_PER` (on digital output only) is whole PWM period (normally set to 61) and it defines
   1 kHz PWM frequency (`PR1` timer register)


And here is Workspace file for [Digilent WaveForms][Digilent WaveForms]  software:
- [digilentad2/PIC32MX-PWM-TMR2-OC1-BUS.dwf3work](digilentad2/PIC32MX-PWM-TMR2-OC1-BUS.dwf3work)


Used peripherals:
- on-chip oscillator FRC 8MHz scaled down to 4MHz using `FRCDIV` by 2,
  so `SYSCLK` and `PBCLK` is 4 Mhz - for easy probing using affordable
  scope - in my case [Digilent AD2][Digilent AD2]
- TMR1 - 100ms interval, flashing external LED on PIN3 (for debug) and increasing
  PWM duty (causing glowing effect on LED)
- TMR2 - 1ms interval, PR2=61, clock source for PWM (OCMP1)
- OC1 - PWM output to on-board LED D6 on Pin 2 (primary function RA0)
- we also send current value of `OC1RS` register to "data bus" using RB7,8,9,14,14,15 GPIO pins

> WARNING!
>
> I intended to use PMP (Parallel Master Port) interface for this data-bus, but was really annoyed by
> Chip errata, where PMP completely overtakes all Address Pins, even when they are configured
> in GPIO mode in PTEN register(!)
>
> Other problem is that PMD0 and PMD1 (Data 0 and Data 1 bits) signals conflict with Programmer/Debugger
> pins PGED1 and PGEC1.  Programming switch does not help, because alternative PGDx,PGCx pins are
> on unsupported location (these alternate pins are used for USB signals by this chip and are not
> available for Programmer/Debugger)

Used Pins on [Microstick II][PIC Microstick II]:

| Pin | Signal | Note |
| --- | --- | --- |
| 1 | /MCLR | Reset + Programmer/Debugger |
| 2 | OC1/RA0 | PWM - red LED |
| 3 | RA1 | TMR1 - Debug LED |
| 4 | PGED1 | Data for Programmer/Debugger |
| 5 | PGEC1 | Clock for Programmer/Debugger |
| 9 | RA2  | TMR2/PWM period Toggle |
| 10 | CLKO/RA3 | PBCLK Output |
| 16 | RB7 | DATA0 (output of `OC1RS`) |
| 17 | RB8 | DATA1 |
| 18 | RB9 | DATA2 |
| 24 | RB13 | DATA3 |
| 25 | RB14 | DATA4 |
| 26 | RB15 | DATA5 |

Pins 16 to 26 are useful to show real OC1RS value on scope/analyzer.

Controlling PWM is easy. Its period is equal to source timer period
(in our example TMR2 with period 62 => PR2=61 (-1). PWM duty is controlled
with OC2RS register. Here are few examples:
- OC2RS=0  - Duty 0% (LED off)
- OC2RS=31 - Duty 51% 
- OC2RS=61 - (=PR2) - Duty nearly 100% - 1clock tick
- OC2RS=62 - (=PR2+1) - Duty 100%

More information is available on [PIC32 Output Compare data sheet - PDF][PIC32 Output Compare].

> WARNING!
>
> Older datasheets for PIC24 clearly stated that `PRx` registers
> contain actually `PERIOD+1`, because they start counting from 0,
> but Interrupt/Reset occurs *after* reaching value in `PRx` (period
> compare registers *match* triggers on next clock edge). However,
> for some weird reason this important information is missing
> from PIC32 datasheets(!)
> 
> Compare this datasheet where is no useful information on period
> and `PRx` register: [DS61105F][DS61105F] (they use careful wording
> "TMR count register matches the PR period" without clear explanation)
>
> And look here: [DS70205D][DS70205D] on page 13 where the text is clear:
>
> >  This code generates an interrupt after counting 1000 rising edges in
> > the `T1CK` pin.
> >
> > ```
> > ...
> > PR1 = 999;  // Load the period value
> > ```
>
> This explains why `MCC Harmony 5` now sets `PERIOD-1` to `PRx`
> registers while old `MHC Harmony 3` just used `PERIOD` (which is
> wrong)

There is now After build hook generating assembler listing .lst file under:
```
firmware\pic32mx-pwm.X\dist\pic32mx_pwm\BUILD_TYPE\pic32mx-pwm.X.BUILD_TYPE.lst
```
Where `BUILD_TYPE` is `debug` or `production`.

# Verification

Here is system clock 4MHz measured on `PIN10` `CLKO`  with [Digilent AD2][Digilent AD2]
using BNC probe with 10x attenuation:

![CLKO 4 MHz](digilentad2/PIC32MX-PWM-CLKO.gif)

And here is Workspace file for [Digilent WaveForms][Digilent WaveForms]  software:
- [digilentad2/PIC32MX-PWM-CLKO.dwf3work](digilentad2/PIC32MX-PWM-CLKO.dwf3work)

Please note that even 4 Mhz clock is quite high and its testing this 100 Mhz
scope to its limits. It is main reason why I did not use 48 MHz system clock
(which is now default in MCC Tool), because it is practically impossible to
measure such clock without suitable Counter/Frequency meter.

# Requirements

## Hardware requirements

* [Microstick II][PIC Microstick II]  demo board
* [PIC32MX250F128B SPDIP][PIC32MX250F128B] inserted into socket U5
  (included with board, should be default)
* programming pins switch S1 in position `A`

## Software requirements

* [XC32 compiler][XC compilers] - tested version v4.30
* [MPLAB X IDE][MPLAB X IDE] - tested version v6.15
  - code regenerated with MCC Harmony (was MHC Harmony 3)
  - notable changes: both Timers have now set `PRx` less -1 (!) - see
    above notes
  - had to manually create (from other project) file `firmware/src/config/pic32mx_pwm/pic32mx_pwm.mhc/settings.yml` otherwise it was attempting to again and again import
    and convert old MHC Harmony 3 project...

# Notes

PIC32MX is 32-bit `MIPS32 M4K` based CPU Core with peripherals from Microchip.
Please see
- datasheet: [PIC32MX250F128B][PIC32MX250F128B]
- official splash page: https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/32-bit-mcus/pic32-32-bit-mcus/pic32mx
- [MIPS32 M4K Manual][MIPS32 M4K Manual] from mips.com
- [MIPS32 M4K Core Datasheet][MIPS32 M4K DTS] from mips.com
- [MIPS32 Instruction Set][MIPS32 BIS] from mips.com
- [MIPS32 Instruction Set Quick Reference][MIPS32 QRC] from mips.com

[MIPS32 M4K DTS]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00247-2B-M4K-DTS-02.01.pdf
[MIPS32 BIS]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-05.04.pdf
[MIPS32 QRC]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00565-2B-MIPS32-QRC-01.01.pdf
[MIPS32 M4K Manual]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00249-2B-M4K-SUM-02.03.pdf
[DS70205D]: https://ww1.microchip.com/downloads/en/DeviceDoc/70205D.pdf
[DS61105F]: http://ww1.microchip.com/downloads/en/devicedoc/61105f.pdf
[PIC32 Output Compare]: http://ww1.microchip.com/downloads/en/devicedoc/61111e.pdf
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
[Digilent AD2]: https://digilent.com/shop/analog-discovery-2-100ms-s-usb-oscilloscope-logic-analyzer-and-variable-power-supply/
[Digilent WaveForms]: https://digilent.com/shop/software/digilent-waveforms/
