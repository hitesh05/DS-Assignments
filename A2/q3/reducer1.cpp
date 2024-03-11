#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>

int32_t main()
{
	std::map<std::string, std::pair<int, std::vector<int>>> mp;
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::istringstream ss(line);
		std::string node, neighbors, type;
		std::getline(ss, node, '\t');
		std::getline(ss, type, '\t');
		std::getline(ss, neighbors, '\t');
		if (type == "D")
		{
			if (mp.find(node) == mp.end())
				mp[node] = {std::stoi(neighbors), std::vector<int>()};
			else
				mp[node].first = std::min(mp[node].first, std::stoi(neighbors));
		}
		else
		{
			if (mp.find(node) == mp.end())
				mp[node] = {10000001, std::vector<int>()};
			std::vector<int> v;
			std::istringstream ss2(neighbors);
			std::string neighbor;
			while (std::getline(ss2, neighbor, ' '))
			{
				v.push_back(std::stoi(neighbor));
			}
			mp[node].second = v;
		}
	}

	for (const auto &node : mp)
	{
		std::cout << node.first << "\t" << node.second.first << "\t";
		for (const auto &neighbor : node.second.second)
		{
			std::cout << neighbor << " ";
		}
		std::cout << std::endl;
	}
	return 0;
}
