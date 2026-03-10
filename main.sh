sed 's/\s*;.*//;/^$/d' snake.asm
nasm snake.asm -o snake.com
hexdump snake.com
qrencode -8 -r snake.com -t UTF8 -m 2
echo "main;" > small.c
gcc -w small.c -o small
ls -l small
gcc -Os -nostartfiles -nodefaultlibs --entry 0 -Wl,--strip-all -xc /dev/null -o small
ls -l small
ls -l snake.com
