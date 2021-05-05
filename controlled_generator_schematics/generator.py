for i in range(100):
    print(
        """
$Comp
L power:+24V #PWR?
U 1 1 5FA14EB2
P 9350 3800
F 0 "#PWR?" H 9350 3650 50  0001 C CNN
F 1 "+24V" H 9365 3973 50  0000 C CNN
F 2 "" H 9350 3800 50  0001 C CNN
F 3 "" H 9350 3800 50  0001 C CNN
        1    9350 3800
        1    0    0    -1  
$EndComp
$Comp
L Device:Q_PNP_BCE Q?
U 1 1 5FA14EB8
P 9250 4000
F 0 "Q?" H 9440 3954 50  0000 L CNN
F 1 "Q_PNP_BCE" H 9440 4045 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92" H 9450 4100 50  0001 C CNN
F 3 "~" H 9250 4000 50  0001 C CNN
        1    9250 4000
        1    0    0    1   
$EndComp
Wire Wire Line
        9350 4100 9350 4200
Wire Wire Line
        8550 4000 8550 4100
Wire Wire Line
        8850 4000 9050 4000
$Comp
L Device:R R?
U 1 1 5FA14EC1
P 8700 4000
F 0 "R?" V 8493 4000 50  0000 C CNN
F 1 "10k" V 8584 4000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P2.54mm_Vertical" V 8630 4000 50  0001 C CNN
F 3 "~" H 8700 4000 50  0001 C CNN
        1    8700 4000
        0    1    1    0   
$EndComp
Text GLabel 9350 4200 0    50   Output ~ 0
HB{i}
Text GLabel 8550 4100 0    50   Input ~ 0
DRAIN{}
""".format(
            i=i
        )
    )
