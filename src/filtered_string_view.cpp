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
		auto result = std::string();
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

	auto filtered_string_view::length() const -> std::size_t {
		return length_;
	}

	auto filtered_string_view::predicate() const -> const filter& {
		return predicate_;
	}

	// Non-member operators
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		if (lhs.size() != rhs.size()) {
			return false;
		}
		for (int i = 0; i < static_cast<int>(lhs.size()); ++i) {
			if (lhs[i] != rhs[i]) {
				return false;
			}
		}
		return true;
	}

	auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		return !(lhs == rhs);
	}

	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		int lhs_length = static_cast<int>(lhs.size());
		int rhs_length = static_cast<int>(rhs.size());
		for (int i = 0; i < std::min(lhs_length, rhs_length); ++i) {
			if (auto cmp = lhs[i] <=> rhs[i]; cmp != std::strong_ordering::equal) {
				return cmp;
			}
		}
		return lhs_length <=> rhs_length;
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
		auto composed_predicate = [filts](const char& c) {
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
		auto result = std::vector<filtered_string_view>();

		if (fsv.empty()) {
			result.push_back(fsv);
			return result;
		}

		auto start = fsv.data();
		auto end = fsv.data() + fsv.length();
		auto delim_start = tok.data();
		auto delim_length = tok.length();
		auto current = start;

		while (current < end) {
			auto next_delim = std::search(current, end, delim_start, delim_start + delim_length);
			if (next_delim == end) {
				result.emplace_back(current, fsv.predicate());
				break;
			}

			result.emplace_back(current, [current, next_delim, fsv](const char& c) {
				return &c >= current && &c < next_delim && fsv.predicate()(c);
			});
			current = next_delim + delim_length;
		}
		if (current == end) {
			result.push_back("");
		}

		return result;
	}

	// substr function
	auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		if (pos < 0 || static_cast<std::size_t>(pos) > fsv.size()) {
			throw std::out_of_range{"filtered_string_view::substr(" + std::to_string(pos) + ", " + std::to_string(count)
			                        + "): invalid position"};
		}

		auto start_pos = static_cast<std::size_t>(pos);
		auto rcount = (count <= 0) ? fsv.size() - start_pos : static_cast<std::size_t>(count);
		if (start_pos + rcount > fsv.size()) {
			rcount = fsv.size() - start_pos;
		}

		filter new_predicate = [fsv, start_pos, rcount](const char& c) {
			for (std::size_t i = 0; i < rcount; ++i) {
				if (fsv[static_cast<int>(start_pos + i)] == c) {
					return fsv.predicate()(c);
				}
			}
			return false;
		};

		return filtered_string_view(fsv.data() + start_pos, new_predicate);
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
