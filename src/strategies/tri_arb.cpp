#include "tri_arb.h"
#include <iostream>

TriArbStrategy TriArbStrategy::create()
{
	std::vector<TriArbExchangeSpec> specs;
	TriArbStrategy s{ std::move(specs) };

	return s;
}

void TriArbStrategy::operator()()
{
	std::cout << "Running iteration of Tri Arb Strategy" << std::endl;
}
