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
NoConn ~ 5200 2400
NoConn ~ 5200 2550
NoConn ~ 5200 2700
NoConn ~ 5200 2850
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
Text Label 8200 3900 0    60   ~ 0
3.3V
Text Label 8200 4100 0    60   ~ 0
GND
$Comp
L CONN_01X06 J1
U 1 1 591FC29A
P 9150 4400
F 0 "J1" H 9150 4750 50  0000 C CNN
F 1 "CONN_01X06" V 9250 4400 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.00mm" H 9150 4400 50  0001 C CNN
F 3 "" H 9150 4400 50  0001 C CNN
	1    9150 4400
	1    0    0    -1  
$EndComp
Text Label 8950 4150 0    60   ~ 0
MOSI
Text Label 8950 4250 0    60   ~ 0
MISO
Text Label 8950 4350 0    60   ~ 0
SCK
Text Label 8950 4450 0    60   ~ 0
RST
Text Label 8950 4550 0    60   ~ 0
VCC
Text Label 8950 4650 0    60   ~ 0
GND
$Comp
L CONN_01X06 J2
U 1 1 598853A4
P 9125 5125
F 0 "J2" H 9125 5475 50  0000 C CNN
F 1 "CONN_01X06" V 9225 5125 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.00mm" H 9125 5125 50  0001 C CNN
F 3 "" H 9125 5125 50  0001 C CNN
	1    9125 5125
	1    0    0    -1  
$EndComp
Text Label 8925 4875 0    60   ~ 0
MOSI
Text Label 8925 4975 0    60   ~ 0
MISO
Text Label 8925 5075 0    60   ~ 0
SCK
Text Label 8925 5375 0    60   ~ 0
GND
Text Label 8925 5175 0    60   ~ 0
RST
Text Label 8925 5275 0    60   ~ 0
5V
$Comp
L R R1
U 1 1 59885CC6
P 7900 3150
F 0 "R1" V 7980 3150 50  0000 C CNN
F 1 "R" V 7900 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7830 3150 50  0001 C CNN
F 3 "" H 7900 3150 50  0001 C CNN
	1    7900 3150
	0    1    1    0   
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
	7300 3600 8950 3600
Wire Wire Line
	7300 3750 7700 3750
Wire Wire Line
	7700 3750 7700 3900
Wire Wire Line
	7700 3900 7950 3900
Wire Wire Line
	7300 3900 7600 3900
Wire Wire Line
	7600 3900 7600 4350
Wire Wire Line
	7600 4000 7950 4000
Wire Wire Line
	7950 4100 4950 4100
Wire Wire Line
	4950 4100 4950 3000
Wire Wire Line
	4950 3000 5200 3000
Wire Wire Line
	8450 3600 8450 4000
Wire Wire Line
	8450 4000 8200 4000
Wire Wire Line
	8950 3600 8950 4150
Connection ~ 8450 3600
Wire Wire Line
	8950 4250 7500 4250
Wire Wire Line
	7500 4250 7500 3750
Connection ~ 7500 3750
Wire Wire Line
	7600 4350 8950 4350
Connection ~ 7600 4000
Wire Wire Line
	8950 4450 7700 4450
Wire Wire Line
	7700 4450 7700 4100
Connection ~ 7700 4100
Wire Wire Line
	8950 4550 8550 4550
Connection ~ 8550 4550
Wire Wire Line
	8950 4650 8200 4650
Connection ~ 8200 4650
Wire Wire Line
	5200 4950 4675 4950
Wire Wire Line
	4675 4950 4675 5650
Wire Wire Line
	4675 5650 8625 5650
Wire Wire Line
	8625 5650 8625 5275
Wire Wire Line
	8625 5275 8925 5275
Wire Wire Line
	8925 5375 8450 5375
Wire Wire Line
	8450 5375 8450 5200
Wire Wire Line
	8450 5200 8200 5200
Connection ~ 8200 5200
Wire Wire Line
	8925 4875 8700 4875
Wire Wire Line
	8700 4875 8700 3600
Connection ~ 8700 3600
Wire Wire Line
	8925 4975 8375 4975
Wire Wire Line
	8375 4975 8375 4250
Connection ~ 8375 4250
Wire Wire Line
	8025 5075 8925 5075
Wire Wire Line
	8025 4350 8025 5075
Connection ~ 8025 4350
Wire Wire Line
	8925 5175 7850 5175
Wire Wire Line
	7850 5175 7850 4450
Connection ~ 7850 4450
Wire Wire Line
	7300 3450 7650 3450
Wire Wire Line
	7650 3450 7650 3150
Wire Wire Line
	7650 3150 7750 3150
$Comp
L LED D1
U 1 1 59885E11
P 8125 2750
F 0 "D1" H 8125 2850 50  0000 C CNN
F 1 "LED" H 8125 2650 50  0000 C CNN
F 2 "LEDs:LED_0805_HandSoldering" H 8125 2750 50  0001 C CNN
F 3 "" H 8125 2750 50  0001 C CNN
	1    8125 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	8125 3150 8125 2900
Wire Wire Line
	8050 3150 8125 3150
Wire Wire Line
	8125 2600 7650 2600
Wire Wire Line
	7650 2600 7650 2550
Wire Wire Line
	7650 2550 7300 2550
$EndSCHEMATC
