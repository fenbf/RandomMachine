#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <algorithm>
#include <iostream>

struct InputParams
{
	std::string strPath;
	unsigned int numElementsToPick{ 0 };
	unsigned int numTopLinesToSkip{ 0 };
};

InputParams ReadInputParams(int argc, char *argv[])
{
	InputParams out{ "input.csv", 1 };
	
	if (argc > 1)
		out.strPath = argv[1];

	if (argc > 2)
		out.numElementsToPick = ::atoi(argv[2]);

	if (argc > 3)
		out.numTopLinesToSkip = ::atoi(argv[3]);

	return out;
}

std::vector<std::string> ReadAllLines(std::string strPath, unsigned int numTopLinesToSkip)
{
	std::vector<std::string> outVec;
	std::string strLine;
	std::ifstream inFile(strPath);

	if (!inFile)
		throw std::exception("cannot open file...");

	for (unsigned int i = 0; i < numTopLinesToSkip && std::getline(inFile, strLine); ++i);

	if (!inFile)
		std::cout << "all lines from the input file skipped, so nothing to read...\n";

	while (std::getline(inFile, strLine))
		outVec.push_back(strLine);

	return outVec;
}

int main(int argc, char *argv[])
{
	try
	{
		const auto inputParams = ReadInputParams(argc, argv);

		auto vecLines = ReadAllLines(inputParams.strPath, inputParams.numTopLinesToSkip);

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(std::begin(vecLines), std::end(vecLines), g);

		for (unsigned int i = 0; i < inputParams.numElementsToPick && i < vecLines.size(); ++i)
			std::cout << i << ": " << vecLines[i] << "\n";

		if (inputParams.numElementsToPick > vecLines.size())
			std::cout << "Cannot pick next " << inputParams.numElementsToPick - vecLines.size() << " elements as there are not that many lines in the file...\n";
	}
	catch (...)
	{
		std::cout << "Error...\n";
	}

	return 0;
}