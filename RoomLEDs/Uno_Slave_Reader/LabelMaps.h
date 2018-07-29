using modePointer = void(*)(void);

modePointer mode;

enum EEPROM_ADDRESS {
  e_mode
  ,e_red
  ,e_green
  ,e_blue
  ,e_freq
  ,e_per
  ,e_width
  
  ,e_STORED_ARGM_NUM
};

/* Arduino doesn't have hashmaps.
   Best alternative is a 1:1 array correspondence.
*/
const char modeLabels[] = {
   'f'//ill color
  ,'s'
  ,'t'//heaterChase
  ,'r'//ainbow
  ,'v'//isualizer
  ,'R'//ainbow (solid)

  //Eye mode?
  //Paintball?
  //Traffic?
};
const modePointer modes[] = {
  []() { showColor(R, G, B); delay(100);},
  Sparkle,
  theaterChase,
  rainbowGradient,
  Chroma,
  rainbowSolid,
};

int8_t label2modeIndex(char label) {
  for (uint8_t i = 0; i < ARRAY_SIZE(modeLabels); i++)
    if (label == modeLabels[i]) return i;
  return -1;
}


#define EEPROM_MODE_ADDR        0
#define EEPROM_RGB_R_ADDR       1
#define EEPROM_RGB_G_ADDR       2
#define EEPROM_RGB_B_ADDR       3
#define EEPROM_CYCLE_SPEED_ADDR 4
#define EEPROM_CYCLE_REPS_ADDR  5
#define EEPROM_SPACING_ADDR     6
#define EEPROM_STORED_ARG_NUM   7 // Increase if more customization options are added

//uint8_t arg2addr(char argument) {
//  switch(argument) {
//    case 'M': return 0;
//    case 'R': return 1;
//    case 'G': return 2;
//    case 'B': return 3;
//    case 'F': return 4;
//    case 'P': return 5;
//    case 'W': return 6;
//    default: return -1;
//   }
//}
//

//modePointer label2mode(char label) {
//  switch(label) {
//    case 't': return []() { theaterChase(R, G, B); };
//    case 'f': return []() { showColor(R, G, B); delay(1000);};
//    case 'r': return []() { rainbowCycle(5 , 3); };
//    case 'v': return Chroma;
//
//    //Eye mode?
//    //Paintball?
//    //Traffic?
//
////    default: return label2mode('f');
//    default: return NULL;
//  }
//}

//const PROGMEM uint8_t i2c_arg_map[] = {
//   'm'//ode       -> EEPROM_MODE_ADDR  <- This is only for index padding
//  ,'r'//ed        -> EEPROM_RGB_R_ADDR
//  ,'g'//reen      -> EEPROM_RGB_G_ADDR
//  ,'b'//lue       -> EEPROM_RGB_B_ADDR
//  ,'f'//requency  -> EEPROM_CYCLE_SPEED_ADDR
//  ,'p'//eriod     -> EEPROM_CYCLE_REPS_ADDR
//  ,'w'//idth      -> EEPROM_SPACING_ADDR
//};

