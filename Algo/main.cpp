#include "IntervalTree.h"
#include "CommandLine.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>

#define VNAME(x) #x

const IntervalDimensionalTreeNode<int, 1>::MedianFunction IntervalDimensionalTreeNode<int, 1>::Median = [](const IntervalDimensionalTreeNode<int, 1>::Ival &a) {
	std::array<int, 1> arr = { (a.first[0] + a.second[0]) / 2 };
	return arr;
};

const IntervalDimensionalTreeNode<int, 1>::WeightFunction IntervalDimensionalTreeNode<int, 1>::Weight = [](const IntervalDimensionalTreeNode<int, 1>::Ival &a) {
	size_t val = a.second[0] - a.first[0] + 1;
	return val;
};

int main(int argc, char** argv) {
	std::ofstream ofs;
	std::ifstream ifs;

	std::vector<Command> commands = { 
		Command("build",1,
			{
				Option(	"i","input_file", nullptr, 
						"Input file that holds intervals and interval names. More info in documentation.",
						true, false		),
				Option( "o","output_file", "out.txt", 
						"Output file that holds built tree visualization. If this option is not specified,"\
						"standard output stream is selected for output",
						false, false	),
			}, 
			"Builds and prints Interval Tree. Useful for debugging."),
		Command("locate",2,
			{
				Option( "i","input_file", nullptr,
						"Input file that holds intervals and interval names. More info in documentation.",
						true, false		),
				Option( "p","point", nullptr,
						"Value of the point-of-interest in one-dimensional space", 
						true, false		),
				Option( "o","output_file", nullptr, 
						"Output file that holds a list of all intervals with that point. If this option is not specified,"\
						"standard output stream is selected for output",
						false, false	)
			},"Returns all intervals which intersect this point.") 
	};
	CommandLine cmdline = CommandLine(commands,0,"This program is a solution for problem $num: \'Geolocation in one-dimensional space\'");
	//SelfTestResult st;
	try {
		cmdline.SelfTest();
	}
	catch (std::exception e) {
		std::cerr << "Self test failed: " << std::endl << e.what();
	}

	cmdline.Parse(argc, argv);
	if (cmdline.ParseResult() != ParsingResult::CLPR_OK) {
		std::cerr << "[!] Failed to parse arguments: " << GetStringParsingResult(cmdline.ParseResult()) << std::endl << std::endl;
		std::cerr << cmdline.Help();
	}
	else {
		std::cout << "[+]\tParsing finished! " << std::endl << "[*]\tValidating parameters..." << std::endl;
	}
	//TEST CODE
	const char* name = cmdline["locate"]["output_file"].DefaultValue();
	if(name)
		ofs.open(name);

	std::shared_ptr<IntervalDimensionalTreeNode<int,1>> itree(new IntervalDimensionalTreeNode<int, 1>({ { {0},{3} }, { {1}, {3} }, { {5}, {9} }, { {3}, {4} } }));

	IntervalDimensionalTreeNode<int,1>::Ivals intervals = itree->Find({ 7 },false);

	for (auto it = intervals.begin(), s = intervals.end(); it != s; ++it) {
		(name ? ofs : std::cout ) << "Interval: " << "[" << it->first[0] << "," << it->second[0] << "]" << std::endl;
	}

	ofs.close();
	IntervalDimensionalTreeNode<int, 1>::Parse(std::cin);
	return 0;
}

