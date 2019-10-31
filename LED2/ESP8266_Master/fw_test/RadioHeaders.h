#pragma once

enum RadioHeader : uint8_t {
    
    Null          ,
    Mode          ,
    PayloadSize   ,
    FirmwareBegin ,
    Firmware      ,
    FirmwareEnd   ,
    Health        ,
    
    LENGTH
};

