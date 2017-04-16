EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:stm32f103c8t6-module-china
LIBS:FASTUSBaspAdapter-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "FASTUSBasp Adapter Board"
Date "2017-04-16"
Rev "0.1"
Comp "Amitesh Singh"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L stm32f103c8t6-module-china U1
U 1 1 58F3343D
P 6250 3800
F 0 "U1" H 6250 5150 60  0000 C CNN
F 1 "stm32f103c8t6-module-china" V 6250 3800 60  0000 C CNN
F 2 "myelin-kicad:stm32f103c8t6-module-china" H 6250 2100 60  0001 C CNN
F 3 "" H 5850 4050 60  0000 C CNN
	1    6250 3800
	1    0    0    -1  
$EndComp
NoConn ~ 5200 3150
NoConn ~ 5200 3300
NoConn ~ 5200 3450
NoConn ~ 5200 3600
NoConn ~ 5200 3750
NoConn ~ 5200 3900
NoConn ~ 5200 4050
NoConn ~ 5200 4200
NoConn ~ 5200 4350
NoConn ~ 5200 4500
NoConn ~ 5200 4650
NoConn ~ 5200 4800
NoConn ~ 5200 4950
NoConn ~ 7300 4050
NoConn ~ 7300 4200
NoConn ~ 7300 4350
NoConn ~ 7300 4500
NoConn ~ 7300 4650
NoConn ~ 7300 4800
NoConn ~ 7300 4950
NoConn ~ 7300 5100
NoConn ~ 7300 5250
NoConn ~ 7300 3000
NoConn ~ 7300 2850
NoConn ~ 7300 2400
NoConn ~ 7300 3150
NoConn ~ 7300 3300
NoConn ~ 7300 3450
NoConn ~ 5200 2400
NoConn ~ 5200 2550
NoConn ~ 5200 2700
NoConn ~ 5200 2850
NoConn ~ 7300 2550
NoConn ~ 7300 2700
$Comp
L AVR-ISP-6 CON1
U 1 1 58F3AC05
P 8100 4000
F 0 "CON1" H 7995 4240 50  0000 C CNN
F 1 "AVR-ISP-6" H 7835 3770 50  0000 L BNN
F 2 "Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm" V 7580 4040 50  0001 C CNN
F 3 "" H 8075 4000 50  0001 C CNN
	1    8100 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 5250 5000 5250
Wire Wire Line
	5000 5250 5000 5500
Wire Wire Line
	5000 5500 8550 5500
Wire Wire Line
	8550 5500 8550 3900
Wire Wire Line
	8550 3900 8200 3900
Wire Wire Line
	8200 4100 8200 5350
Wire Wire Line
	8200 5350 4800 5350
Wire Wire Line
	4800 5350 4800 5100
Wire Wire Line
	4800 5100 5200 5100
Wire Wire Line
	7300 3600 8450 3600
Wire Wire Line
	7300 3750 7700 3750
Wire Wire Line
	7700 3750 7700 3900
Wire Wire Line
	7700 3900 7950 3900
Wire Wire Line
	7300 3900 7600 3900
Wire Wire Line
	7600 3900 7600 4000
Wire Wire Line
	7600 4000 7950 4000
Wire Wire Line
	7950 4100 4950 4100
Wire Wire Line
	4950 4100 4950 3000
Wire Wire Line
	4950 3000 5200 3000
Text Label 8200 3900 0    60   ~ 0
3.3V
Wire Wire Line
	8450 3600 8450 4000
Wire Wire Line
	8450 4000 8200 4000
Text Label 8200 4100 0    60   ~ 0
GND
$EndSCHEMATC
