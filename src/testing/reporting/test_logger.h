#pragma once

#include <vector>

#include "test_report.h"
#include "testing/paper_trading/paper_trade_api.h"
#include "common/file/file.h"

namespace mb
{
	class test_logger_exchange_data
	{
	private:
		std::shared_ptr<paper_trade_api> _tradeApi;
		unordered_string_map<double> _initialBalances;
		int _closedOrderIndex;

	public:
		test_logger_exchange_data(std::shared_ptr<paper_trade_api> tradeApi);

		std::shared_ptr<paper_trade_api> trade_api() const noexcept { return _tradeApi; }
		const unordered_string_map<double>& initial_balances() const noexcept { return _initialBalances; }
		int closed_order_index() const noexcept { return _closedOrderIndex; }
		void set_closed_order_index(int index) noexcept { _closedOrderIndex = index; }
		std::string_view exchange_id() const noexcept { return _tradeApi->exchange_id(); }
	};

	class test_logger
	{
	private:
		std::vector<test_logger_exchange_data> _exchangeData;
		std::filesystem::path _outputDirectory;
		file_handler _tradeFileHandler;
		std::time_t _startTime;

	public:
		test_logger(
			std::vector<test_logger_exchange_data> exchangeData,
			std::filesystem::path outputDirectory,
			file_handler tradeFileHandler);

		void flush_trades();

		test_report generate_test_report(std::time_t dataTimeRange = 0, report_result_list additionalResults = {}) const;
		void log_test_report(const test_report& report) const;
	};

	test_logger create_test_logger(std::vector<std::shared_ptr<paper_trade_api>> tradeApis);
}