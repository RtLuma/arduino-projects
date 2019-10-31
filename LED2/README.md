
## Lessons Learned
* Audio signals have negative voltage, say -2.5V to 2.5V is the signal range. You use a voltage divider (in this case, use identical resistors to halve it) and a capacitor to store a charge at ~2.5V and then the negative voltage discharges the capacitor to 0, or positive voltage adds it to 5V, essentially adding 2.5V to the signal so there's no negative voltage.
* Hamming window is probably fine. FFT is constricting the endpoints of the signal to meet, essentially creating a finite field that can wrap into modulus infinitum. CPU speed directly determines sampling Hz, and the Nyquist limit of the bands we can get frequency domain amplitudes for.
* Differential signals are similar, but in the above case instead of -2.5 to 2.5V you'd get 2 signals each ranging 0-2.5V, but one of them represents the negative component.
* For EEPROM 24**256 modules, 24LC256 won't do lower voltages but has a higher temperature range. 24AA256 can handle lower voltages, but has a lower temperature range. Both have a maximum clockrate of 400kHz.
* Don't pass in the address of the symbol that refers to an array as it's likely garbage (the address of a pointer), e.g.
  ```c++
  #include <iostream>
  #include <string>
  using namespace std;

  void iterate_and_print(uint8_t* a, size_t sa) {
    for (int i =0; i < sa; i++) cout << to_string(a[i]);
    cout << endl;
  }

  int main() {
    uint8_t a[] ={ 0, 1, 2, 3, 4 };
    size_t a_size = (sizeof(a) / sizeof(*a));
    iterate_and_print(a, a_size);
  }
  ```
  `&a` is meaningless, `a` is already a pointer i.e. address, the address of an address is garbage. Instead do `iterate_and_print(a, a_size);`
* To get miniboot to work with my specific EEPROM module (24LC256) I had to raise both SDA and SCL to high so that the SCL could be pulled low to initiate the I2C transaction and actually recognize an ACK signal, not getting stuck in a while loop, specifically:
  ```c++
  void i2c_init(void) {
  	TWBR = (uint8_t)TWBR_val;
  	PORTC = 0x30; 
  }
  ```
  because SCK and SDA are port C pins 4 and 5 respectively, raising them high is (big-endian)`0b00110000` == `0x30`.
* Compiled Arduino bins are only ~10KB, meaning OTA is tractable as heck! I can pull around 10KB/s of reliable transmission in a direct streaming-pipe setup.
* Maybe the fast-FFT worked perfectly and the noisiness I was seeing in the bands was a result of quantization noise in the output samples (I was using youtube after all). I may need to add some dithering blue noise to the signal so that the fast FFT is more viable, as the less clock cycles I can use on that the better.
* The mesh is too complicated for my needs, upon rethinking the multiple-pipe support at the bare-radio level was simple and perfect and was shining into my eyeballs from the start. What's the lesson here? Idk, it was worth exploring the mesh because I learned some good lessons and it gave me some decent ideas. I just wish it had taken up less time.
* External/hardware interrupts > timer. But to get an interrupt interrupted you must enable interrupts from within the interrupt. [Extensive AVR interrupt info.](https://courses.cs.washington.edu/courses/csep567/10wi/lectures/Lecture7.pdf)
* `writeFast` was too fast, it was clobbering my ACKed payloads. `radio.write(&fft_payload, FFT_SAMPLES, 1);` is still perfectly fast for my purposes (can transmit ~160 32-byte payloads a second)


  
---
# Project Skeleton
## Master &mdash; Sparkfun ESP8266 Thing Dev (WRL-13711)
* [ ] Revisit [fast(fixed)FFT](https://github.com/kosme/fix_fft) with the inclusiong of [blue-noise](https://github.com/joshbainbridge/blue-noise-generator/blob/master/src/main.cpp) dithering to see if that fixes the noisy power spectrum.
* [x] ~~Also look into arduinoCLI cuz fuck this menuing , let's automate per `tty` please.~~ Now this is pod-racing.
* [ ] Get a dang async file-upload proof of concept. Slave payload should fit (I hope), ~30KB max


### Swarm Protocol (Simple Mesh)
#### Pipes
> Pipe 0 is used by the writing pipe. So if you open pipe 0 for reading, and then startListening(), it will overwrite the writing pipe. Ergo, do an openWritingPipe() again before write().

> Pipes 1-5 should share the first 32 bits. Only the least significant byte should be unique, e.g. `openReadingPipe(1,0xF0F0F0F0AA); openReadingPipe(2,0xF0F0F0F066);`

  * Writing:
    * Multicast UDP / NOACK data-bombardment, mostly for FFT payloads, tentatively `0x696969{69,69}`
    * Unicast TCP / ACKed datastream, critical for mode-changing and OTA payload distribution. Probably `multicast_pipe_addr` + ID.
    * Buddy-system request hotline, `"HELP!"` aka `uint8_t pipe[] = {0x48, 0x45, 0x4c, 0x50, 0x21};`
    * Slaves can only "write" in an ACK payload, if the Master sends a query packet, much like I2C. 
      * Will likely need to have several flavors of query to fit payload-size constraints.
      * Master should only query if music is quiet or off.

* [ ] On master boot, just iterate over X possible supported pipe quantity, probably ~16 or 32 to populate network list.
  * This would mean the master needs to be turned on last or &darr;
    * [ ] have a mode that triggers a re-mesh.


### Asynchronous webserver
* [x] ~~Look into [asynchronous webserver](https://github.com/me-no-dev/ESPAsyncWebServer)~~
* [ ] Async web proof of viability
  * [ ] Slave diagnostics for heartbeat monitoring? Might be overkill, but cool IoT fun.
    * ~~Use the "write to response" buffer/stream thing~~ Obviously would take too long.
    * [ ] or use the streaming/socket stuff
  * [ ] Obviously an API to control the network
  * [ ] Remake pretty GUI~ (that just uses the API)
    * [ ] Complete with minification compression goodness!
  * [ ] Provide AP captive portal for setting Wi-Fi credentials on network failure.
  * [ ] Radio-analytics page, provide interface for setting channel and whatnot
  * [ ] Settings for other jazz, should also be accessible from its own AP.
  
  #### Slave OTA Firmware Update
  * [x] ~~Slave self-burn from external EEPROM proof of viability~~
  * [ ] Async webpage for uploading new firmware and dispatching
    * [ ] Upload event pipeline:
      * [ ] Read POST fields for user-provided metadata e.g. appname
      * [ ] Return checksum + metadata on upload
      * [ ] Place all nodes in NOP mode to prevent airspace colisions.
      * [ ] Per-node upload handshake goodness
        * [x] ~~Need to find maximum payload size of shockburst so I can manually shard.~~ It's 32bytes
        * Use `startFastWrite` for multi-payload.
      * _Requeue failures? Guess it uniquedepends on how often failures occur_
      * [ ] Trigger reboot all once new firmware is dispatched, S-a-M Mesh should handle this gracefully.
    * [ ] Look into header-metadata node-module so we can set that ourself. Probably makes sense to send the metadata in a separate payload independent from the firmware so the slaves can set that in extEEPROM upon successful firmware transmission.
      * [ ] Need to look into NTP stuff to get the timestamp.
    * [ ] Will probably also want to make this `curl`able to tack into the arduinoCLI compipeline mentioned above.
      * It'll be curlable by existing retard but okay
    

<details>
<summary> ~~Mesh network~~ JK, this overengineered garbage significantly overcomplicates things.</summary>

  * [x] ~~Look into getting multicast to work at the network layer~~
    * [x] ~~Might have to iterate over meshed addresses to see if any jive~~
    * ~~It works at master address (00) and starting at level 1, and most notably with small payload~~
  * [ ] ~~Implement network dropper for nodes that fall out.~~
    * [ ] ~~Or don't, and just make the expected-shutdown process more regulated and release radio-DHCP addresses more religiously. i.e. `releaseAddress() && tuya node off`~~


~~Char representations of high-priority header-type enums, e.g. `A = 65` is the lowest high priority label value, going up to `~ = 126` (I believe 127 is reserved)~~
```
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _ `
a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
```
</details>



### 


### nRF24L01

<details>
  <summary>Pinout</summary>
  
#### nRF24L01 pinout to ESP8266 Thing Dev Board pins
|ESP8266||nRF24L01|nRF24L01||ESP8266|
|:-:|:-:|:-:|:-:|:-:|:-|
| GND  | &larr; | GND  | VCC   | &rarr; | 3.3V  |
| GPIO - 4 (was 2, but 2 is SDA)  | &larr; | CE   | !CS aka CSN  | &rarr; | GPIO - 0  |
| 14 | &larr; | SCK  | MOSI | &rarr; |  13 |
| 12  | &larr; | MISO | IRQ  | &rarr; | 5 |

GPIO not used for SPI can be 0, 2(SDA for I2C), 4, 5(LED), 16(XPD), so any GPIO listed above can be from this pool&mdash;just make sure to initialize the RF24 in code appropriately, e.g. `RF24 radio(4, 0);`
</details>





---

## Slaves &mdash; Arduino Nano 

* [ ] `nodeID` from EEPROM
* [ ] Listen on `HELP!` pipe to support nodes not in range.
  * Wait random time to send a help-ack cuz Wi-Fi protocols taught me collision stuff.
  * Help-ack-ack solidifies friendship for a lifetime! Coding the relaying will be a pain, I guess only build this bridge if we need to.
    * ~~Maybe have the helpee set their pipe to this node's ID pipe but open for writing? Could get hairy if TCP inbound from master to helper.~~ Actually, just have the helper write to the helpee's ID-pipe duh, but UDP data-bomb will need to be profiled in this configuration.