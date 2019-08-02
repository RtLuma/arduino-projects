
## Lessons Learned
* Audio signals have negative voltage, say -2.5V to 2.5V is the signal range. You use a voltage divider (in this case, use 2 resistors to perfectly halve it) and a capacitor to store a charge at ~2.5V and then the negative voltage discharges the capacitor to 0, or positive voltage adds it to 5V, essentially adding 2.5V to the signal so there's no negative voltage
* Differential signals are similar, but in the above case instead of -2.5 to 2.5V you'd get 2 signals each ranging 0-2.5V, but one of them represents the negative component.
* Don't pass in the address of the symbol that refers to an array as it's likely garbage (the address of a pointer), e.g.
  > ```c++
  > uint8_t a[] ={ 0, 1, 2, 3, 4 };
  > uint8_t a_size = (sizeof(a) / sizeof(*a));
  > iterate_and_print(&a, a_size);
  > ```
  > `&a` is meaningless, `a` is already a pointer i.e. address, the address of an address is garbage.
* To get miniboot to work with my specific EEPROM module (24LC256) I had to raise both SDA and SCL to high so that the SCL could be pulled low to initiate the I2C transaction and actually recognize an ACK signal, not getting stuck in a while loop, specifically:
  > ```c++
  > void i2c_init(void)> {
  > 	TWBR = (uint8_t)TWBR_val;
  > 	PORTC = 0x30; 
  > }
  > ```
  because SCK and SDA are port C pins 4 and 5 respectively, raising them high is (big-endian)0b00110000 == 0x30.