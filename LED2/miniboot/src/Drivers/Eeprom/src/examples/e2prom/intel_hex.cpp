#include "intel_hex.h"
#include "e2prom.h"

#define debug(X) Serial.print(X)
#define debug2(X, Y) Serial.print(X, Y)
#define debugn(X) Serial.println(X)

const uint8_t hex_file_line_length = 43;
const uint8_t metadata_length = 11; // inclues <':'-1 character><size-2
                                    // characters><address-4 characters><type`-2
                                    // characters><CRC-2 characters>

typedef enum { Intel_Hex_Data = 0, Intel_Hex_EndOfFile } TRecordType;

uint32_t hex2int(const char *source_address, uint8_t start, uint8_t count) {
  uint32_t val = 0;
  for (uint8_t i = 0; i < count; ++i) {
    uint8_t byte = pgm_read_byte(source_address + start + i);
    if (byte >= '0' && byte <= '9')
      byte = byte - '0';
    else if (byte >= 'A' && byte <= 'F')
      byte = byte - 'A' + 10;
    val = (val << 4) | (byte & 0xF);
  }
  return val;
}

const uint8_t blink_hex[] PROGMEM = {
    ":100000000C945D000C9485000C9485000C94850084\
:100010000C9485000C9485000C9485000C9485004C\
:100020000C9485000C9485000C9485000C9485003C\
:100030000C9485000C9485000C9485000C9485002C\
:100040000C943F020C9485000C94AF020C9489022E\
:100050000C9485000C9485000C9485000C9485000C\
:100060000C9485000C9485000000000024002700FB\
:100070002A0000000000250028002B0004040404CE\
:100080000404040402020202020203030303030342\
:10009000010204081020408001020408102001021F\
:1000A00004081020000000080002010000030407FB\
:1000B0000000000000000000910311241FBECFEFDC\
:1000C000D8E0DEBFCDBF11E0A0E0B1E0E6E9F7E0A7\
:1000D00002C005900D92AA31B107D9F721E0AAE13B\
:1000E000B1E001C01D92A03CB207E1F710E0CDE500\
:1000F000D0E004C02197FE010E94BE03CC35D10799\
:10010000C9F70E94E1020C94C9030C940000E1EBD2\
:10011000F0E09491EDE9F0E02491E9E8F0E0E49179\
:10012000EE2309F43CC0992339F1933091F038F46F\
:100130009130A9F0923001F594B59F7D12C09730AF\
:1001400091F09830A1F09430B9F4909180009F7DA7\
:1001500003C0909180009F77909380000DC094B56C\
:100160009F7794BD09C09091B0009F7703C0909194\
:10017000B0009F7D9093B000F0E0EE0FFF1FEE58AF\
:10018000FF4FA591B4919FB7F894811104C08C9151\
:100190002095282302C0EC912E2B2C939FBF08950D\
:1001A0003FB7F8948091BC019091BD01A091BE0130\
:1001B000B091BF0126B5A89B05C02F3F19F001964D\
:1001C000A11DB11D3FBFBA2FA92F982F8827820FDD\
:1001D000911DA11DB11DBC01CD0142E0660F771F2D\
:1001E000881F991F4A95D1F708958F929F92AF92D9\
:1001F000BF92CF92DF92EF92FF920E94D0004B010C\
:100200005C0188EEC82E83E0D82EE12CF12C0E94F0\
:10021000D000DC01CB0188199909AA09BB09883EE5\
:100220009340A105B10598F321E0C21AD108E10875\
:10023000F10888EE880E83E0981EA11CB11CC11441\
:10024000D104E104F10419F7FF90EF90DF90CF9013\
:10025000BF90AF909F908F900895CF92DF92EF92D2\
:10026000FF920F931F93CF93DF936C017A018B0161\
:10027000C0E0D0E0CE15DF0581F0D8016D918D0191\
:10028000D601ED91FC910190F081E02DC601099518\
:10029000892B11F02196EECF7E01C701DF91CF911E\
:1002A0001F910F91FF90EF90DF90CF9008950895E8\
:1002B00080E090E00895FC01538D448D252F30E0BF\
:1002C000842F90E0821B930B541710F0CF96089563\
:1002D00001970895FC01918D828D981761F0828DB0\
:1002E000DF01A80FB11D5D968C91928D9F5F9F736A\
:1002F000928F90E008958FEF9FEF0895FC01918D0C\
:10030000828D981731F0828DE80FF11D858D90E078\
:1003100008958FEF9FEF0895FC01918D228D892F15\
:1003200090E0805C9F4F821B91098F7399270895FD\
:100330008AE191E00E948C0121E0892B09F420E000\
:10034000822F0895FC01848DDF01A80FB11DA35AEF\
:10035000BF4F2C91848D90E001968F739927848FE5\
:10036000A689B7892C93A089B1898C918370806408\
:100370008C93938D848D981306C00288F389E02DA9\
:1003800080818F7D80830895EF92FF920F931F935A\
:10039000CF93DF93EC0181E0888F9B8D8C8D981338\
:1003A00005C0E889F989808185FD26C0F62E0B8D70\
:1003B00010E00F5F1F4F0F731127E02E8C8DE81296\
:1003C0000CC00FB607FCFACFE889F989808185FF58\
:1003D000F5CFCE010E94A201F1CF8B8DFE01E80F77\
:1003E000F11DE35AFF4FF0829FB7F8940B8FEA8913\
:1003F000FB89808180620AC09FB7F894EE89FF89EB\
:100400006083E889F98980818370806480839FBFDD\
:1004100081E090E0DF91CF911F910F91FF90EF90DD\
:100420000895CF93DF93EC01888D8823C9F0EA8982\
:10043000FB89808185FD05C0A889B9898C9186FDDD\
:100440000FC00FB607FCF5CF808185FFF2CFA889DA\
:10045000B9898C9185FFEDCFCE010E94A201E7CF33\
:10046000DF91CF91089580E090E0892B29F00E94E0\
:10047000980181110C94000008950E94C4031F92FA\
:100480000F920FB60F9211242F933F938F939F9348\
:10049000AF93BF938091B8019091B901A091BA0137\
:1004A000B091BB013091B70123E0230F2D3720F429\
:1004B0000196A11DB11D05C026E8230F0296A11DBE\
:1004C000B11D2093B7018093B8019093B901A09317\
:1004D000BA01B093BB018091BC019091BD01A09184\
:1004E000BE01B091BF010196A11DB11D8093BC0159\
:1004F0009093BD01A093BE01B093BF01BF91AF9196\
:100500009F918F913F912F910F900FBE0F901F9051\
:1005100018951F920F920FB60F9211242F933F93AD\
:100520004F935F936F937F938F939F93AF93BF93FB\
:10053000EF93FF938AE191E00E94A201FF91EF9176\
:10054000BF91AF919F918F917F916F915F914F91EB\
:100550003F912F910F900FBE0F901F9018951F92F3\
:100560000F920FB60F9211242F938F939F93EF93B7\
:10057000FF93E0912A01F0912B018081E0913001FD\
:10058000F091310182FD12C09081809133018F5F23\
:100590008F7320913401821751F0E0913301F0E024\
:1005A000E65EFE4F958F8093330101C08081FF91FD\
:1005B000EF919F918F912F910F900FBE0F901F90F1\
:1005C0001895789484B5826084BD84B5816084BDBB\
:1005D00085B5826085BD85B5816085BD80916E00E1\
:1005E000816080936E001092810080918100826012\
:1005F000809381008091810081608093810080914F\
:1006000080008160809380008091B100846080933D\
:10061000B1008091B00081608093B00080917A0039\
:10062000846080937A0080917A00826080937A005F\
:1006300080917A00816080937A0080917A0080684E\
:1006400080937A001092C100EDE9F0E02491E9E88E\
:10065000F0E08491882399F090E0880F991FFC01C5\
:10066000E859FF4FA591B491FC01EE58FF4F8591D9\
:1006700094918FB7F894EC91E22BEC938FBFE091BB\
:100680002A01F0912B0182E08083E0912601F09114\
:1006900027011082E0912801F091290180E18083F7\
:1006A00010923201E0912E01F0912F0186E08083BB\
:1006B000E0912C01F0912D01808180618083E09197\
:1006C0002C01F0912D01808188608083E0912C01C4\
:1006D000F0912D01808180688083E0912C01F09160\
:1006E0002D0180818F7D808381E00E9487000E94A0\
:1006F000F50080E00E9487000E94F50044E050E091\
:1007000062E171E08AE191E00E942D0142E050E057\
:1007100067E171E08AE191E00E942D010E943302BD\
:10072000E3CFEAE1F1E01382128288EE93E0A0E0E9\
:10073000B0E084839583A683B78384E091E09183BE\
:10074000808385EC90E09587848784EC90E09787A0\
:10075000868780EC90E0918B808B81EC90E0938B8E\
:10076000828B82EC90E0958B848B86EC90E0978B6B\
:10077000868B118E128E138E148E0895EE0FFF1F2E\
:100780000590F491E02D099481E090E0F8940C94A8\
:06079000C903F894FFCF3D\
:1007960000000000C4012D015B0111028C016A01F9\
:0A07A6007E016E696365000D0A0014\
:00000001FF"};


#ifdef _DEBUG_

void IntelHex::parse_intel_hex_from_flash() {

  Serial.begin(9600);

  uint8_t *p = &blink_hex[0];
  uint8_t step_in_file = hex_file_line_length;

  for (uint16_t i = 0;; ++i, p += step_in_file) {
    uint8_t page_size = hex2int(p, 1, 2);
    if (page_size * 2 + metadata_length < hex_file_line_length)
      step_in_file = page_size * 2 + metadata_length;

    debug("step: ");
    debug2(step_in_file, DEC);

    debug("    size: ");
    debug2(page_size, HEX);

    uint32_t address = hex2int(p, 3, 4);
    debug("    address: ");

    if (address <= 0xF)
      debug("000");
    else if (address <= 0xFF)
      debug("00");
    else if (address <= 0x0FFF)
      debug("0");
    debug2(address, HEX);

    uint8_t record_type = hex2int(p, 7, 2);
    debug("    type: ");
    debug2(record_type, HEX);

    debug("    payload: ");
    if (0 == page_size &&
        Intel_Hex_EndOfFile == static_cast<TRecordType>(record_type)) {
      debug("FF");
    } else {
      for (int j = 0; j < page_size * 2; j += 4) {
        uint32_t payload = hex2int(p, 9 + j, 4);

        if (payload <= 0xF)
          debug("000");
        else if (payload <= 0xFF)
          debug("00");
        else if (payload <= 0x0FFF)
          debug("0");
        debug2(payload, HEX);
        debug(" ");
      }
    }
    debugn("");

    if (0 == page_size)
      break; // end
  }
}
#endif // _DEBUG_

void IntelHex::write_to_eeprom_i2c_old(uint8_t eeprom_i2c_address,
                                       uint16_t destination_start_byte) {
  E2PROM e(eeprom_i2c_address);
  uint16_t total_size = sizeof(blink_hex) / sizeof(blink_hex[0]);
  Serial.begin(9600);
  Serial.print("Total example intel hex file raw size: ");
  Serial.print(total_size);
  Serial.println(" bytes.");
  Serial.println(">>> start i2c write");

  uint8_t step_in_file = hex_file_line_length;
  uint8_t current_page = 0;

  for (uint16_t position_in_file = 0; position_in_file < total_size;
       position_in_file += step_in_file, ++current_page) {
    uint8_t page_size = hex2int(&blink_hex[position_in_file], 1, 2);
    uint8_t payload_position_in_page = 9;
    uint8_t crc_position_in_page = 41;

    if (page_size * 2 + metadata_length < hex_file_line_length)
      step_in_file = page_size * 2 + metadata_length;

    char buffer[E2PROM::eeprom_page_size];
    memset(buffer, 0, E2PROM::eeprom_page_size);

    for (uint8_t j = 0; j < E2PROM::eeprom_page_size - 1; ++j) {
      buffer[j] = pgm_read_byte(
          &blink_hex[position_in_file + payload_position_in_page + j]);
      // e.writeByte(current_page*E2PROM::eeprom_page_size+j, buffer[j]);
    }
    uint8_t crc_hi_nibble =
        pgm_read_byte(&blink_hex[position_in_file + crc_position_in_page]);
    e.writeByte(current_page * E2PROM::eeprom_page_size +
                    E2PROM::eeprom_page_size,
                crc_hi_nibble);
    uint8_t crc_lo_nibble =
        pgm_read_byte(&blink_hex[position_in_file + crc_position_in_page + 1]);
    e.writeByte(current_page * E2PROM::eeprom_page_size +
                    E2PROM::eeprom_page_size,
                crc_lo_nibble);
  }
  Serial.println(">>> end i2c write");
}

void IntelHex::write_vector(uint8_t eeprom_i2c_address,
                            uint16_t destination_start_byte, uint8_t *vector,
                            uint8_t size_in_bytes) {
  E2PROM e(0x50);
#ifdef _DEBUG_
  Serial.print("Writing to ");
  Serial.print(eeprom_i2c_address, HEX);
  Serial.print(":   @");
  Serial.print(destination_start_byte);
  Serial.print("    :");
#endif
  for (uint8_t i = 0; i < size_in_bytes; ++i) {
    e.writeByte(destination_start_byte + i, vector[i]);
#ifdef _DEBUG_
    Serial.print("    ");
    Serial.print(vector[i], HEX);
#endif
  }
}

void IntelHex::write_variable(uint8_t eeprom_i2c_address,
                              uint16_t destination_start_byte, uint8_t *data,
                              uint8_t size_in_bytes) {
  E2PROM e(0x50);
#ifdef _DEBUG_
  Serial.print(":Writing to ");
  Serial.print(eeprom_i2c_address, HEX);
  Serial.print(":   @");
  Serial.print(destination_start_byte);
  Serial.print("    :");
#endif
  uint8_t j = 0;
  for (int8_t i = size_in_bytes - 1; i >= 0; --i, ++j) {
    e.writeByte(destination_start_byte + j, data[i]);
#ifdef _DEBUG_
    Serial.print("    ");
    Serial.print(data[i], HEX);
#endif
  }
}

uint32_t IntelHex::GetValueFromSerial(char *message, uint8_t base) const {
  String inString = "";
  uint32_t result = 0;
  bool ready = false;

  Serial.print(message);
  do {
    while (Serial.available() > 0) {
      uint8_t inChar = Serial.read();
      if (10 == base && isDigit(inChar)) {
        inString += static_cast<char>(inChar);
      } else if (16 == base && isHexadecimalDigit(inChar)) {
        inString += static_cast<char>(inChar);
      }
      if ('\n' == inChar) {
        if (10 == base)
          result = inString.toInt();
        else if (16 == base)
          result = strtol(inString.c_str(), 0, 16);
        ready = true;
      }
    }
  } while (!ready);
  return result;
}
void IntelHex::write_preamble(uint8_t eeprom_i2c_address,
                              uint16_t destination_start_byte) {
  uint8_t preamble[] = "miniboot";
  uint8_t base10 = 10, base16 = 16;
  write_vector(eeprom_i2c_address, destination_start_byte, preamble,
               sizeof(preamble) / sizeof(preamble[0]) - 1);

  uint8_t application_name[] = "blink\xFF\xFF\xFF\xFF\xFF";
  write_vector(eeprom_i2c_address, destination_start_byte + 8, application_name,
               sizeof(application_name) / sizeof(application_name[0]) - 1);

  uint32_t application_timestamp =
      GetValueFromSerial("Application unix timestamp (decimal): ", base10);
  write_variable(eeprom_i2c_address, destination_start_byte + 18,
                 reinterpret_cast<uint8_t *>(&application_timestamp),
                 sizeof(application_timestamp));

  uint32_t current_time =
      GetValueFromSerial("Current unix timestamp (decimal): ", base10);
  write_variable(eeprom_i2c_address, destination_start_byte + 22,
                 reinterpret_cast<uint8_t *>(&current_time),
                 sizeof(current_time));

  uint32_t crc = GetValueFromSerial("CRC32 (hex): ", base16);
  write_variable(eeprom_i2c_address, destination_start_byte + 26,
                 reinterpret_cast<uint8_t *>(&crc), sizeof(crc));

  last_written_file_length_ = static_cast<uint16_t>(
      GetValueFromSerial("Application size (bytes, decimal): ", base10));
  write_variable(eeprom_i2c_address, destination_start_byte + 30,
                 reinterpret_cast<uint8_t *>(&last_written_file_length_),
                 sizeof(last_written_file_length_));
}

void IntelHex::write_to_eeprom_i2c(uint8_t eeprom_i2c_address,
                                   uint16_t destination_start_byte) {

  Serial.println("-- Miniboot EEPROM uploader --");
  Serial.println("");

  write_preamble(eeprom_i2c_address, destination_start_byte);

  Serial.println("");
  Serial.print("Header written. Please switch to binary mode and send a file "
               "of byte length ");
  Serial.print(last_written_file_length_);
  Serial.println(".");
  E2PROM e(eeprom_i2c_address);

  uint16_t pos = 32 + destination_start_byte;

  do {
    if (Serial.available() > 0) {
      char incomingByte = Serial.read(); // read the incoming byte:
      e.writeByte(pos++, incomingByte);
    }

  } while (1);
}
