gcc -c -o build/general.o source/general.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/joystick.o source/joystick.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/keyboard.o source/keyboard.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/keys.o source/keys.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/main.o source/main.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/settings.o source/settings.c -Iinclude -fms-extensions -O2 -Wall
gcc -c -o build/wireless.o source/wireless.c -Iinclude -fms-extensions -O2 -Wall
gcc build/general.o build/joystick.o build/keyboard.o build/keys.o build/main.o build/settings.o build/wireless.o -Iinclude -fms-extensions -O2 -Wall lib/vJoyInterface.lib -lws2_32 -lGdi32 -lgdiplus -static-libgcc
PAUSE