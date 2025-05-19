# rpsd-server - Rock-Paper-Scissors
## Project Description
A concurrent game server in C using sockets

The system includes:
- A multi - client TCP server (called rpsd)
- A test client implementation (called rc) to interact with the server

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
