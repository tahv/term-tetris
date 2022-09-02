# term-tetris

A Tetris game written in C using `ncurses`.

This is a side project to learn / experiment with the C language.

![gameplay](docs/gameplay.gif)

## Installation

Install dependencies

```bash
sudo apt install libncursesw5-dev
```

Compile and run

```bash
make all && ./tetris
```

## Commands

|             |            |
| ----------- | ---------- |
| left arrow  | move left  |
| right arrow | move right |
| up arrow    | rotate     |
| down arrow  | soft drop  |
<!--
| space       | hard drop  |
| c           | hold       |
-->


## Features

From the [Tetris Guidelines](https://tetris.wiki/Tetris_Guideline)

- [X] [Ghost piece](https://tetris.wiki/Ghost_piece)
- [ ] Scoring
- [X] [Marathon](https://tetris.wiki/Marathon) speed curve
- [ ] [Super rotation system](https://tetris.wiki/Super_Rotation_System)
- [X] [Random Generator](https://tetris.wiki/Random_Generator) for sequence of tetrominos
- [ ] [Hold](https://tetris.wiki/Tetris_Guideline) current piece
- [ ] [Lock Delay](https://tetris.wiki/Lock_delay)
- [ ] Hard Drop
- [X] Next piece display

