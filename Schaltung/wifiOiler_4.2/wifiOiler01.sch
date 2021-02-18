EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "wifiOiler D1-mini"
Date ""
Rev "4.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L wifiOilerLib:CONN_01X08 P1
U 1 1 5CCB4EEA
P 6950 4000
F 0 "P1" H 7028 4041 50  0000 L CNN
F 1 "CONN_01X08" H 6700 3800 50  0001 L CNN
F 2 "_ProjectFootprints:Stiftleiste_1x8_5mm_D13_Box_korr" H 6300 3950 50  0001 C CNN
F 3 "" H 6950 4000 50  0000 C CNN
	1    6950 4000
	1    0    0    -1  
$EndComp
$Comp
L wifiOilerLib:IRLZ34N Q1
U 1 1 5CCB81B0
P 5650 3350
F 0 "Q1" H 5856 3396 50  0000 L CNN
F 1 "IRLZ34N" H 5856 3305 50  0000 L CNN
F 2 "_ProjectFootprints:TO-220-3_Horizontal_NoPad" H 5900 3275 50  0001 L CIN
F 3 "http://www.infineon.com/dgdl/irlz34npbf.pdf?fileId=5546d462533600a40153567206892720" H 5650 3350 50  0001 L CNN
	1    5650 3350
	1    0    0    1   
$EndComp
$Comp
L wifiOilerLib:1N4004 D2
U 1 1 5CCBB590
P 6600 2600
F 0 "D2" H 6600 2816 50  0000 C CNN
F 1 "1N4004" H 6600 2725 50  0000 C CNN
F 2 "_ProjectFootprints:D_DO-41_SOD81_P7.62mm_Horizontal" H 6600 2425 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 6600 2600 50  0001 C CNN
	1    6600 2600
	1    0    0    -1  
$EndComp
$Comp
L wifiOilerLib:R R1
U 1 1 5CCBE933
P 5200 3350
F 0 "R1" H 5270 3396 50  0000 L CNN
F 1 "220" H 5270 3305 50  0000 L CNN
F 2 "_ProjectFootprints:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 5130 3350 50  0001 C CNN
F 3 "~" H 5200 3350 50  0001 C CNN
	1    5200 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6750 3750 6650 3750
Wire Wire Line
	6750 2600 6750 3650
$Comp
L wifiOilerLib:1N4004 D1
U 1 1 5CCBC0AA
P 6500 3000
F 0 "D1" H 6650 3000 50  0000 C CNN
F 1 "1N4004" H 6550 3100 50  0000 C CNN
F 2 "_ProjectFootprints:D_DO-41_SOD81_P7.62mm_Horizontal" H 6500 2825 50  0001 C CNN
F 3 "http://www.vishay.com/docs/88503/1n4001.pdf" H 6500 3000 50  0001 C CNN
	1    6500 3000
	-1   0    0    1   
$EndComp
$Comp
L wifiOilerLib:NEO6M N1
U 1 1 5CCB638B
P 5650 4700
F 0 "N1" V 5600 4550 60  0000 L CNN
F 1 "NEO6M" V 5500 4450 60  0000 L CNN
F 2 "_ProjectFootprints:NEO6M_18_ChipDown" H 5650 4550 60  0001 C CNN
F 3 "" H 5650 4550 60  0000 C CNN
	1    5650 4700
	0    -1   -1   0   
$EndComp
Text Notes 7200 4350 0    59   ~ 0
1 = Batterie Plus\n2 = Batterie Minus\n3 = Pumpe (plus)\n4 = Pumpe (minus, D7)\n5 = LED grün (D0)\n6 = LED rot (D5)\n7 = Button (minus, D6)\n8 = Button (plus)
Wire Wire Line
	5350 3350 5450 3350
Wire Wire Line
	5750 3150 5750 3000
Connection ~ 5750 3000
Wire Wire Line
	5150 4650 4850 4650
Wire Wire Line
	4950 4750 5150 4750
Wire Wire Line
	5750 2900 5750 3000
Connection ~ 5450 2600
Wire Wire Line
	5450 2700 5450 2600
$Comp
L wifiOilerLib:CP_Small C1
U 1 1 5CCBDC9B
P 5450 2800
F 0 "C1" H 5550 2750 50  0000 L CNN
F 1 "CP_Small" H 5300 2650 50  0000 L CNN
F 2 "_ProjectFootprints:MKS02-63 100N Folienkondensator" H 5450 2800 50  0001 C CNN
F 3 "~" H 5450 2800 50  0001 C CNN
	1    5450 2800
	1    0    0    -1  
$EndComp
$Comp
L wifiOilerLib:CP_Small C2
U 1 1 5CCBD364
P 6050 2800
F 0 "C2" H 5850 2750 50  0000 L CNN
F 1 "CP_Small" H 5850 2650 50  0000 L CNN
F 2 "_ProjectFootprints:MKS02-63 100N Folienkondensator" H 6050 2800 50  0001 C CNN
F 3 "~" H 6050 2800 50  0001 C CNN
	1    6050 2800
	1    0    0    -1  
$EndComp
$Comp
L wifiOilerLib:LF50_TO220 U1
U 1 1 5CCB9E96
P 5750 2600
F 0 "U1" H 5750 2842 50  0000 C CNN
F 1 "LC78_05_TO220" H 5750 2751 50  0000 C CNN
F 2 "_ProjectFootprints:LC78_0.5 aufrecht" H 5750 2825 50  0001 C CIN
F 3 "http://www.st.com/content/ccc/resource/technical/document/datasheet/c4/0e/7e/2a/be/bc/4c/bd/CD00000546.pdf/files/CD00000546.pdf/jcr:content/translations/en.CD00000546.pdf" H 5750 2550 50  0001 C CNN
	1    5750 2600
	-1   0    0    -1  
$EndComp
$Comp
L wifiOilerLib:CP C3
U 1 1 5DEDE899
P 6250 2750
F 0 "C3" H 6368 2796 50  0000 L CNN
F 1 "10 uF" H 6368 2705 50  0000 L CNN
F 2 "_ProjectFootprints:CP_Radial_D5.0mm_P2.00mm" H 6288 2600 50  0001 C CNN
F 3 "~" H 6250 2750 50  0001 C CNN
	1    6250 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3550 5750 3950
Wire Wire Line
	5750 3950 6750 3950
$Comp
L wifiOilerLib:R R3
U 1 1 5E8B325C
P 6100 3550
F 0 "R3" H 6030 3504 50  0000 R CNN
F 1 "10K" H 6030 3595 50  0000 R CNN
F 2 "_ProjectFootprints:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6030 3550 50  0001 C CNN
F 3 "~" H 6100 3550 50  0001 C CNN
	1    6100 3550
	-1   0    0    1   
$EndComp
Connection ~ 5050 4850
Wire Wire Line
	5050 4850 5150 4850
Wire Wire Line
	5050 4850 5050 5050
Wire Wire Line
	5050 5050 6600 5050
Wire Wire Line
	6700 5150 6100 5150
$Comp
L wifiOilerLib:R R2
U 1 1 5E8C0F69
P 4850 3200
F 0 "R2" H 4780 3154 50  0000 R CNN
F 1 "10K" H 4780 3245 50  0000 R CNN
F 2 "_ProjectFootprints:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 4780 3200 50  0001 C CNN
F 3 "~" H 4850 3200 50  0001 C CNN
	1    4850 3200
	-1   0    0    1   
$EndComp
Connection ~ 4850 3350
Wire Wire Line
	4850 3350 5050 3350
Wire Wire Line
	4850 3050 4850 3000
Connection ~ 4850 3000
Wire Wire Line
	4850 3000 4750 3000
Wire Wire Line
	4850 3000 4950 3000
Wire Wire Line
	5750 3000 6100 3000
Wire Wire Line
	6250 2900 6250 3000
Connection ~ 6250 3000
Wire Wire Line
	6250 3000 6350 3000
NoConn ~ 3300 3650
NoConn ~ 3300 3750
NoConn ~ 3300 4250
NoConn ~ 4450 4050
NoConn ~ 4450 4150
Wire Wire Line
	5450 2900 5750 2900
Connection ~ 5750 2900
Wire Wire Line
	6050 2900 5750 2900
Wire Wire Line
	6050 2700 6050 2600
Wire Wire Line
	6250 2600 6050 2600
Connection ~ 6250 2600
Connection ~ 6050 2600
Wire Wire Line
	6650 3000 6650 3750
Wire Wire Line
	6100 3700 6100 5150
Connection ~ 6100 5150
Wire Wire Line
	6100 3400 6100 3000
Connection ~ 6100 3000
Wire Wire Line
	6100 3000 6250 3000
Wire Wire Line
	6600 5050 6600 4350
Wire Wire Line
	6600 4350 6750 4350
Wire Wire Line
	6400 4050 6750 4050
Wire Wire Line
	6400 4050 6400 5350
Wire Wire Line
	6700 5150 6700 4250
Wire Wire Line
	6700 4250 6750 4250
Wire Wire Line
	6500 4150 6750 4150
Wire Wire Line
	6500 4150 6500 5250
$Comp
L wifiOilerLib:Conn_01x04 J1
U 1 1 5FD907FD
P 2550 2950
F 0 "J1" H 2468 2525 50  0000 C CNN
F 1 "Conn_01x04" H 2468 2616 50  0000 C CNN
F 2 "_ProjectFootprints:Pin_Header_Straight_1x04_Pitch2.54mm" H 2550 2950 50  0001 C CNN
F 3 "~" H 2550 2950 50  0001 C CNN
	1    2550 2950
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 2750 4950 2750
Wire Wire Line
	4950 2750 4950 3000
Connection ~ 4950 3000
Wire Wire Line
	4950 3000 5750 3000
Wire Wire Line
	4650 2600 5450 2600
Wire Wire Line
	2750 2850 2900 2850
Wire Wire Line
	4950 4750 4950 4150
Wire Wire Line
	4950 3650 4450 3650
Wire Wire Line
	4850 4650 4850 4250
Wire Wire Line
	4850 3750 4450 3750
Wire Wire Line
	4450 4250 4750 4250
Wire Wire Line
	4750 4250 4750 4350
Connection ~ 4750 4250
Wire Wire Line
	4750 3000 4750 4250
Wire Wire Line
	4750 4550 5150 4550
Wire Wire Line
	4450 4350 4650 4350
Wire Wire Line
	4650 2600 4650 4350
Wire Wire Line
	2750 2950 4450 2950
Wire Wire Line
	4450 3950 4550 3950
Wire Wire Line
	4550 3950 4550 3050
Wire Wire Line
	2750 3050 4550 3050
Wire Wire Line
	2900 4350 3300 4350
Wire Wire Line
	2900 2850 2900 4350
$Comp
L wifiOilerLib:D1-mini MCU1
U 1 1 5CE73972
P 3850 4350
F 0 "MCU1" H 3875 5715 50  0000 C CNN
F 1 "D1-mini" H 3875 5624 50  0000 C CNN
F 2 "_ProjectFootprints:D1 mini (ESP8266)" H 3650 4500 50  0001 C CNN
F 3 "" H 3650 4500 50  0001 C CNN
	1    3850 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 4850 3300 4350
Wire Wire Line
	3300 4850 5050 4850
Connection ~ 3300 4350
Wire Wire Line
	3200 5150 3200 4050
Wire Wire Line
	3200 4050 3300 4050
Wire Wire Line
	3200 5150 6100 5150
Wire Wire Line
	3100 5250 3100 3950
Wire Wire Line
	3100 3950 3300 3950
Wire Wire Line
	3100 5250 6500 5250
Wire Wire Line
	3000 5350 3000 3850
Wire Wire Line
	3000 3850 3300 3850
Wire Wire Line
	3000 5350 6400 5350
Wire Wire Line
	2800 3350 2800 4150
Wire Wire Line
	2800 4150 3300 4150
Wire Wire Line
	2800 3350 4850 3350
Wire Wire Line
	4450 2950 4450 3850
Wire Wire Line
	6250 2600 6400 2600
Wire Wire Line
	6400 2600 6400 3850
Wire Wire Line
	6400 3850 6750 3850
Connection ~ 6400 2600
Wire Wire Line
	6400 2600 6450 2600
Text Label 2450 3250 0    50   ~ 0
Display
$Comp
L wifiOilerLib:Conn_01x04 J2
U 1 1 5FFC3957
P 5550 4150
F 0 "J2" H 5630 4142 50  0000 L CNN
F 1 "Conn_01x04" H 5630 4051 50  0000 L CNN
F 2 "_ProjectFootprints:NEO6M_18_ChipUp" H 5550 4150 50  0001 C CNN
F 3 "" H 5550 4150 50  0001 C CNN
	1    5550 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 4350 4750 4350
Connection ~ 4750 4350
Wire Wire Line
	4750 4350 4750 4550
Wire Wire Line
	5350 4250 4850 4250
Connection ~ 4850 4250
Wire Wire Line
	4850 4250 4850 3750
Wire Wire Line
	5350 4150 4950 4150
Connection ~ 4950 4150
Wire Wire Line
	4950 4150 4950 3650
Wire Wire Line
	5350 4050 5050 4050
Wire Wire Line
	5050 4050 5050 4850
$EndSCHEMATC
