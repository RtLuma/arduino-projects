
## Lessons Learned
* Audio signals have negative voltage, say -2.5V to 2.5V is the signal range. You use a voltage divider (in this case, use identical resistors to halve it) and a capacitor to store a charge at ~2.5V and then the negative voltage discharges the capacitor to 0, or positive voltage adds it to 5V, essentially adding 2.5V to the signal so there's no negative voltage.
* Hamming window is probably fine. FFT is constricting the endpoints of the signal to meet, essentially creating a finite field that can wrap into modulus infinitum. CPU speed directly determines sampling Hz, and the Nyquist limit of the bands we can get frequency domain amplitudes for.
* Differential signals are similar, but in the above case instead of -2.5 to 2.5V you'd get 2 signals each ranging 0-2.5V, but one of them represents the negative component.
* For EEPROM 24**256 modules, 24LC256 won't do lower voltages but has a higher temperature range. 24AA256 can handle lower voltages, but has a lower temperature range. Both have a maximum clockrate of 400kHz.
* Don't pass in the address of the symbol that refers to an array as it's likely garbage (the address of a pointer), e.g.
  > ```c++
  > uint8_t a[] ={ 0, 1, 2, 3, 4 };
  > uint8_t a_size = (sizeof(a) / sizeof(*a));
  > iterate_and_print(&a, a_size);
  > ```
  > `&a` is meaningless, `a` is already a pointer i.e. address, the address of an address is garbage. Instead do `iterate_and_print(a, a_size);`
* To get miniboot to work with my specific EEPROM module (24LC256) I had to raise both SDA and SCL to high so that the SCL could be pulled low to initiate the I2C transaction and actually recognize an ACK signal, not getting stuck in a while loop, specifically:
  > ```c++
  > void i2c_init(void) {
  > 	TWBR = (uint8_t)TWBR_val;
  > 	PORTC = 0x30; 
  > }
  > ```
  because SCK and SDA are port C pins 4 and 5 respectively, raising them high is (big-endian)`0b00110000` == `0x30`.
* Compiled Arduino bins are only ~10KB, meaning OTA is tractable as heck! I can pull around 10KB/s of reliable transmission in a direct streaming-pipe setup.
  
---
# Project Skeleton
## Master &mdash; Sparkfun ESP8266 Thing Dev (WRL-13711)
### ESP8266
* [ ] Look into [asynchronous webserver](https://github.com/me-no-dev/ESPAsyncWebServer)
### nRF24L01
> #### nRF24L01 pinout to ESP8266 Thing Dev Board pins
> |||||||
> |:-:|:-:|:-:|:-:|:-:|:-|
> | GND  | &larr; | GND  | VCC   | &rarr; | 3.3V  |
> | GPIO - 4 (was 2, but 2 is SDA)  | &larr; | CE   | !CS aka CSN  | &rarr; | GPIO - 0  |
> | 14 | &larr; | SCK  | MOSI | &rarr; |  13 |
> | 12  | &larr; | MISO | IRQ  | &rarr; | _(not necessary when used in mesh)_ |
> | | | | | 
> GPIO not used for SPI can be 0, 2(SDA for I2C), 4, 5(LED), 16(XPD), so any GPIO listed above can be from this pool&mdash;just make sure to initialize the RF24 in code appropriately, e.g. `RF24 radio(4, 0);`


#### Mesh
##### Payload 

## Slaves &mdash; Arduino Nano 