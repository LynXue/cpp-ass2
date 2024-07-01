#include "./filtered_string_view.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>

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

	filtered_string_view::filtered_string_view(const filtered_string_view& other)
	: data_(other.data_)
	, length_(other.length_)
	, predicate_(other.predicate_) {}

	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: data_(other.data_)
	, length_(other.length_)
	, predicate_(std::move(other.predicate_)) {
		other.data_ = nullptr;
		other.length_ = 0;
		other.predicate_ = default_predicate;
	}

	// assignment operator
	auto filtered_string_view::operator=(const filtered_string_view& other) -> filtered_string_view& {
		if (this != &other) {
			data_ = other.data_;
			length_ = other.length_;
			predicate_ = other.predicate_;
		}
		return *this;
	}

	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			data_ = other.data_;
			length_ = other.length_;
			predicate_ = std::move(other.predicate_);
			other.data_ = nullptr;
			other.length_ = 0;
			other.predicate_ = default_predicate;
		}
		return *this;
	}

	// subscript
	auto filtered_string_view::operator[](int index) const -> const char& {
		auto count = 0;
		for (auto i = 0U; i < length_; ++i) {
			if (predicate_(data_[i])) {
				if (count == index) {
					return data_[i];
				}
				++count;
			}
		}
		throw std::out_of_range{"filtered_string_view::operator[](" + std::to_string(index) + "): invalid index"};
	}

	filtered_string_view::operator std::string() const {
		std::string result;
		for (auto i = 0U; i < length_; ++i) {
			if (predicate_(data_[i])) {
				result.push_back(data_[i]);
			}
		}
		return result;
	}
} // namespace fsv
