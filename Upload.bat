echo %0
echo %~s0
echo %~d0%~p0
arduino-cli upload -p COM4 -b esp8266:esp8266:generic -i IR.ino.bin 