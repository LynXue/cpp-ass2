#include "./filtered_string_view.h"
#include <cstring>

// Implement here
namespace fsv {
	const filter filtered_string_view::default_predicate = [](const char&) { return true; };

	filtered_string_view::filtered_string_view()
	: data_(nullptr)
	, length_(0)
	, predicate_(default_predicate) {}

	filtered_string_view::filtered_string_view(const std::string& str)
	: data_(str.data())
	, length_(str.size())
	, predicate_(default_predicate) {}

	filtered_string_view::filtered_string_view(const std::string& str, filter predicate)
	: data_(str.data())
	, length_(str.size())
	, predicate_(std::move(predicate)) {}

	filtered_string_view::filtered_string_view(const char* str)
	: data_(str)
	, length_(std::strlen(str))
	, predicate_(default_predicate) {}

	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: data_(str)
	, length_(std::strlen(str))
	, predicate_(std::move(predicate)) {}

} // namespace fsv
