EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x20_Odd_Even J1
U 1 1 61F999CE
P 8700 3250
F 0 "J1" H 8750 4367 50  0000 C CNN
F 1 "Conn_02x20_Odd_Even" H 8750 4276 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 8700 3250 50  0001 C CNN
F 3 "~" H 8700 3250 50  0001 C CNN
	1    8700 3250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 61F9BF91
P 8500 2350
F 0 "#PWR0101" H 8500 2100 50  0001 C CNN
F 1 "GND" V 8505 2222 50  0000 R CNN
F 2 "" H 8500 2350 50  0001 C CNN
F 3 "" H 8500 2350 50  0001 C CNN
	1    8500 2350
	0    1    1    0   
$EndComp
$Comp
L Shop-Parts:I50115 U1
U 1 1 61FA03B5
P 10500 900
F 0 "U1" H 10678 821 50  0000 L CNN
F 1 "I50115" H 10678 730 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 10500 900 50  0001 C CNN
F 3 "" H 10500 900 50  0001 C CNN
	1    10500 900 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 61FA2415
P 9650 1050
F 0 "#PWR0102" H 9650 800 50  0001 C CNN
F 1 "GND" V 9655 922 50  0000 R CNN
F 2 "" H 9650 1050 50  0001 C CNN
F 3 "" H 9650 1050 50  0001 C CNN
	1    9650 1050
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_Coaxial_Power J2
U 1 1 61FA6FB0
P 9850 950
F 0 "J2" V 9633 900 50  0000 C CNN
F 1 "Conn_Coaxial_Power" V 9724 900 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 9850 900 50  0001 C CNN
F 3 "~" H 9850 900 50  0001 C CNN
	1    9850 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	9950 950  10250 950 
Wire Wire Line
	9650 950  9650 1050
Wire Wire Line
	9650 1050 10250 1050
Connection ~ 9650 1050
$Comp
L Connector_Generic:Conn_01x06 J5
U 1 1 61FAA974
P 10550 2650
F 0 "J5" H 10630 2642 50  0000 L CNN
F 1 "Conn_01x06" H 10630 2551 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-6_P5.08mm" H 10550 2650 50  0001 C CNN
F 3 "~" H 10550 2650 50  0001 C CNN
	1    10550 2650
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J6
U 1 1 61FAD3EC
P 10550 3650
F 0 "J6" H 10630 3642 50  0000 L CNN
F 1 "Conn_01x06" H 10630 3551 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-6_P5.08mm" H 10550 3650 50  0001 C CNN
F 3 "~" H 10550 3650 50  0001 C CNN
	1    10550 3650
	1    0    0    -1  
$EndComp
Text Label 9050 4050 0    50   ~ 0
Y_ENA
Text Label 9050 3950 0    50   ~ 0
Y_DIR
Wire Wire Line
	9000 3850 9300 3850
Wire Wire Line
	9000 3950 9300 3950
Wire Wire Line
	9000 4050 9300 4050
Entry Wire Line
	9300 3850 9400 3950
Entry Wire Line
	9300 3950 9400 4050
Entry Wire Line
	9300 4050 9400 4150
$Comp
L power:GND #PWR0103
U 1 1 61FC1054
P 10350 2550
F 0 "#PWR0103" H 10350 2300 50  0001 C CNN
F 1 "GND" V 10355 2422 50  0000 R CNN
F 2 "" H 10350 2550 50  0001 C CNN
F 3 "" H 10350 2550 50  0001 C CNN
	1    10350 2550
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 61FC183C
P 10350 2750
F 0 "#PWR0104" H 10350 2500 50  0001 C CNN
F 1 "GND" V 10355 2622 50  0000 R CNN
F 2 "" H 10350 2750 50  0001 C CNN
F 3 "" H 10350 2750 50  0001 C CNN
	1    10350 2750
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 61FC1B06
P 10350 2950
F 0 "#PWR0105" H 10350 2700 50  0001 C CNN
F 1 "GND" V 10355 2822 50  0000 R CNN
F 2 "" H 10350 2950 50  0001 C CNN
F 3 "" H 10350 2950 50  0001 C CNN
	1    10350 2950
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0106
U 1 1 61FC1CD2
P 10350 3550
F 0 "#PWR0106" H 10350 3300 50  0001 C CNN
F 1 "GND" V 10355 3422 50  0000 R CNN
F 2 "" H 10350 3550 50  0001 C CNN
F 3 "" H 10350 3550 50  0001 C CNN
	1    10350 3550
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 61FC205F
P 10350 3750
F 0 "#PWR0107" H 10350 3500 50  0001 C CNN
F 1 "GND" V 10355 3622 50  0000 R CNN
F 2 "" H 10350 3750 50  0001 C CNN
F 3 "" H 10350 3750 50  0001 C CNN
	1    10350 3750
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 61FC26E4
P 10350 3950
F 0 "#PWR0108" H 10350 3700 50  0001 C CNN
F 1 "GND" V 10355 3822 50  0000 R CNN
F 2 "" H 10350 3950 50  0001 C CNN
F 3 "" H 10350 3950 50  0001 C CNN
	1    10350 3950
	0    1    1    0   
$EndComp
Wire Bus Line
	9400 4150 9950 4150
Entry Wire Line
	9300 3750 9400 3850
Entry Wire Line
	9300 3650 9400 3750
Entry Wire Line
	9300 3550 9400 3650
Wire Wire Line
	9000 3750 9300 3750
Wire Wire Line
	9000 3650 9300 3650
Wire Wire Line
	9000 3550 9300 3550
Text Label 9050 3850 0    50   ~ 0
Y_PUL
Text Label 9050 3650 0    50   ~ 0
X_DIR
Text Label 9050 3550 0    50   ~ 0
X_PUL
Text Label 9050 3750 0    50   ~ 0
X_ENA
Entry Wire Line
	10050 2850 9950 2950
Entry Wire Line
	10050 2650 9950 2750
Entry Wire Line
	10050 2450 9950 2550
Wire Wire Line
	10350 2850 10050 2850
Wire Wire Line
	10350 2650 10050 2650
Wire Wire Line
	10350 2450 10050 2450
Text Label 10300 2650 2    50   ~ 0
X_DIR
Text Label 10300 2450 2    50   ~ 0
X_PUL
Text Label 10300 2850 2    50   ~ 0
X_ENA
Entry Wire Line
	10050 3850 9950 3950
Entry Wire Line
	10050 3650 9950 3750
Entry Wire Line
	10050 3450 9950 3550
Wire Wire Line
	10350 3850 10050 3850
Wire Wire Line
	10350 3650 10050 3650
Wire Wire Line
	10350 3450 10050 3450
Text Label 10300 3650 2    50   ~ 0
Y_DIR
Text Label 10300 3450 2    50   ~ 0
Y_PUL
Text Label 10300 3850 2    50   ~ 0
Y_ENA
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 61FCBBE0
P 10450 2150
F 0 "J4" H 10368 1825 50  0000 C CNN
F 1 "Conn_01x02" H 10368 1916 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 10450 2150 50  0001 C CNN
F 3 "~" H 10450 2150 50  0001 C CNN
	1    10450 2150
	1    0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 61FCE34B
P 10450 1700
F 0 "J3" H 10368 1375 50  0000 C CNN
F 1 "Conn_01x02" H 10368 1466 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-2_P5.08mm" H 10450 1700 50  0001 C CNN
F 3 "~" H 10450 1700 50  0001 C CNN
	1    10450 1700
	1    0    0    1   
$EndComp
Wire Wire Line
	9000 3450 9850 3450
Wire Wire Line
	9750 3350 9000 3350
$Comp
L power:GND #PWR0109
U 1 1 61FCF5EE
P 10250 2150
F 0 "#PWR0109" H 10250 1900 50  0001 C CNN
F 1 "GND" V 10255 2022 50  0000 R CNN
F 2 "" H 10250 2150 50  0001 C CNN
F 3 "" H 10250 2150 50  0001 C CNN
	1    10250 2150
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0110
U 1 1 61FCF880
P 10250 1700
F 0 "#PWR0110" H 10250 1450 50  0001 C CNN
F 1 "GND" V 10255 1572 50  0000 R CNN
F 2 "" H 10250 1700 50  0001 C CNN
F 3 "" H 10250 1700 50  0001 C CNN
	1    10250 1700
	0    1    1    0   
$EndComp
Wire Wire Line
	9850 2050 10250 2050
Wire Wire Line
	9850 2050 9850 3450
Wire Wire Line
	9750 1600 10250 1600
Wire Wire Line
	9750 1600 9750 3350
$Comp
L power:+12V #PWR0111
U 1 1 61FD0CEA
P 10250 1150
F 0 "#PWR0111" H 10250 1000 50  0001 C CNN
F 1 "+12V" V 10265 1278 50  0000 L CNN
F 2 "" H 10250 1150 50  0001 C CNN
F 3 "" H 10250 1150 50  0001 C CNN
	1    10250 1150
	0    -1   -1   0   
$EndComp
$Comp
L power:+12V #PWR0112
U 1 1 61FD1504
P 8500 2750
F 0 "#PWR0112" H 8500 2600 50  0001 C CNN
F 1 "+12V" V 8515 2878 50  0000 L CNN
F 2 "" H 8500 2750 50  0001 C CNN
F 3 "" H 8500 2750 50  0001 C CNN
	1    8500 2750
	0    -1   -1   0   
$EndComp
NoConn ~ 8500 2450
NoConn ~ 8500 2550
NoConn ~ 8500 2650
NoConn ~ 8500 2850
NoConn ~ 8500 2950
NoConn ~ 8500 3050
NoConn ~ 8500 3150
NoConn ~ 8500 3250
NoConn ~ 8500 3350
NoConn ~ 8500 3450
NoConn ~ 8500 3550
NoConn ~ 8500 3650
NoConn ~ 8500 3750
NoConn ~ 8500 3850
NoConn ~ 8500 3950
NoConn ~ 8500 4050
NoConn ~ 8500 4150
NoConn ~ 8500 4250
NoConn ~ 9000 4250
NoConn ~ 9000 4150
NoConn ~ 9000 3250
NoConn ~ 9000 3150
NoConn ~ 9000 3050
NoConn ~ 9000 2950
NoConn ~ 9000 2850
NoConn ~ 9000 2750
NoConn ~ 9000 2650
NoConn ~ 9000 2550
NoConn ~ 9000 2450
NoConn ~ 9000 2350
Wire Bus Line
	9400 3650 9400 4150
Wire Bus Line
	9950 2550 9950 4150
$EndSCHEMATC
