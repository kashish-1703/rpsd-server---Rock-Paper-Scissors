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
```

## Acknowledgements
This project was completed as part of a systems programming assignment at Rutgers University.

The following files and folders were provided as starter code by the course staff:
- 'test_client_r2/'
- 'network.c'
- 'network.h'

All other server logic, including the design and implementation of the 'rpsd' concurrent server, client management, game protocol, and match handling, were developed collaboratively by the project partners.

## Author

**Kashish Patel**
Computer Science & ITI Major, Rutgers University
GitHub: [@kashish-1703](https://github.com/kashish-1703)

**Tushti Patel**
Computer Science & ITI Major, Rutgers University
Github:

## License

This project is licensed for **educational and personal use only**.
It may not be distributed, submitted, or published as part of another academic course.

> Portions of the code (specifically 'network.c', 'network.h', and the 'test_client_r2/' directory) were provided by the course staff for use in the assignment.
