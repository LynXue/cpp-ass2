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
			using MEMBER_TYPEDEFS_GO_HERE = void;

			iter();

			auto operator*() const -> void; // change this
			auto operator->() const -> void; // change this

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) -> bool;
			friend auto operator!=(const iter&, const iter&) -> bool;

		 private:
			/* Implementation-specific private members */
		};

	 public:
		// constructor
		filtered_string_view();
		filtered_string_view(const std::string& str);
		filtered_string_view(const std::string& str, filter predicate);
		filtered_string_view(const char* str);
		filtered_string_view(const char* str, filter predicate);
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

		auto at(int index) const -> const char&;
		auto size() const -> std::size_t;
		auto empty() const -> bool;
		auto data() const -> const char*;
		auto predicate() const -> const filter&;
		auto length() const -> std::size_t;

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
