#pragma once

#include "common/json/json.h"

namespace mb
{
	class back_testing_config
	{
	public:
		static constexpr std::string name() noexcept { return "back_testing"; }
	};

	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json);

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer);
}
