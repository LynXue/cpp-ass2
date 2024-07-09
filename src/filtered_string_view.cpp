#include "./filtered_string_view.h"
#include <algorithm>
#include <cstring>
#include <iostream>

// Implement here
namespace fsv {
	const filter filtered_string_view::default_predicate = [](const char&) { return true; };

	filtered_string_view::filtered_string_view()
	: data_(nullptr)
	, length_(0)
	, predicate_(default_predicate) {}

	filtered_string_view::filtered_string_view(const std::string& str, filter predicate)
	: data_(str.data())
	, length_(str.size())
	, predicate_(std::move(predicate)) {}

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

	auto filtered_string_view::at(int index) const -> const char& {
		if (index < 0) {
			throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
		}
		auto count = 0;
		for (auto i = 0U; i < length_; ++i) {
			if (predicate_(data_[i])) {
				if (count == index) {
					return data_[i];
				}
				++count;
			}
		}
		throw std::domain_error{"filtered_string_view::at(" + std::to_string(index) + "): invalid index"};
	}

	auto filtered_string_view::size() const -> std::size_t {
		return static_cast<std::size_t>(std::count_if(data_, data_ + length_, predicate_));
	}

	auto filtered_string_view::empty() const -> bool {
		return size() == 0;
	}

	auto filtered_string_view::data() const -> const char* {
		return data_;
	}

	auto filtered_string_view::predicate() const -> const filter& {
		return predicate_;
	}

	// Non-member operators
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		auto lhs_it = lhs.data();
		auto rhs_it = rhs.data();
		auto lhs_end = lhs.data() + lhs.size();
		auto rhs_end = rhs.data() + rhs.size();

		while (lhs_it != lhs_end && rhs_it != rhs_end) {
			while (lhs_it != lhs_end && !lhs.predicate()(*lhs_it)) {
				++lhs_it;
			}
			while (rhs_it != rhs_end && !rhs.predicate()(*rhs_it)) {
				++rhs_it;
			}
			if (lhs_it != lhs_end && rhs_it != rhs_end) {
				if (*lhs_it != *rhs_it) {
					return false;
				}
				++lhs_it;
				++rhs_it;
			}
		}

		while (lhs_it != lhs_end && !lhs.predicate()(*lhs_it)) {
			++lhs_it;
		}
		while (rhs_it != rhs_end && !rhs.predicate()(*rhs_it)) {
			++rhs_it;
		}

		return lhs_it == lhs_end && rhs_it == rhs_end;
	}

	auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		return !(lhs == rhs);
	}

	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		auto lhs_it = lhs.data();
		auto rhs_it = rhs.data();
		auto lhs_end = lhs.data() + lhs.size();
		auto rhs_end = rhs.data() + rhs.size();

		while (lhs_it != lhs_end && rhs_it != rhs_end) {
			while (lhs_it != lhs_end && !lhs.predicate()(*lhs_it)) {
				++lhs_it;
			}
			while (rhs_it != rhs_end && !rhs.predicate()(*rhs_it)) {
				++rhs_it;
			}
			if (lhs_it != lhs_end && rhs_it != rhs_end) {
				if (auto cmp = *lhs_it <=> *rhs_it; cmp != std::strong_ordering::equal) {
					return cmp;
				}
				++lhs_it;
				++rhs_it;
			}
		}

		while (lhs_it != lhs_end && !lhs.predicate()(*lhs_it)) {
			++lhs_it;
		}
		while (rhs_it != rhs_end && !rhs.predicate()(*rhs_it)) {
			++rhs_it;
		}

		return (lhs_it == lhs_end) <=> (rhs_it == rhs_end);
	}

	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream& {
		for (auto i = 0U; i < fsv.length_; ++i) {
			if (fsv.predicate_(fsv.data_[i])) {
				os << fsv.data_[i];
			}
		}
		return os;
	}

	// compose function
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		filter composed_predicate = [filts](const char& c) {
			for (const auto& f : filts) {
				if (!f(c)) {
					return false;
				}
			}
			return true;
		};
		return filtered_string_view(fsv.data(), composed_predicate);
	}

	// split function
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view> {
		std::vector<filtered_string_view> result;

		if (fsv.empty()) {
			result.push_back(fsv);
			return result;
		}

		const char* start = fsv.data();
		const char* end = fsv.data() + fsv.size();
		const char* delim_start = tok.data();
		std::size_t delim_length = tok.size();
		const char* current = start;
		while (current < end) {
			const char* next_delim = std::search(current, end, delim_start, delim_start + delim_length);
			if (next_delim == end) {
				result.emplace_back(current, fsv.predicate());
				break;
			}

			result.emplace_back(current,
			                    [current, next_delim](const char& c) { return &c >= current && &c < next_delim; });
			current = next_delim + delim_length;
		}

		return result;
	}

	// substr function
	auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		if (pos < 0 || pos > static_cast<int>(fsv.size())) {
			throw std::out_of_range{"filtered_string_view::substr(" + std::to_string(pos) + ", " + std::to_string(count)
			                        + "): invalid position"};
		}

		std::size_t rcount = (count <= 0) ? fsv.size() - static_cast<std::size_t>(pos) : static_cast<std::size_t>(count);
		if (static_cast<std::size_t>(pos) + rcount > fsv.size()) {
			rcount = fsv.size() - static_cast<std::size_t>(pos);
		}

		const char* substr_data = fsv.data() + pos;
		filter substr_predicate = [substr_data, rcount](const char& c) {
			return &c >= substr_data && &c < substr_data + rcount;
		};

		return filtered_string_view(substr_data, substr_predicate);
	}

	void filtered_string_view::iter::advance() {
		do {
			++ptr_;
		} while (ptr_ != nullptr && view_ != nullptr && *ptr_ != '\0' && !view_->predicate()(*ptr_));
	}

	void filtered_string_view::iter::retreat() {
		do {
			--ptr_;
		} while (ptr_ != nullptr && view_ != nullptr && ptr_ >= view_->data() && !view_->predicate()(*ptr_));
	}

	// iterator class implementation
	filtered_string_view::iter::iter()
	: ptr_(nullptr)
	, view_(nullptr) {}

	filtered_string_view::iter::iter(const char* ptr, const filtered_string_view* view)
	: ptr_(ptr)
	, view_(view) {
		if (ptr_ != nullptr && view_ != nullptr && !view_->predicate()(*ptr_)) {
			advance();
		}
	}

	auto filtered_string_view::iter::operator*() const -> reference {
		return *ptr_;
	}

	auto filtered_string_view::iter::operator++() -> iter& {
		advance();
		return *this;
	}

	auto filtered_string_view::iter::operator++(int) -> iter {
		iter temp = *this;
		advance();
		return temp;
	}

	auto filtered_string_view::iter::operator--() -> iter& {
		retreat();
		return *this;
	}

	auto filtered_string_view::iter::operator--(int) -> iter {
		iter temp = *this;
		retreat();
		return temp;
	}

	auto operator==(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) -> bool {
		return lhs.ptr_ == rhs.ptr_;
	}

	auto operator!=(const filtered_string_view::iterator& lhs, const filtered_string_view::iterator& rhs) -> bool {
		return !(lhs == rhs);
	}

	// iterator functions
	auto filtered_string_view::begin() const -> const_iterator {
		return const_iterator(data_, this);
	}

	auto filtered_string_view::end() const -> const_iterator {
		return const_iterator(data_ + length_, this);
	}

	auto filtered_string_view::cbegin() const -> const_iterator {
		return begin();
	}

	auto filtered_string_view::cend() const -> const_iterator {
		return end();
	}

	auto filtered_string_view::rbegin() const -> std::reverse_iterator<const_iterator> {
		return std::reverse_iterator<const_iterator>(end());
	}

	auto filtered_string_view::rend() const -> std::reverse_iterator<const_iterator> {
		return std::reverse_iterator<const_iterator>(begin());
	}

	auto filtered_string_view::crbegin() const -> std::reverse_iterator<const_iterator> {
		return rbegin();
	}

	auto filtered_string_view::crend() const -> std::reverse_iterator<const_iterator> {
		return rend();
	}
} // namespace fsv
