#pragma once

#include <string>

namespace mb
{
	class asset_report
	{
	private:
		std::string _asset;
		std::string _start;
		std::string _end;
		std::string _change;
		std::string _percentageChange;
		std::string _annualReturn;

	public:
		constexpr asset_report(
			std::string asset,
			std::string start,
			std::string end,
			std::string change,
			std::string percentageChange,
			std::string annualReturn)
			:
			_asset{ std::move(asset) },
			_start{ std::move(start) },
			_end{ std::move(end) },
			_change{ std::move(change) },
			_percentageChange{ std::move(percentageChange) },
			_annualReturn{ std::move(annualReturn) }
		{}

		constexpr const std::string& asset() const noexcept { return _asset; }
		constexpr const std::string& start_balance() const noexcept { return _start; }
		constexpr const std::string& end_balance() const noexcept { return _end; }
		constexpr const std::string& change() const noexcept { return _change; }
		constexpr const std::string& percentage_change() const noexcept { return _percentageChange; }
		constexpr const std::string& annual_return() const noexcept { return _annualReturn; }
	};
}