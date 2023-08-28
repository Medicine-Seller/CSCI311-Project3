#include "soc.h"
#include "game.h"

// Extract GameInfo structure into a string
std::string GetGameInfoStream(SGameInfo gameInfo);

// Get a random option "r,p,s"
char GetComputerOption();

// Check if option1 beats option2
bool IsWinner(char option1, char option2);

// Check for game over condition
bool IsGameOver(SGameInfo& gameInfo);

// Get round winner
int GetRoundResult(char option1, char option2);

// Get game winner
int GetGameWinner(SGameInfo& gameInfo);

// Add point to winner
void AddPoint(SGameInfo& gameInfo);

// Initialize GameInfo structure
void InitGame(SGameInfo& gameInfo);

int main(int argc, char* argv[])
{
    // Set random seed
    srand(time(NULL));

    pid_t pid = getpid();
    printf("[ServerG] [PID: %d] created\n", pid);

    // Retrieve socket
    int fdClientSocket = atoi(argv[1]);

    // Initialize game
    SGameInfo gameInfo;
    InitGame(gameInfo);

    SendMessage(fdClientSocket, CreateInstruction("game_start", GetGameInfoStream(gameInfo)).c_str());

    while (true)
    {
        // Receive message from ServerG
        char readBuffer[BUFFERLENGTH+1];
        ReceiveMessage(fdClientSocket, readBuffer);
        printf("[ServerG] [PID: %d] readBuffer: %s\n", pid, readBuffer);

        // Retrieve first word
        std::stringstream ssReadBuffer(readBuffer);
        std::string str;
        ssReadBuffer >> str;

        if (strcmp(str.c_str(), "game_player_ready") == 0)
        {
            SendMessage(fdClientSocket, CreateInstruction("game_play", GetGameInfoStream(gameInfo)).c_str());
        }
        else if (strcmp(str.c_str(), "game_player_option") == 0)
        {
            // Extract player choice
            char playerChoice;
            ssReadBuffer >> playerChoice;
            gameInfo.optionPlr = playerChoice;
            gameInfo.optionComp = GetComputerOption();

            AddPoint(gameInfo);

            std::string instruction = "game_round_result";

            // If game is over, set winner and send "game_over"
            if (IsGameOver(gameInfo))
            {
                gameInfo.winner = GetGameWinner(gameInfo);
                instruction = "game_over";
            }
            SendMessage(fdClientSocket, CreateInstruction(instruction, GetGameInfoStream(gameInfo)).c_str());
            gameInfo.round++;
        }
        else if (strcmp(str.c_str(), "game_close") == 0)
        {
            break;
        }

    }

    // Connect to serverC and send message "serverg_exit"
    int fdServerSocket = CreateSocket();
    sockaddr_in serverAddr = ConnectToServer(fdServerSocket);
    SendMessage(fdServerSocket, "serverg_exit");

    close(fdClientSocket);
    printf("[ServerG] [PID: %d] exited\n", pid);
    return 0;
}


std::string GetGameInfoStream(SGameInfo gameInfo)
{
    std::stringstream ss;
    for (int i = 0; i < sizeof(SGameInfo)/sizeof(int); i++)
        ss << *((int*)&gameInfo + i) << " ";

    return ss.str();
}

char GetComputerOption() 
{
    int num = rand() % 3 + 1;
    char option;

    if (num == 1)
        option = c_rock;
    else if (num == 2) 
        option = c_paper;
    else if (num == 3) 
        option = c_scissor;

    return option;
}

bool IsWinner(char option1, char option2)
{
    return (option1 == c_rock && option2 == c_scissor) || 
           (option1 == c_paper && option2 == c_rock)   ||
           (option1 == c_scissor && option2 == c_paper);
}

bool IsGameOver(SGameInfo& gameInfo)
{
    return (gameInfo.winPlrCount >= gameInfo.maxWinRound  || 
            gameInfo.winCompCount >= gameInfo.maxWinRound ||
            gameInfo.round >= gameInfo.maxRound);
}

int GetRoundResult(char option1, char option2)
{
    int result;

    if (IsWinner(option1, option2))
        result = WIN_PLAYER;
    else if (IsWinner(option2, option1))
        result = WIN_COMPUTER;
    else
        result = TIE;

    return result;
}

int GetGameWinner(SGameInfo& gameInfo)
{
    int winner = TIE;

    if (gameInfo.winPlrCount > gameInfo.winCompCount)
        winner = WIN_PLAYER;
    else if (gameInfo.winPlrCount < gameInfo.winCompCount)
        winner = WIN_COMPUTER;

    return winner;
}   

void AddPoint(SGameInfo& gameInfo)
{
    switch(GetRoundResult(gameInfo.optionPlr, gameInfo.optionComp))
    {
        case WIN_PLAYER: 
            gameInfo.winPlrCount++; 
            gameInfo.roundWinner = WIN_PLAYER;
            break;

        case WIN_COMPUTER: 
            gameInfo.winCompCount++; 
            gameInfo.roundWinner = WIN_COMPUTER;
            break;

        default:
            gameInfo.roundWinner = TIE;
    }
}

void InitGame(SGameInfo& gameInfo)
{
    memset(&gameInfo, 0, sizeof(SGameInfo));

    int randmaxRound = rand() % 4;
    int maxRound;

    switch (randmaxRound)
    {
        case 0: maxRound = 3; break;
        case 1: maxRound = 5; break;
        case 2: maxRound = 7; break;
        case 3: maxRound = 9; break;
        default: maxRound = 3;
    }

    gameInfo.maxRound = maxRound;
    gameInfo.maxWinRound = maxRound / 2 + 1;
    gameInfo.round = 1;
}