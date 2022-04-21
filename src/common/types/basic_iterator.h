#pragma once

namespace mb
{
	template<typename Item, typename Iterator>
	class basic_iterator
	{
	private:
		Iterator _iterator;

	public:
		constexpr explicit basic_iterator(Iterator iterator)
			: _iterator{ std::move(iterator) }
		{}

		constexpr basic_iterator& operator++()
		{
			_iterator++;
			return *this;
		}

		constexpr bool operator!=(const basic_iterator& other) const noexcept
		{
			return _iterator != other._iterator;
		}

		constexpr const Item* operator->() const noexcept
		{
			return _iterator.operator->();
		}
	};
}