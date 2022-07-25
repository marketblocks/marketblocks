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
		asset_report(
			std::string asset,
			std::string start,
			std::string end,
			std::string change,
			std::string percentageChange,
			std::string annualReturn);

		const std::string& asset() const noexcept { return _asset; }
		const std::string& start_balance() const noexcept { return _start; }
		const std::string& end_balance() const noexcept { return _end; }
		const std::string& change() const noexcept { return _change; }
		const std::string& percentage_change() const noexcept { return _percentageChange; }
		const std::string& annual_return() const noexcept { return _annualReturn; }
	};
}