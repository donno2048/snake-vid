# snake-vid

[This](https://youtu.be/Wv1MnvnvAiY) is a demo for the demo-ing program I made ([main.cpp](./main.cpp)), which let's you create mp4 from bash scripts ([main.sh](./main.sh)).

Currently in a demo stage...

The demo generates a video about my [snake game](https://github.com/donno2048/snake).

The reason for not using it's snake.asm directly is that the emulation platform I use, `emu2` doesn't support the `xadd` instruction.

The snake.asm file used to generate the video compiles to the same size binary, and is almost identical to the original.

## TODO
- use tempfiles (fr.txt, main.cast, main.raw, main.gif)
- get bash file and voices from command line
- make wrapper program that turns output into mp4
