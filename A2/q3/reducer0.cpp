#include <iostream>
#include <string>

int32_t main()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		std::cout << line << std::endl;
	}
	return 0;
}