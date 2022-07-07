#include "http_request.h"

namespace mb
{
	std::ostream& operator<<(std::ostream& stream, const http_request& request)
	{
		stream << "Verb: " << to_string(request.verb()) << std::endl;
		stream << "Url: " << request.url() << std::endl;
		stream << "Content: " << request.content() << std::endl;

		return stream;
	}
}