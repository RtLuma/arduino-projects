#include "intel_hex.h"
#include "e2prom.h"

#define debug(X) Serial.print(X)
#define debug2(X, Y) Serial.print(X, Y)
#define debugn(X) Serial.println(X)

const uint8_t hex_file_line_length = 43;
const uint8_t metadata_length = 11; // inclues <':'-1 character><size-2
                                    // characters><address-4 characters><type-2
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
    ":100000000C945C000C946E000C946E000C946E00CA\
:100010000C946E000C946E000C946E000C946E00A8\
:100020000C946E000C946E000C946E000C946E0098\
:100030000C946E000C946E000C946E000C946E0088\
:100040000C9416010C946E000C946E000C946E00CF\
:100050000C946E000C946E000C946E000C946E0068\
:100060000C946E000C946E00000000002400270029\
:100070002A0000000000250028002B0004040404CE\
:100080000404040402020202020203030303030342\
:10009000010204081020408001020408102001021F\
:1000A00004081020000000080002010000030407FB\
:1000B000000000000000000011241FBECFEFD8E0B8\
:1000C000DEBFCDBF21E0A0E0B1E001C01D92A930AC\
:1000D000B207E1F70E9460010C94CF010C9400007C\
:1000E000E1EBF0E09491EDE9F0E02491E9E8F0E053\
:1000F000E491EE2309F43CC0992339F1933091F057\
:1001000038F49130A9F0923001F594B59F7D12C07A\
:10011000973091F09830A1F09430B9F4909180002C\
:100120009F7D03C0909180009F77909380000DC0C9\
:1001300094B59F7794BD09C09091B0009F7703C09C\
:100140009091B0009F7D9093B000F0E0EE0FFF1F04\
:10015000EE58FF4FA591B4919FB7F894811104C058\
:100160008C912095282302C0EC912E2B2C939FBFBD\
:1001700008953FB7F8948091050190910601A091F0\
:100180000701B091080126B5A89B05C02F3F19F0C3\
:100190000196A11DB11D3FBFBA2FA92F982F882707\
:1001A000820F911DA11DB11DBC01CD0142E0660F62\
:1001B000771F881F991F4A95D1F708958F929F92B4\
:1001C000AF92BF92CF92DF92EF92FF920E94B9005E\
:1001D0004B015C0188EEC82E83E0D82EE12CF12C77\
:1001E0000E94B900DC01CB0188199909AA09BB0951\
:1001F000883E9340A105B10598F321E0C21AD108C9\
:10020000E108F10888EE880E83E0981EA11CB11C5D\
:10021000C114D104E104F10419F7FF90EF90DF90CD\
:10022000CF90BF90AF909F908F9008951F920F92A4\
:100230000FB60F9211242F933F938F939F93AF93F9\
:10024000BF938091010190910201A0910301B091AF\
:1002500004013091000123E0230F2D3720F4019693\
:10026000A11DB11D05C026E8230F0296A11DB11DD9\
:10027000209300018093010190930201A093030158\
:10028000B09304018091050190910601A0910701AE\
:10029000B09108010196A11DB11D809305019093B5\
:1002A0000601A0930701B0930801BF91AF919F9100\
:1002B0008F913F912F910F900FBE0F901F90189527\
:1002C000789484B5826084BD84B5816084BD85B531\
:1002D000826085BD85B5816085BD80916E0081603D\
:1002E00080936E00109281008091810082608093E3\
:1002F00081008091810081608093810080918000E5\
:100300008160809380008091B10084608093B1000F\
:100310008091B00081608093B00080917A00846009\
:1003200080937A0080917A00826080937A00809135\
:100330007A00816080937A0080917A00806880934F\
:100340007A001092C100EDE9F0E02491E9E8F0E0D4\
:100350008491882399F090E0880F991FFC01E85957\
:10036000FF4FA591B491FC01EE58FF4F85919491F8\
:100370008FB7F894EC91E22BEC938FBFC0E0D0E004\
:1003800081E00E9470000E94DE0080E00E94700008\
:100390000E94DE002097A1F30E940000F1CFF894A4\
:0203A000FFCF8D\
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
