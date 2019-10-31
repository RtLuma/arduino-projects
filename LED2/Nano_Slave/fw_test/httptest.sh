#!/usr/bin/bash
curl -s -X DELETE 192.168.0.69/fs/fw.eeprom
curl -s -X POST -F 'data=@fw.eeprom' 192.168.0.69
curl -s -X GET 192.168.0.69/fs/fw.eeprom --output test.bin
read S1 _ <<<"`sha1sum fw.eeprom`"
read S2 _ <<<"`sha1sum test.bin`"
if [[ $S1 == $S2 ]]
	then echo nice
	else echo not nice
fi

curl -s -X GET 192.168.0.69/fw --output test.bin
read S1 _ <<<"`sha1sum fw.eeprom`"
read S2 _ <<<"`sha1sum test.bin`"
if [[ $S1 == $S2 ]]
	then echo nice
	else echo not nice
fi

