#include "back_testing_config.h"

namespace mb
{
	template<>
	back_testing_config from_json<back_testing_config>(const json_document& json)
	{
		return back_testing_config{};
	}

	template<>
	void to_json<back_testing_config>(const back_testing_config& config, json_writer& writer)
	{

	}
}