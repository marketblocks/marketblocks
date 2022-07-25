#include "asset_report.h"

namespace mb
{
	asset_report::asset_report(
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
}