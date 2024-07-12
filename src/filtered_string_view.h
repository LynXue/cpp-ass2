#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

namespace fsv {
	using filter = std::function<bool(const char&)>;

	class filtered_string_view {
		class iter {
		 public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = std::ptrdiff_t;
			using pointer = void;
			using reference = const char&;

			iter();
			iter(const char* ptr, const filtered_string_view* view);

			auto operator*() const -> reference;

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) -> bool;
			friend auto operator!=(const iter&, const iter&) -> bool;

		 private:
			/* Implementation-specific private members */
			const char* ptr_;
			const filtered_string_view* view_;
			void advance();
			void retreat();
		};

	 public:
		using iterator = iter;
		using const_iterator = iter;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		// constructor
		filtered_string_view();
		filtered_string_view(const std::string& str, filter predicate = default_predicate);
		filtered_string_view(const char* str, filter predicate = default_predicate);
		filtered_string_view(const filtered_string_view& other);
		filtered_string_view(filtered_string_view&& other) noexcept;

		~filtered_string_view() = default;

		// assignment
		auto operator=(const filtered_string_view& other) -> filtered_string_view&;
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&;

		// subscript
		auto operator[](int index) const -> const char&;
		// string type conversion
		explicit operator std::string() const;

		// member function
		auto at(int index) const -> const char&;
		auto size() const -> std::size_t;
		auto empty() const -> bool;
		auto data() const -> const char*;
		auto predicate() const -> const filter&;
		auto length() const -> std::size_t;

		// iterator functions
		auto begin() const -> const_iterator;
		auto end() const -> const_iterator;
		auto cbegin() const -> const_iterator;
		auto cend() const -> const_iterator;
		auto rbegin() const -> std::reverse_iterator<const_iterator>;
		auto rend() const -> std::reverse_iterator<const_iterator>;
		auto crbegin() const -> std::reverse_iterator<const_iterator>;
		auto crend() const -> std::reverse_iterator<const_iterator>;

		// non-member operators
		friend auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
		friend auto operator!=(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool;
		friend auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering;
		friend auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&;

	 private:
		const char* data_;
		std::size_t length_;
		filter predicate_;

		// default predicate
		static const filter default_predicate;
	};

	// non-member utility functions
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view;
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view>;
	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view;
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
