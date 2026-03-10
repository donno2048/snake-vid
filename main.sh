#This video shows the extreme optimization possible using X86 assembly specifically using by showing a snake game I made, I believe is the smallest snake game ever written.
#First, let's look at the code, I'll print it stripped from comment to make it easier to view.
sed 's/\s*;.*//;/^$/d' snake.asm
#Let's assemble the game using nasm.
nasm snake.asm -o snake.com
#Here's the full hex dump of the game. As you can see, it's quite small.
hexdump snake.com
#So small, in fact, that it could fit in a single QR code.
qrencode -8 -r snake.com -t UTF8 -m 2
#Let's see how small it really is. To do so, let's make a C program that does nothing and contains only a declaration for the main method.
echo "main;" > small.c
#If we compile this program
gcc -w small.c -o small
#And look how much it weighs, we can see it's around fifteen thousand bytes.
ls -l small
#This is because the compiler adds some things to every program it has to have in order to be able to run correctly, I won't go into the details as it would take another full video. If we try to make the compiler not include any additional data in the binary and compile, we will get the minimal size for a binary file the compiler is willing to create
gcc -Os -nostartfiles -nodefaultlibs --entry 0 -Wl,--strip-all -xc /dev/null -o small
#Which is around nine thousand bytes
ls -l small
#However, the program I made is only fifty-four bytes.
ls -l snake.com
#The project was inspired by MattKC's video, who also tried to make a snake game that fits in a QR code.
