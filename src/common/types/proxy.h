#pragma once

template<typename T>
class value_proxy
{
private:
	T _value;

public:
	value_proxy(T value)
		: _value(std::move(value)) // ( ) initialization needs to be used to work with nlohmann::json due to
								   // bug where { } initialization cause wrong constructor to be called with
								   // certain compilers
	{}

	T& value() { return _value; }
	const T& value() const { return _value; }
};

template<typename T>
struct reference_proxy
{
private:
	T& _value;

public:
	reference_proxy(T& value)
		: _value(value) // see above
	{}

	T& value() { return _value; }
	const T& value() const { return _value; }
};