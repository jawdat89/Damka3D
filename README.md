
# Damka3D

**Final Project for Practical Software Engineer Program - 2018**

## Overview

Damka3D is a strategic checkers game developed as a final project for the Practical Software Engineer program at Ort Braude College. The project was supervised by Shadi Asaakla and developed by Jawdat Abdallah and Roni Mouadi.

## Project Description

Checkers, also known as Draughts, is a strategy board game played on a board divided into 64 squares. The game involves two players, each with 12 pieces, and the objective is to capture all of the opponent's pieces. Damka3D takes this classic game and enhances it with modern technology by implementing it in a 3D environment using OpenGL. 

## Goals

1. **Develop Social Skills**: Especially aimed at teenagers, playing against a human opponent adds a layer of challenge and promotes social interaction.
2. **Enhanced Visuals**: Upgrade the traditional 2D checkers game to a more engaging 3D experience.
3. **Challenge Players**: Provide a more challenging gameplay experience by allowing two human players to compete against each other.
4. **OpenGL Environment**: Implement the game using OpenGL to create a fully immersive 3D environment.
5. **Improve Online Availability**: Increase the game's presence on the internet by offering an enriched online playing experience.
6. **Team Collaboration**: Work effectively as a team, combining different programming techniques and skills.
7. **Learning New Technologies**: Gain experience and skills in new technologies and methodologies.

## Final Product

The final product is a 3D checkers game that allows for gameplay against both the computer and another human player. The game supports various difficulty levels and features a user-friendly interface with a main menu for setting game options.

## Features

- **3D Environment**: A visually appealing 3D board and pieces.
- **Multiplayer**: Play against another human player over the network.
- **Single Player**: Play against the computer with adjustable difficulty levels.
- **Strategy Development**: Encourages logical thinking and strategy development.

## Game Rules

- Players take turns moving their pieces diagonally across the board.
- Pieces can capture opponent's pieces by jumping over them.
- When a piece reaches the opponent's back row, it is crowned as a "King" and gains the ability to move both forwards and backwards.
- The game ends when a player captures all of the opponent's pieces or blocks all possible moves.

## Technical Details

- **Language**: C++
- **Graphics**: OpenGL, GLUT
- **Networking**: Winsock2
- **Development Environment**: Visual Studio

## Class Diagram

The game consists of the following main classes:

- `Environment`: Responsible for drawing the game environment (room).
- `Table`: Responsible for drawing the table on which the game is played.
- `Checkers`: Manages the game logic and uses other classes for rendering.
- Various enums and structs to manage game states, player turns, piece states, etc.

## Building and Running

To build and run the game, follow these steps:

1. Ensure you have a C++ development environment set up with OpenGL and GLUT libraries.
2. Clone the repository: `git clone https://github.com/jawdat89/Damka3D.git`
3. Open the project in your development environment.
4. Build the project.
5. Run the executable to start the game.

