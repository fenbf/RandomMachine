#include "RandomMachineApp.h"

#include <iostream>

// main -----------------------------------------------------------------------

int main(int argc, const char *argv[])
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

		auto vecWinners = DrawWinners(vecLineEntries, vecIdWithWeights, inputParams.numElementsToPick);

		ShowWinners(vecWinners, vecLineEntries, inputParams);
	}
	catch (...)
	{
		// todo: expand error handling here :)
		std::cout << "Error...\n";
	}

	return 0;
}