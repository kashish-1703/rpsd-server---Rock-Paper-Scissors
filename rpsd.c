#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "network.h"
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>

//MACROS
#define WAITING_PLAYERS 16  //maximum number of players that can wait in the queue to be accepted by the server 
#define NAME_LEN 64 //max size a players name can be

//STRUCTS
/*Player: 
    stores information on 
    players name, move, and turn
    for each game
*/
typedef struct {
    int fd; //file descriptr
    char player_name[NAME_LEN]; //players name gets stored
    char player_move[10]; //stores the players moves played during the round
    bool player_turn; //keeps track of players turn
} Player;


//HELPER METHODS
/*Message:
    either recieves messages from player
        P: play, takes players name as argument 
        M: move, takes players move as argument
        C: continue
        Q: quit
    OR sends messages to player
        W: wait, sends 1 (version number) for successful connection
        B: begin, takes opposing players name as argument
        R: result, W for win, L for lose, D for draw, or F for forfeit
*/
bool message(int fd, char *buf, int buff_len, bool send_msg) {
    if (send_msg){
        write(fd, buf, strlen(buf)); // writing message to player
        return true;
    } else{//taking in players message
        int len = 0;
        while (len < buff_len - 1){
            int ret = read(fd, buf + len, 1);
            if (ret <= 0){
                return false;  // Disconnection = forfeit (optional)
            }
            len++;

            // Exit when a complete message is received
            if ((len == 1 && (buf[0] == 'Q' || buf[0] == 'C')) || (len >= 2 && buf[len - 2] == '|' && buf[len - 1] == '|')){
                break;
            }
        }
        buf[len] = '\0';
        return true;
    }
}

bool valid_move(const char *move) {
    return strcmp(move, "ROCK") == 0 ||
           strcmp(move, "PAPER") == 0 ||
           strcmp(move, "SCISSORS") == 0;
}


void to_uppercase(char *str) { //converts strings to uppercase
    for (int i = 0; str[i] != '\0'; i++){
        str[i] = toupper(str[i]);
    }
}

const char *find_result( char *player, char *opponent){
    //avoids letter casing comparisions
    to_uppercase(player);
    to_uppercase(opponent);


    if (strcmp(player, opponent) == 0){ //check for draw
        return "D";
    }
    //determining winner and loser of round
    if ((strcmp(player, "ROCK") == 0 && strcmp(opponent, "SCISSORS") == 0) || (strcmp(player, "PAPER") == 0 && strcmp(opponent, "ROCK") == 0) || (strcmp(player, "SCISSORS") == 0 && strcmp(opponent, "PAPER") == 0)){
        return "W";
    }
    return "L";
}

/*Single Game:
    a singular game of RPS between 2 players
*/
void single_game(Player p1, Player p2){
    //prompting begin game messages to both players
    char b1[NAME_LEN +8], b2[NAME_LEN +8];
    snprintf(b1, sizeof(b1), "B|%s||", p2.player_name);
    snprintf(b2, sizeof(b2), "B|%s||", p1.player_name);
    message(p1.fd, b1, NAME_LEN, true);
    message(p2.fd, b2, NAME_LEN, true);
    
    bool active_game = true; // is true until a player quits or forfeits game
    while(active_game == true){ // keep playing until a player decides to quit/forfeit
        //getting the players moves
        for (int i = 0; i < 2; i++){
            char buf[NAME_LEN];
            // bool mssg_check;
            
            if (i == 0) { // player 1
                if (!message(p1.fd, buf, NAME_LEN, false) || strncmp(buf, "M|", 2) != 0) {
                    message(p2.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                }
                sscanf(buf, "M|%[^|]", p1.player_move);
                to_uppercase(p1.player_move);
                if (!valid_move(p1.player_move)) {
                    message(p2.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                }
            } else { // player 2
                if (!message(p2.fd, buf, NAME_LEN, false) || strncmp(buf, "M|", 2) != 0) {
                    message(p1.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                }
                sscanf(buf, "M|%[^|]", p2.player_move);
                to_uppercase(p2.player_move);
                if (!valid_move(p2.player_move)) {
                    message(p1.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                }
            }
        }

        if (active_game != true){ //messages that don't start with M| automatically end the game
            break;
        }

        //finding out the result of the match
        const char *r1 = find_result(p1.player_move, p2.player_move);
        const char *r2 = find_result(p2.player_move, p1.player_move);

        // sending results to the players
        char res1[NAME_LEN], res2[NAME_LEN];
        snprintf(res1, sizeof(res1), "R|%s|%s||", r1, p2.player_move);
        snprintf(res2, sizeof(res2), "R|%s|%s||", r2,p1.player_move);
        message(p1.fd, res1, NAME_LEN,true);
        message(p2.fd, res2, NAME_LEN,true); 

/*
        bool continue1 = false, continue2 = false; //checking if BOTH players want to continue
        for (int i = 0; i < 2; i++){
            char buf[NAME_LEN];
            bool mssg_check;
            if (i == 0){ //first checking for player 1
                mssg_check = message(p1.fd, buf, NAME_LEN, false);
                if (!mssg_check || strncmp(buf, "Q", 1) == 0){
                    active_game = false;
                    break;
                } else if (strncmp(buf, "C", 1) == 0){
                    continue1 = true;
                }
            } else{ //checks player 2
                mssg_check = message(p2.fd, buf, NAME_LEN, false);
                if (!mssg_check || strncmp(buf, "Q", 1) == 0){
                    active_game = false;
                    break;
                } else if (strncmp(buf, "C", 1) == 0){
                    continue2 = true;
                }
            }
        }
*/

///

        // Ask if players want to continue
        bool continue1 = false, continue2 = false;
        for (int i = 0; i < 2; i++) {
            char buf[NAME_LEN];
            bool mssg_check;
            if (i == 0) { // player 1
                mssg_check = message(p1.fd, buf, NAME_LEN, false);
                if (!mssg_check) {
                    message(p2.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                } else if (strcmp(buf, "Q||") == 0) {
                    active_game = false;
                    break;
                } else if (strcmp(buf, "C||") == 0) {
                    continue1 = true;
                } else {
                    active_game = false;
                    break;
                }
            } else { // player 2
                mssg_check = message(p2.fd, buf, NAME_LEN, false);
                if (!mssg_check) {
                    message(p1.fd, "R|F|||", NAME_LEN, true);
                    close(p1.fd);
                    close(p2.fd);
                    exit(EXIT_SUCCESS);
                } else if (strcmp(buf, "Q||") == 0) {
                    active_game = false;
                    break;
                } else if (strcmp(buf, "C||") == 0) {
                    continue2 = true;
                } else {
                    active_game = false;
                    break;
                }
            }
        }

///
        // if both sent C, loop again; else end
        if (!(continue1 && continue2)) {
            active_game = false;
        }
    }

    // exiting the game and clearing players
    close(p1.fd);
    close(p2.fd);
    exit(EXIT_SUCCESS);
}

//handling zombie (basically unneeded child proccesses are exited)
void handle_sigchld(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char **argv){

    if (argc != 2){
        fprintf(stderr, "Error: %s\n", argv[0]);
        return EXIT_FAILURE;
    }

    int input_taker = open_listener(argv[1], WAITING_PLAYERS);
    if (input_taker < 0){
        return EXIT_FAILURE;
    }

    signal(SIGCHLD, handle_sigchld); // handles dead/zombie players

    while(1){
        Player player_one;
        Player player_two;
        memset(&player_one, 0, sizeof(Player));
        memset(&player_two, 0, sizeof(Player));

        

        int players_connected = 0;
        while (players_connected < 2){
            struct sockaddr_storage rem;
            socklen_t len = sizeof(rem);
            int fd = accept(input_taker, (struct sockaddr *)&rem, &len);
            if (fd < 0){
                perror("accept");
                continue;
            }
            
            char buff[NAME_LEN];

            if (!message(fd, buff, NAME_LEN, false)) {
                // player disconnected immediately
                close(fd);
                continue;
            }
            //message(fd, buff, NAME_LEN, false);

            if (strncmp(buff, "P|", 2) == 0){
                if (players_connected == 0){
                    sscanf(buff, "P|%[^|]", player_one.player_name);
                    player_one.fd = fd;

                    if (!message(fd, "W|1||", NAME_LEN, true)) {
                        // player 1 disconnected at welcome message
                        close(fd);
                        memset(&player_one, 0, sizeof(Player));
                        continue;
                    }

                    //message(fd, "W|1||", NAME_LEN, true);
                    players_connected++;
                }
                else if (players_connected == 1){
                    sscanf(buff, "P|%[^|]", player_two.player_name);
                    player_two.fd = fd;
                    
                    if (!message(fd, "W|1||", NAME_LEN, true)) {
                        //player 2 disconnected at welcome
                        close(fd);
                        memset(&player_two, 0, sizeof(Player));
                        continue;
                    }
                    
                    //message(fd, "W|1||", NAME_LEN, 1);
                    players_connected++;
                }
                else{
                    close(fd);
                }
            }
        }

        pid_t pid = fork();
        if (pid == 0){
            close(input_taker);
            single_game(player_one, player_two);
        }
        else if (pid > 0){
            close(player_one.fd);
            close(player_two.fd);
        }
        else{
            perror("error: fork");
            close(player_one.fd);
            close(player_two.fd);
            close(input_taker);
            exit(EXIT_FAILURE);
        }
    }

    close(input_taker);
    return EXIT_SUCCESS;
}