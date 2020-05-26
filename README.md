# ArduTempMonitor

This project is a simple thermometer data logger with some specials.

#### Main board: Arduino Mega 2560
I choose this board for the number of its I/O and more program capacity. So if I want to develop more stuff I don't have to deal with I/O or memory.
Also is the only board that I found a junction board with screw terminals so is more stable to connect cable and other stuff to screw terminal instead pinning them into a molex pin.

### What it does?
- Read temperature from DS18B20
- Write on LCD with date and time
- Send same data via UDP packet to a remote server for storing, analysis and graphing (the server has a listening process written in Python that get udp packets and write to mysql server. See ......... for the repos.)

### Feature

- uses a Dallas DS18B20 as temperature probe
- uses a Dallas DS3121 as RTC to have clock with date and time
- uses a Keyestudio KS0156 Ethernet shield which mount WIZnet W5100 chip (I prefer cabled ethernet connection instead of WiFi)
- uses an I2C LCD 20x4 from Quimat just for wiring semplifications and more speedy coding

---

### Future implementation
Just to be implemented:

- Touch keyboard in I2C interface from DFRobot
- SD card management for storing data (implemented in ethernet shield) just as a backup if remote server connection is broken.

