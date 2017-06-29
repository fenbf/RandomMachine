#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <iostream>
#include <set>

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

CmdInputParams ReadInputParams(int argc, char *argv[]);

LineEntry ExtractLineInformation(const std::string& strLine, int idWeightColumn, char delimiter);

std::vector<LineEntry> ReadAllLines(std::string strPath, const CmdInputParams& params);

std::vector<int> BuildFromEntries(const std::vector<LineEntry>& vecEntries);

template <typename ItRandom> void ShuffleWithMT19937(ItRandom itFirst, ItRandom itLast);

std::vector<LineEntry> DrawWinners(const std::vector<LineEntry>& vecInputLines, const std::vector<int>& vecWeightedIndices, const CmdInputParams& inputParams);

void ShowWinners(const std::vector<LineEntry>& vecWinners, const std::vector<LineEntry>& vecInputLines, const CmdInputParams& inputParams);

// main -----------------------------------------------------------------------

int main(int argc, char *argv[])
{
	try
	{
		const auto inputParams = ReadInputParams(argc, argv);

		//if (!ValidCmdParams(inputParams))
		//{
		//	ShowHelp();
		//	return 0; // not an error, just return early...
		//}

		const auto vecLineEntries = ReadAllLines(inputParams.strPath, inputParams);

		auto vecIdWithWeights = BuildFromEntries(vecLineEntries);

		ShuffleWithMT19937(std::begin(vecIdWithWeights), std::end(vecIdWithWeights));

		auto vecWinners = DrawWinners(vecLineEntries, vecIdWithWeights, inputParams);

		ShowWinners(vecWinners, vecLineEntries, inputParams);
	}
	catch (...)
	{
		// todo: expand error handling here :)
		std::cout << "Error...\n";
	}

	return 0;
}

// implementation -------------------------------------------------------------


// program.exe inputFile numberOfWinners LinesToSkip idColumnWithWeight ColumnSeparator
CmdInputParams ReadInputParams(int argc, char *argv[])
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
size_t find_Nth(const std::string & str, int occurence, const char delimiter)
{
	if (occurence <= 0)
		return std::string::npos;

	size_t pos, from = 0;
	for (int i = 0; i < occurence; ++i)
	{
		pos = str.find(delimiter, from);
		if (pos == std::string::npos)
			break;

		from = pos + 1;
	}

	return pos;
}

LineEntry ExtractLineInformation(const std::string& strLine, int idWeightColumn, char delimiter)
{
	LineEntry out{ strLine, 1 };

	if (idWeightColumn >= 0)
	{
		auto startPos = find_Nth(strLine, idWeightColumn, delimiter);
		if (startPos != std::string::npos)
		{
			const auto strRemainder = strLine.substr(startPos + 1);
			const auto nextPos = strRemainder.find(delimiter);
			const auto strWeight = strRemainder.substr(0, nextPos);
			out.weight = std::stoi(strWeight);
		}
	}

	return out;
}

std::vector<LineEntry> ReadAllLines(std::string strPath, const CmdInputParams& params)
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

template <typename ItRandom> void ShuffleWithMT19937(ItRandom itFirst, ItRandom itLast)
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(itFirst, itLast, g);
}

// draws winners from the input lines, includes weights, but also makes sure that one line cannot win more than once (even if it has multiple entries/weight)
std::vector<LineEntry> DrawWinners(const std::vector<LineEntry>& vecInputLines, const std::vector<int>& vecWeightedIndices, const CmdInputParams& inputParams)
{
	unsigned int winnersFound = 0;
	std::set<int> setWinners;
	std::vector<LineEntry> outVec;
	for (unsigned int i = 0; winnersFound < inputParams.numElementsToPick && i < vecInputLines.size(); ++i)
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