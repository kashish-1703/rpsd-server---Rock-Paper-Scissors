# rpsd-server - Rock-Paper-Scissors
## Project Description
A concurrent game server in C using sockets

The system includes:
- 'rpsd': a multi - client TCP server that manages players, game sessions, and protocol handling
- 'rc': a test client to interact with the server via the custom protocol
- 'gc': a secondary test client for evaluation purposes

The project emphasizes modularity and real - time concurrency using 'select()' for managing multiple client connections simultaneously.

## Features
- Designed with modularity in mind
- Implements a custom protocol for communication, very specific to this game
- Detects and handles disconnections
- Manages client state: login, matchmaking, move submission, and result display (specific to this game)
- Supports concurrent games between multiple players
- Includes test client code for evaluation (rc, gc)

## Building the Project

A 'Makefile' is included to simplify compilation.

To build both the server and the test clients:

```bash
make

> Note: 'network.c', 'network.h', 'gc', and 'rc' were starter files provided by the course staff. All other logic and modifications were implemented by me.
