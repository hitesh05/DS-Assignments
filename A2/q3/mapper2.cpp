#include <iostream>
#include <sstream>
#include <string>

int32_t main()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::istringstream ss(line);
		std::string node, neighbours, distance;
		std::getline(ss, node, '\t');
		std::getline(ss, distance, '\t');
		std::getline(ss, neighbours, '\t');
		int dist = std::stoi(distance);
		if (dist <= 10)
			std::cout << node << "\t" << dist << std::endl;
	}
	return 0;
}