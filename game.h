#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <sstream>
#include <string>

const char c_rock = 'r';
const char c_paper = 'p';
const char c_scissor = 's';

enum GAME_RESULT
{
    TIE,
    WIN_PLAYER,
    WIN_COMPUTER
};

struct SGameInfo
{
    int maxRound;
    int maxWinRound;
    int round;
    int winPlrCount;
    int winCompCount;
    int winner;
    int roundWinner;
    int optionPlr;
    int optionComp;
};

// Combine two string with space padding
std::string CreateInstruction(std::string instruction, std::string parameters = "");

// Get string value of option
std::string GetOptionString(char option);

// Get winner as string
std::string GetWinnerString(int winner);

#endif