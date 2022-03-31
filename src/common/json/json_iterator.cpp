#include "json_iterator.h"
#include "json.h"

namespace mb
{
	json_iterator::json_iterator(iterator iter)
		: _iter{ iter }
	{}

	json_iterator json_iterator::operator++()
	{
		++_iter;
		return *this;
	}

	bool json_iterator::operator!=(const json_iterator& other) const
	{
		return _iter != other._iter;
	}

	const std::string& json_iterator::key() const
	{
		try
		{
			return _iter.key();
		}
		catch (nlohmann::detail::invalid_iterator)
		{
			return "";
		}
	}

	const json_element json_iterator::value() const
	{
		return json_element{ *_iter };
	}
}