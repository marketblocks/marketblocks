#include <iostream>

#include "test_strategy.h"

void TestStrategy::operator()()
{
	std::cout << "Run " << iterations++ << " iterations" << std::endl;
}
