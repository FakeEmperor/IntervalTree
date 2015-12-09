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
				Option(	"s","sort", "0",
						"Indicates whether it is necessary to perform sorting on reported intervals. 0 - not needed, 1 - ascending sort, 2 - descending sort",
						true, false),
				Option( "o","output_file", nullptr, 
						"Output file that holds a list of all intervals with that point. If this option is not specified,"\
						"standard output stream is selected for output",
						false, false	)
			},"Returns all intervals which intersect this point.") 
	};
	CommandLine cmdline = CommandLine(
		commands,0,
		"This program is a solution for problem 17: \'Geolocation in one-dimensional space\'."\
		"\r\nThis program actually supports solutions for geo location problem in multi-dimensional space,"\
		"but needs to be recompiled with different dimensional parameter."\
		"\r\nMore info and source code at: https://github.com/FakeEmperor/IntervalTree" );
	//SelfTestResult st;
	try {

		cmdline.SelfTest();
	}
	catch (std::exception e) {
		std::cerr << "[!] Self test failed: " << std::endl << e.what();
	}

	try {
		cmdline.Parse(argc, argv);
		if (cmdline.ParseResult() != ParsingResult::CLPR_OK) {
			std::cerr << "[!] Failed to parse arguments: " << GetStringParsingResult(cmdline.ParseResult())
				<< std::endl
				<< std::endl
				<< cmdline.Help();
		}
		else {
			std::cout << "[+] Parsing finished! " << std::endl
				<< "[*] Executing command..." << std::endl;
			const Command& acmd = cmdline.ParsedCommand();
			switch (acmd.Id()) {
			case 2:
			{
				//MAIN CODE IS HERE
				const char *oname = acmd["output_file"].StoredValue(), *iname = acmd["input_file"].StoredValue();
				int sort = std::stoi(!acmd["sort"].StoredValue()?acmd["sort"].DefaultValue():acmd["sort"].StoredValue());
				IntervalDimensionalTreeNode<int, 1>::Point p = IntervalDimensionalTreeNode<int, 1>::ParsePoint(acmd["point"].StoredValue());
				if (sort > 2 || sort < 0)
					throw std::exception("Sort parameter invalid for command \'locate\'. Check help for more info.");
				if (oname)
					ofs.open(oname);
				if (!iname || !strlen(iname))
					throw std::exception("Input file path is invalid");
				else
					ifs.open(iname);

				auto itree = IntervalDimensionalTreeNode<int, 1>::Parse(ifs);
				std::shared_ptr<IntervalDimensionalTreeNode<int, 1>> itreep(&itree, [](IntervalDimensionalTreeNode<int, 1> *p){ /* do nothing in destructor*/ }); //warp into this
				IntervalDimensionalTreeNode<int, 1>::Ivals intervals = itreep->Find(p, sort);
				for (auto it = intervals.begin(), s = intervals.end(); it != s; ++it)
					(oname ? ofs : std::cout) << "Interval: " << "[ (" << it->first[0] << ") , (" << it->second[0] << ") ]" << std::endl;

				
			}
				break;
			default:
				throw std::exception("Sorry, command is not supported at the moment.");
				break;
			}
		}
	}
	catch (std::exception e) {
		std::cerr	<< "[!] Exception occured during program execution: " << std::endl 
					<< "\t" <<  e.what() <<std::endl;
	}
	
	std::cout << "[OK] Finished! Have a good day, sir!" << std::endl;
	


	

	ifs.close();
	ofs.close();
	
	return 0;
}

