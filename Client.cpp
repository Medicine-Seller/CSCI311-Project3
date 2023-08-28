#include "soc.h"
#include "game.h"

// Build structure from data stream
SGameInfo GetGameInfoFromStream(std::stringstream& ss);

// Display and prompt player for option
char GetPlayerOption();

// Print starting game info
void PrintGameBegin(SGameInfo& gameInfo);

// Print round
void PrintRound(SGameInfo& gameInfo);

// Print answers and winner
void PrintRoundResult(SGameInfo& gameInfo);

// Print game result
void PrintGameOver(SGameInfo& gameInfo);

// Print player and computer wins
void PrintWins(SGameInfo& gameInfo);

int main(int argc, char* argv[])
{
    pid_t pid = getpid();

    // Connect to serverC and send message "client_connect"
    int fdServerSocket = CreateSocket();
    sockaddr_in& serverAddr = ConnectToServer(fdServerSocket);
    SendMessage(fdServerSocket, "client_connect");

    printf("%s\n", std::string(80, '-').c_str());
    SGameInfo gameInfo;
    while (true)
    {
        // Receive message from ServerG
        char readBuffer[BUFFERLENGTH+1];
        ReceiveMessage(fdServerSocket, readBuffer);
        //printf("[Client] [PID: %d] readBuffer: %s\n", pid, readBuffer);

        // Retrieve first word
        std::stringstream ssReadBuffer(readBuffer);
        std::string str;
        ssReadBuffer >> str;

        // Extract stream data into gameInfo structure
        gameInfo = GetGameInfoFromStream(ssReadBuffer);

        if (strcmp(str.c_str(), "game_start") == 0)
        {
            PrintGameBegin(gameInfo);
            SendMessage(fdServerSocket, "game_player_ready");
        }
        else if (strcmp(str.c_str(), "game_play") == 0)
        {
            PrintWins(gameInfo);
            PrintRound(gameInfo);
            std::string playerChoice = std::string(1, GetPlayerOption());
            SendMessage(fdServerSocket, CreateInstruction("game_player_option", playerChoice).c_str());
        }
        else if (strcmp(str.c_str(), "game_round_result") == 0)
        {
            PrintRoundResult(gameInfo);
            SendMessage(fdServerSocket, "game_player_ready");
        }
        else if (strcmp(str.c_str(), "game_over") == 0)
        {
            PrintGameOver(gameInfo);
            SendMessage(fdServerSocket, "game_close");
            break;
        }
    }
    printf("%s\n", std::string(80, '-').c_str());

    close(fdServerSocket);
    printf("[Client] [PID: %d] exited\n", pid);
    return 0;
}

SGameInfo GetGameInfoFromStream(std::stringstream& ss)
{
    SGameInfo gameInfo;
    for (int i = 0; i < sizeof(SGameInfo)/sizeof(int); i++)
        ss >> *((int*)&gameInfo + i);

    return gameInfo;
}

char GetPlayerOption() 
{
    char option;
    while (true)
    {
        printf("Your input: ");
        std::cin >> option;

        if (option != c_rock && option != c_paper && option != c_scissor)
        {
            printf("Please enter one of the following options only.\n");
            continue;
        }
        break;
    }

    return option;
}

void PrintGameBegin(SGameInfo& gameInfo)
{
    printf("Game started, number of rounds: %d, rounds to win immediately: %d\n",   gameInfo.maxRound, 
                                                                                    gameInfo.maxWinRound);
    printf("Options are:\n\'r\' for rock\n\'p\' for paper\n\'s\' for scissor\n\n");
}

void PrintRound(SGameInfo& gameInfo)
{
    printf("Round(%d/%d)| ", gameInfo.round, gameInfo.maxRound);
}

void PrintWins(SGameInfo& gameInfo)
{
    printf("Player: %d | Computer: %d\n",   gameInfo.winPlrCount, 
                                            gameInfo.winCompCount);
}

void PrintRoundResult(SGameInfo& gameInfo)
{
    printf("Round Result: Player -> %7s vs %-7s <- Computer, Winner: %s\n\n",  GetOptionString((char)gameInfo.optionPlr).c_str(),
                                                                                GetOptionString((char)gameInfo.optionComp).c_str(),
                                                                                GetWinnerString(gameInfo.roundWinner).c_str());      
}

void PrintGameOver(SGameInfo& gameInfo)
{
    PrintRoundResult(gameInfo);
    PrintWins(gameInfo);
    printf("[Game Over] Winner: %s \n", GetWinnerString(gameInfo.winner).c_str());
}
