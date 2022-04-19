#pragma once

#include <vector>

namespace mb
{
	template<typename DataType, typename SequenceId>
	class partial_data_result
	{
	private:
		std::vector<DataType> _data;
		SequenceId _id;

	public:
		constexpr partial_data_result(std::vector<DataType> data, SequenceId id)
			: _data{ std::move(data) }, _id{ std::move(id) }
		{}

		constexpr const std::vector<DataType>& data() const noexcept { return _data; }
		constexpr const SequenceId& id() const noexcept { return _id; }
	};
}