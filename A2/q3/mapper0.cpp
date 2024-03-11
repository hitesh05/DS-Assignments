#include <iostream>
#include <sstream>
#include <string>

int32_t main()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::istringstream ss(line);
		std::string node, neighbours;
		std::getline(ss, node, '\t');
		std::getline(ss, neighbours, '\t');
		std::cout << node << "\t" << (node == "1" ? "0" : "10000001") << "\t" << neighbours << std::endl;
	}
	return 0;
}