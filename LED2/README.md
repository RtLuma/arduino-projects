## FFT
Never had much luck with the FHT style low-level code, worth retesting the stuff with a faster board if/when that's an option. The piccolo code seemed to be promising except for the second to last bin always being maxxed out. Nevertheless you can't beat the benefits of 10x the clock cycles + the benefits of floats.
  
## nRF24L01 
I still have no idea if this can communicate directly with Wi-Fi stuff or if it is just a masquerade. Either way, the ESP8266 can be the radio-DNS as it will need a RF module in any case.

Need to look into self-flashing from external EEPROM so I can remote flash the arduino modules. Plus all the other swarm fun that goes with that.
