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
$Comp
L CONN_01X06 J1
U 1 1 58F33714
P 8500 3950
F 0 "J1" H 8500 4300 50  0000 C CNN
F 1 "CONN_01X06" V 8600 3950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 8500 3950 50  0001 C CNN
F 3 "" H 8500 3950 50  0001 C CNN
	1    8500 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 3600 8300 3600
Wire Wire Line
	8300 3600 8300 3700
Wire Wire Line
	7300 3750 8300 3750
Wire Wire Line
	8300 3750 8300 3800
Wire Wire Line
	7300 3900 8300 3900
Wire Wire Line
	5200 3000 4700 3000
Wire Wire Line
	4700 3000 4700 2000
Wire Wire Line
	4700 2000 7800 2000
Wire Wire Line
	7800 2000 7800 4000
Wire Wire Line
	7800 4000 8300 4000
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
Text Label 8300 3700 0    60   ~ 0
MOSI
Text Label 8300 3800 0    60   ~ 0
MISO
Text Label 8300 4000 0    60   ~ 0
RST
Text Label 8300 3900 0    60   ~ 0
SCK
Text Label 8300 4100 0    60   ~ 0
Vcc_3.3v
Text Label 8300 4200 0    60   ~ 0
GND
Wire Wire Line
	5200 5250 5100 5250
Wire Wire Line
	5100 5250 5100 5500
Wire Wire Line
	5100 5500 8000 5500
Wire Wire Line
	8000 5500 8000 4100
Wire Wire Line
	8000 4100 8300 4100
Wire Wire Line
	8300 4200 8150 4200
Wire Wire Line
	8150 4200 8150 5600
Wire Wire Line
	8150 5600 5000 5600
Wire Wire Line
	5000 5600 5000 5100
Wire Wire Line
	5000 5100 5200 5100
NoConn ~ 7300 2550
NoConn ~ 7300 2700
$EndSCHEMATC
