## 2nd Year Engineering Design Project
### Gas flow controller
This code was used in my final 2nd year Engineering design project with a group of 8 others. This code was written by a group member and I. The project regarded creating an automated oxygen flow controller to automatically adjust oxygen flow to the patient depending on their oxygen saturation levels (SpO<sub>2</sub>). The oxygen saturation was measured by using the [MIKROE-2000](https://uk.rs-online.com/web/p/sensor-development-kits/1360770/) by MikroElektronika. This pulseoximeter was used with an Arduino nano running the pulseoxcode.ino code. A LCD was also connected to the device to show live oxygen saturation readings.
  
A pager was also created for nurses/doctors to use as they were away from the main oxygen controlling unit. The pager consisted of an LCD screen showing SpO<sub>2</sub> and an Arduino nano running the receiver.ino code. The main flow controller/measuring device would also send warning messages to the pager if the oxygen saturation of the patient was too low/high.

All code can be run by using the Arduino IDE

**Libraries required:**
* Radiohead
* Servotimer2 (Default servomotor library conflicts with RF library)

### Picture
<img src="https://i.imgur.com/KRZ8Dbn.jpg" alt="drawing" width="400"/>
