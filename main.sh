#This video shows the extreme level of optimization you can achieve with x86 assembly, by showcasing my fifty-four bytes snake game.
#First, let's take a look at the code. I'll print it without the comments so it's easier to view.
sed 's/\s*;.*//;/^$/d' snake.asm
#Now let's assemble the game using NASM.
nasm snake.asm -o snake.com
#Here's the full hex dump of the game. As you can see... it's very small.
hexdump snake.com
#In fact, it's so small that it can fit inside a single QR code.
qrencode -8 -r snake.com -t UTF8 -m 2
#But how small is it really? To get some perspective, let's create a C program that does absolutely nothing, and only contains a declaration for the main method.
echo "main;" > small.c
#Now let's compile it.
gcc -w small.c -o small
#If we check the size of the compiled program, we can see it's around fifteen thousand bytes.
ls -l small
#This happens because the compiler automatically adds extra code that every program needs in order to run correctly. I won't go into the details here, since that would take an entire separate video.
#But if we force the compiler to remove as much extra data as possible, we can reach the smallest binary it is willing to generate.
gcc -Os -nostartfiles -nodefaultlibs --entry 0 -Wl,--strip-all -xc /dev/null -o small
#That ends up being around nine thousand bytes
ls -l small
#However, the program I made is only fifty-four bytes.
ls -l snake.com
#This project was inspired by a video from MattKC, who also tried to create a snake game that fits inside a QR code.
