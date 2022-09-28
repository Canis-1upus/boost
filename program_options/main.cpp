#include <iostream>
#include <boost/program_options.hpp>

using namespace boost;
using namespace boost::program_options;
using namespace std;

int main(int argc, char* argv[])
{
	boost::program_options::options_description desc("test program options:");

	desc.add_options()
		("help,h", "HELP SCREEN")
		("version,v", "test version: 0.0.1")
		("number,n", boost::program_options::value<double>(), "EXAMPLE number")
		;

	// store 예외 처리가 부족하다 ---------- / not digit number check
	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);

	if (vm.count("help"))
	{
		cout << "Usage: regex [options]\n";
		cout << desc;
	}
	if (vm.count("version"))
	{
		cout << "Version 1.\n";
	}
	if (vm.count("number"))
	{
		cout << "--number option was set .(" << vm["number"].as<double>() << ")" << endl;
	}

	return 0;
}
