#pragma once

class TestStrategy
{
private:
	int iterations;

public:
	TestStrategy()
		: iterations{0}
	{}

	void operator()();
};