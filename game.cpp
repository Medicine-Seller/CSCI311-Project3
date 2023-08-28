#include "game.h"

std::string CreateInstruction(std::string instruction, std::string parameters)
{
    if (parameters == "")
        return instruction.c_str();

    return instruction + " " + parameters;
}

std::string GetOptionString(char option) 
{
    std::string str;

    if (option == c_rock) 
        str = "Rock";
    else if (option == c_paper) 
        str = "Paper";
    else if (option == c_scissor) 
        str = "Scissor";

    return str;
}

std::string GetWinnerString(int winner)
{
    std::string str;
    switch(winner)
    {
        case WIN_PLAYER: str = "PLAYER"; break;
        case WIN_COMPUTER: str = "COMPUTER"; break;
        default: str = "TIE"; break;
    }

    return str;
}


