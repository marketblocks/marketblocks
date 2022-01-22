#pragma once

template<typename T>
class basic_iterator
{
private:
	T* _data;

public:
	basic_iterator(T* data)
		: _data{ data }
	{}

	virtual ~basic_iterator() = default;

	virtual basic_iterator operator++()
	{
		++_data;
		return *this;
	}

	virtual bool operator!=(const basic_iterator& other)
	{
		return _data != other._data;
	}

	virtual const T& operator*()
	{
		return *_data;
	}
};