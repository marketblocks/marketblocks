#pragma once

#include "logging/logger.h"
#include "common/exceptions/cb_exception.h"
#include "common/types/result.h"

namespace mb
{
	template<typename Value, typename Action, typename ResultConverter>
	Value retry_on_fail(const Action& action, const ResultConverter& toResult, int maxRetries)
	{
		logger& log{ logger::instance() };

		for (int i = 0; i < maxRetries + 1; ++i)
		{
			auto value{ action() };
			result<Value> result{ toResult(value) };

			if (result.is_success())
			{
				return result.value();
			}

			log.error(result.error());
			log.warning("Retrying attempt {0}/{1}...", i + 1, maxRetries);
		}

		throw cb_exception{ "Max number of retries exceeded" };
	}
}