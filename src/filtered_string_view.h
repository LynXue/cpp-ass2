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

	 private:
		const char* data_;
		std::size_t length_;
		filter predicate_;

		// default predicate
		static const filter default_predicate;
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
