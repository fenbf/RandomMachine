#include "CppUnitTest.h"

#include "..\RandomWithWeights\RandomMachineApp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(RandomMachine)
	{
	public:
		
		TEST_METHOD(InputParamsDefaultTest)
		{
			const char* argv[] = { "exe", "abc.csv" };
			const auto inputParams = ReadInputParams(2, argv);
			Assert::AreEqual("abc.csv", inputParams.strPath.c_str());
			Assert::AreEqual(0U, inputParams.numElementsToPick);
			Assert::AreEqual(0U, inputParams.numTopLinesToSkip);
		}

		TEST_METHOD(InputParamsTest)
		{
			const char* argv[] = { "exe", "xyz.csv", "3", "1", "4", "|" };
			const auto inputParams = ReadInputParams(6, argv);
			Assert::AreEqual("xyz.csv", inputParams.strPath.c_str());
			Assert::AreEqual(3U, inputParams.numElementsToPick);
			Assert::AreEqual(1U, inputParams.numTopLinesToSkip);
			Assert::AreEqual(4, inputParams.idColumnWithWeights);
			Assert::AreEqual('|', inputParams.delimiter);
		}

		TEST_METHOD(ExtractLineInfoBasicTest)
		{
			{
				const auto lineEntry = ExtractLineInformation("hello;world;abc;50", 3, ';');
				Assert::AreEqual(50, lineEntry.weight);
			}

			{
				const auto lineEntry = ExtractLineInformation("hello;world;abc;50", 30, ';');
				Assert::AreEqual(1, lineEntry.weight);
			}

			auto funPtr = [this] { ExtractLineInformation("hello,world,abc,XXX", 3, ',');  };
			Assert::ExpectException<std::invalid_argument>(funPtr);
		}

		TEST_METHOD(DrawWinnersBasicTest)
		{
			const std::vector<LineEntry> entries = { { "john", 1}, { "adam", 5 }, { "nick", 1 } };
			const auto vecWeightedIds = BuildFromEntries(entries);

			const auto winners = DrawWinners(entries, vecWeightedIds, 3);
			
			Assert::AreEqual(3U, winners.size());
			Assert::AreEqual("john", winners[0].strLine.c_str());
			Assert::AreEqual("adam", winners[1].strLine.c_str());
			Assert::AreEqual("nick", winners[2].strLine.c_str());
		}
	};
}