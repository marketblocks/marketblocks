#include <nlohmann/json.hpp>

#include "json_constants.h"
#include "json_iterator.h"

namespace mb
{
	class json_writer
	{
	private:
		template <typename T>
		friend json_document to_json(const T& t);

		nlohmann::json _document;

	public:
		template<typename T>
		json_writer& add(std::string_view propertyName, T value)
		{
			_document[propertyName.data()] = std::move(value);

			return *this;
		}

		template<>
		json_writer& add(std::string_view propertyName, json_writer value)
		{
			_document[propertyName.data()] = value._document;

			return *this;
		}

		std::string to_string() const
		{
			return _document.dump();
		}
	};
}