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
- TMR2 - 1ms interval, PR2=61, clock source for PWM (OCMP1)
- OCMP1 - PWM output to on-board LED D6

NOTE: controlling PWM is easy. Its period is equal to source timer period
(in our example TMR2 with period 62 => PR2=61 (-1). PWM duty is controlled
with OC2RS register. Here are few examples:
- OC2RS=0  - Duty 0% (LED off)
- OC2RS=31 - Duty 51% 
- OC2RS=61 - (=PR2) - Duty nearly 100% - 1clock tick
- OC2RS=62 - (=PR2+1) - Duty 100%

More information is available on [PIC32 Output Compare data sheet - PDF][PIC32 Output Compare].

> WARNING!
>
> Older datasheets for PIC24 clearly stated that `PRx1` registers
> contain actually `PERIOD+1`, because they start counting from 0,
> but Interrupt/reset is *after* reaching value in `PRx1`. However,
> for some weird reason this important information is missing
> from PIC32 datasheets(!)
> 
> Compare this datasheet where is no useful information on period
> and `PRx` register: [DS61105F][DS61105F]
>
> And look here: [DS70205D][DS70205D] on page 13 where the text is clear:
>
> >  This code generates an interrupt after counting 1000 rising edges in
> > the `T1CK` pin.
> > ```
> > ...
> > PR1 = 999;  // Load the period value
> > ```
>
> This explain why MCC Harmony 5 now sets `PERIOD-1` to `PRx1`
> registers while old MHC Harmony 3 just used `PERIOD` (which is
> wrong)

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
  - notable changes: both Timers have now set PR1 less -1 (!)
  - had to manually create (from other project) file `firmware/src/config/pic32mx_pwm/pic32mx_pwm.mhc/settings.yml` otherwise it was attempting to again and again import
    and convert old MHC Harmony 3 project...

[DS70205D]: https://ww1.microchip.com/downloads/en/DeviceDoc/70205D.pdf
[DS61105F]: http://ww1.microchip.com/downloads/en/devicedoc/61105f.pdf
[PIC32 Output Compare]: http://ww1.microchip.com/downloads/en/devicedoc/61111e.pdf
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
