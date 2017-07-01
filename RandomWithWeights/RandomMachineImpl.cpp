#include "RandomMachineApp.h"

#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <iostream>
#include <set>
#include <string_view>


// implementation -------------------------------------------------------------


// program.exe inputFile numberOfWinners LinesToSkip idColumnWithWeight ColumnSeparator
CmdInputParams ReadInputParams(int argc, const char *argv[])
{
	CmdInputParams out;

	if (argc > 1)
		out.strPath = argv[1];

	if (argc > 2)
		out.numElementsToPick = ::atoi(argv[2]);

	if (argc > 3)
		out.numTopLinesToSkip = ::atoi(argv[3]);

	if (argc > 4)
		out.idColumnWithWeights = ::atoi(argv[4]);

	if (argc > 5)
		out.delimiter = argv[5][0];

	return out;
}

// helper function for ExtractLineInformation
size_t find_Nth(std::string_view svStr, int occurence, const char delimiter)
{
	if (occurence <= 0)
		return std::string::npos;

	size_t pos, from = 0;
	for (int i = 0; i < occurence; ++i)
	{
		pos = svStr.find(delimiter, from);
		if (pos == std::string::npos)
			break;

		from = pos + 1;
	}

	return pos;
}

LineEntry ExtractLineInformation(std::string_view svLine, int idWeightColumn, char delimiter)
{
	LineEntry out{ std::string(svLine), 1 }; // we want a real copy of the line, explicit constructor works here

	if (idWeightColumn >= 0)
	{
		auto startPos = find_Nth(svLine, idWeightColumn, delimiter);
		if (startPos != std::string::npos)
		{
			const auto svRemainder = svLine.substr(startPos + 1);
			const auto nextPos = svRemainder.find(delimiter);
			const auto svWeight = svRemainder.substr(0, nextPos);

			// string_view doesn't support numerical conversions, so we need to have a string copy
			//std::string strTemp{ svWeight.data(), svWeight.size() }; // copy using data/size
			std::string strTemp{ svWeight }; // copy using explicit constructor
			out.weight = std::stoi(strTemp);
		}
	}

	return out;
}

std::vector<LineEntry> ReadAllLines(const std::string& strPath, const CmdInputParams& params)
{
	std::vector<LineEntry> outVec;
	std::string strLine;
	std::ifstream inFile(strPath);

	if (!inFile)
		throw std::exception("cannot open file...");

	for (unsigned int i = 0; i < params.numTopLinesToSkip && std::getline(inFile, strLine); ++i);

	if (!inFile)
		std::cout << "all lines from the input file skipped, so nothing to read...\n";

	while (std::getline(inFile, strLine))
		outVec.push_back(ExtractLineInformation(strLine, params.idColumnWithWeights, params.delimiter));

	return outVec;
}

std::vector<int> BuildFromEntries(const std::vector<LineEntry>& vecEntries)
{
	std::vector<int> out;
	int currentId = 0;
	for (const auto& entry : vecEntries)
	{
		for (int i = 0; i < entry.weight; ++i)
			out.push_back(currentId);

		currentId++;
	}

	return out;
}

// draws winners from the input lines, includes weights, but also makes sure that one line cannot win more than once (even if it has multiple entries/weight)
std::vector<LineEntry> DrawWinners(const std::vector<LineEntry>& vecInputLines, const std::vector<int>& vecWeightedIndices, unsigned int numElementsToPick)
{
	unsigned int winnersFound = 0;
	std::set<int> setWinners;
	std::vector<LineEntry> outVec;
	for (unsigned int i = 0; winnersFound < numElementsToPick && i < vecWeightedIndices.size(); ++i)
	{
		const auto index = vecWeightedIndices[i];
		auto &entry = vecInputLines[index];

		if (setWinners.find(index) == setWinners.end())
		{
			setWinners.insert(index); // mark that 'index' is already a winner

			outVec.push_back(entry);

			winnersFound++;
		}
	}

	return outVec;
}

void ShowWinners(const std::vector<LineEntry>& vecWinners, const std::vector<LineEntry>& vecInputLines, const CmdInputParams& inputParams)
{
	if (!vecWinners.empty())
	{
		int lineCounter = 1;
		for (const auto& entryWinner : vecWinners)
		{
			std::cout << lineCounter << ": " << entryWinner.strLine << "\n";
			lineCounter++;
		}
	}

	if (inputParams.numElementsToPick > vecInputLines.size())
		std::cout << "Cannot pick next " << inputParams.numElementsToPick - vecInputLines.size() << " elements as there are not that many lines in the file...\n";
}