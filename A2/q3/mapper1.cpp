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
		std::istringstream ss2(neighbours);
		std::string neighbour;
		while (std::getline(ss2, neighbour, ' '))
		{
			std::cout << neighbour << "\tD\t" << dist + 1 << std::endl;
		}
		std::cout << node << "\tD\t" << distance << std::endl;
		std::cout << node << "\tN\t" << neighbours << std::endl;
	}
	return 0;
}