# DIY SpaceMouse
A DIY SpaceMouse inspired my sb_ocr 's spacemouse build. Altough heavily inspired and based on sb_ocr's work, i designed all the files myself. 

I designed this to make the top "cap" less tall and to accomodate the Seeed studio Xiao RP2040 microcontroller as that's what i could get my hands on. I also changed the code to be purely rotation and zoom as the magnetic sensor (the TLV493D) doesn't have enough DOF to track the full pitch, yaw, roll, zoom, and pan. The code currently only measures zoom, pitch and yaw, which I have found to be the most important movements to replicate.

PLease note that the code is still a work in progress at this point. The buttons still need work to get working reliably and I've started to implement the code to make my DIY spacemouse compatible with the official spacemouse driver.

The hardware required other than the 3d prints is:
•16x M2.5 X 6mm screws
•2x M2.5 X 4mm screws
•2x 10X3mm magnets
•3x 7.14X19.05mm compression springs
•3x 7.93X28.5mm extension springs
•18x M2.5X3.5X4mm heatset inserts

Electronics:
•Seeed Xiao RP2040
•TLV439D sensor
•2x "long" tactile switches

Assembly is fairly straitfoward, simply add threaded inserts (be carefull about the side for the magnetic sensor, it only has two screws that go on one side) and then assemble like the 3d model.

I used the Arduino IDE to flash the code. you will need the "Arduino Mbed OS RP2040 Boards" Package by the official Arduino account.
