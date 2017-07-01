#pragma once

#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <string_view>

// types ----------------------------------------------------------------------

struct CmdInputParams
{
	std::string strPath;
	unsigned int numElementsToPick{ 0 };
	unsigned int numTopLinesToSkip{ 0 };
	int idColumnWithWeights{ -1 };
	char delimiter{ 0 };
};

struct LineEntry
{
	std::string strLine;
	int weight{ 1 };
};

// function headers -----------------------------------------------------------

CmdInputParams ReadInputParams(int argc, const char *argv[]);

LineEntry ExtractLineInformation(std::string_view svLine, int idWeightColumn, char delimiter);

std::vector<LineEntry> ReadAllLines(const std::string& strPath, const CmdInputParams& params);

std::vector<int> BuildFromEntries(const std::vector<LineEntry>& vecEntries);

template <typename ItRandom> void ShuffleWithMT19937(ItRandom itFirst, ItRandom itLast);

std::vector<LineEntry> DrawWinners(const std::vector<LineEntry>& vecInputLines, const std::vector<int>& vecWeightedIndices, unsigned int numElementsToPick);

void ShowWinners(const std::vector<LineEntry>& vecWinners, const std::vector<LineEntry>& vecInputLines, const CmdInputParams& inputParams);

template <typename ItRandom> void ShuffleWithMT19937(ItRandom itFirst, ItRandom itLast)
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(itFirst, itLast, g);
}