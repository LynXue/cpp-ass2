#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <cstring>
#include <set>
#include <vector>

TEST_CASE("Default constructor") {
	fsv::filtered_string_view sv;
	REQUIRE(sv.size() == 0);
	REQUIRE(sv.empty());
	REQUIRE(sv.data() == nullptr);
}

TEST_CASE("Constructor with std::string") {
	std::string str = "hello";
	fsv::filtered_string_view sv(str);
	REQUIRE(sv.size() == str.size());
	REQUIRE(!sv.empty());
	REQUIRE(sv.data() == str.data());
}

TEST_CASE("Constructor with std::string and predicate") {
	std::string str = "hello";
	auto predicate = [](const char& c) { return c != 'l'; };
	fsv::filtered_string_view sv(str, predicate);
	REQUIRE(sv.size() == 3);
	REQUIRE(!sv.empty());
	REQUIRE(sv[0] == 'h');
	REQUIRE(sv[1] == 'e');
	REQUIRE(sv[2] == 'o');
}

TEST_CASE("Constructor with C-string") {
	const char* str = "hello";
	fsv::filtered_string_view sv(str);
	REQUIRE(sv.size() == std::strlen(str));
	REQUIRE(!sv.empty());
	REQUIRE(sv.data() == str);
}

TEST_CASE("Constructor with C-string and predicate") {
	const char* str = "hello world";
	auto predicate = [](const char& c) { return c != ' '; };
	fsv::filtered_string_view sv(str, predicate);
	REQUIRE(sv.size() == 10);
	REQUIRE(!sv.empty());
	REQUIRE(sv[0] == 'h');
	REQUIRE(sv[1] == 'e');
	REQUIRE(sv[2] == 'l');
	REQUIRE(sv[3] == 'l');
	REQUIRE(sv[4] == 'o');
}

TEST_CASE("Copy constructor") {
	std::string str = "hello world";
	fsv::filtered_string_view sv1(str);
	fsv::filtered_string_view sv2 = sv1;
	REQUIRE(sv1.data() == sv2.data());
	REQUIRE(sv1.size() == sv2.size());
}

TEST_CASE("Move constructor") {
	std::string str = "move";
	fsv::filtered_string_view sv1(str);
	fsv::filtered_string_view sv2(std::move(sv1));
	REQUIRE(sv2.size() == str.size());
	REQUIRE(sv2.data() == str.data());
	REQUIRE(sv1.size() == 0);
	REQUIRE(sv1.data() == nullptr);
}

TEST_CASE("Assignment operator") {
	std::string str1 = "assign1";
	std::string str2 = "hello";
	auto pred = [](const char& c) { return c == 'a' || c == 's'; };
	fsv::filtered_string_view sv1(str1, pred);
	fsv::filtered_string_view sv2(str2);
	sv2 = sv1;
	REQUIRE(sv1.size() == 3);
	REQUIRE(sv1.size() == sv2.size());
	REQUIRE(sv1.data() == sv2.data());
}

TEST_CASE("Move assignment operator") {
	std::string str = "transfer";
	auto pred = [](const char& c) { return c == 't' || c == 'r'; };
	fsv::filtered_string_view sv1(str, pred);
	fsv::filtered_string_view sv2;
	REQUIRE(sv1.size() == 3);
	sv2 = std::move(sv1);
	REQUIRE(sv2.size() == 3);
	REQUIRE(sv1.size() == 0);
	REQUIRE(sv1.data() == nullptr);
}

TEST_CASE("String conversion") {
	std::string str = "convertme";
	fsv::filtered_string_view sv(str);
	std::string result = static_cast<std::string>(sv);
	REQUIRE(result == str);
}

TEST_CASE("Subscript operator") {
	std::string str = "indexing";
	auto pred = [](const char& c) { return c == 'i' || c == 'n'; };
	fsv::filtered_string_view sv(str, pred);
	REQUIRE(sv[0] == 'i');
	REQUIRE(sv[1] == 'n');
	REQUIRE(sv[2] == 'i');
	REQUIRE(sv[3] == 'n');
}

TEST_CASE("At function") {
	std::string str = "position";
	auto pred = [](const char& c) { return c == 'p' || c == 'o'; };
	fsv::filtered_string_view sv(str, pred);
	REQUIRE(sv.at(0) == 'p');
	REQUIRE(sv.at(1) == 'o');
	REQUIRE(sv.at(2) == 'o');
	REQUIRE_THROWS_AS(sv.at(3), std::domain_error);
}

TEST_CASE("Size function") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	fsv::filtered_string_view sv{"Malamute", is_vowel};
	fsv::filtered_string_view sv2{"hello"};
	REQUIRE(sv.size() == 4);
	REQUIRE(sv2.size() == 5);
}

TEST_CASE("Equality and relational comparison") {
	auto const vowels = std::set<char>{'a', 'e', 'i', 'o', 'u'};
	auto const is_vowel = [&vowels](const char& c) { return vowels.contains(c); };

	auto const sv1 = fsv::filtered_string_view{"education", is_vowel};
	auto const sv2 = fsv::filtered_string_view{"ffeuaio", is_vowel};
	auto const sv3 = fsv::filtered_string_view{"abcdefg", is_vowel};

	REQUIRE(sv1 == sv2);
	REQUIRE(sv1 != sv3);
	REQUIRE(sv2 != sv3);

	REQUIRE(sv1 > sv3);
	REQUIRE(sv1 >= sv3);
	REQUIRE(!(sv1 < sv3));
	REQUIRE(!(sv1 <= sv3));
	REQUIRE((sv1 <=> sv3) == std::strong_ordering::greater);
}

TEST_CASE("filtered_string_view output stream") {
	auto fsv = fsv::filtered_string_view{"c++ > golang > rust", [](const char& c) { return c == 'c' || c == '+'; }};
	std::ostringstream oss;
	oss << fsv;
	REQUIRE(oss.str() == "c++");
}

TEST_CASE("compose function") {
	auto best_languages = fsv::filtered_string_view{"c / c++"};
	auto vf = std::vector<fsv::filter>{
	    [](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	    [](const char& c) { return c > ' '; },
	};

	auto sv = compose(best_languages, vf);
	std::ostringstream oss;
	oss << sv;
	REQUIRE(oss.str() == "c/c++");
}

TEST_CASE("split function") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v = split(sv, tok);

	REQUIRE(v.size() == 2);
	std::ostringstream oss1, oss2;
	oss1 << v[0];
	oss2 << v[1];
	REQUIRE(oss1.str() == "DEADBEEF");
	REQUIRE(oss2.str() == "deadbeef");

	auto sv2 = fsv::filtered_string_view{"xax"};
	auto tok2 = fsv::filtered_string_view{"x"};
	auto v2 = split(sv2, tok2);
	REQUIRE(v2.size() == 3);
	REQUIRE(v2[0].empty());
	std::ostringstream oss3;
	oss3 << v2[1];
	REQUIRE(oss3.str() == "a");
	REQUIRE(v2[2].empty());

	auto sv3 = fsv::filtered_string_view{"xx"};
	auto tok3 = fsv::filtered_string_view{"x"};
	auto v3 = split(sv3, tok3);
	REQUIRE(v3.size() == 3);
	REQUIRE(v3[0].empty());
	REQUIRE(v3[1].empty());
	REQUIRE(v3[2].empty());
}

TEST_CASE("substr function") {
	auto pred = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };

	SECTION("Basic substring extraction") {
		auto sv = fsv::filtered_string_view{"Siberian Husky"};
		auto sub_sv = fsv::substr(sv, 9);
		std::ostringstream oss;
		oss << sub_sv;
		REQUIRE(oss.str() == "Husky");
	}

	SECTION("Substring with predicate") {
		auto sv = fsv::filtered_string_view{"Sled Dog", pred};
		auto sub_sv = fsv::substr(sv, 0, 2);
		std::ostringstream oss;
		oss << sub_sv;
		REQUIRE(oss.str() == "SD");
	}

	SECTION("count <= 0") {
		auto sv = fsv::filtered_string_view{"HELLo", pred};
		auto sv2 = fsv::filtered_string_view{"HELLO World", pred};
		auto sub_sv1 = fsv::substr(sv, 3);
		auto sub_sv2 = fsv::substr(sv2, 3, -5);
		std::ostringstream oss1;
		std::ostringstream oss2;
		oss1 << sub_sv1;
		oss2 << sub_sv2;
		REQUIRE(oss1.str() == "L");
		REQUIRE(oss2.str() == "LOW");
	}

	SECTION("Count exceeding the length of the string") {
		auto sv = fsv::filtered_string_view{"Boundary Test"};
		auto sub_sv = fsv::substr(sv, 8, 50);
		std::ostringstream oss;
		oss << sub_sv;
		REQUIRE(oss.str() == " Test");
	}
}

TEST_CASE("Iterators") {
	std::string str = "iterator";
	fsv::filtered_string_view sv(str);
	std::string result;
	for (auto it = sv.begin(); it != sv.end(); ++it) {
		result.push_back(*it);
	}
	REQUIRE(result == str);
}

TEST_CASE("Default predicate iterator test") {
	fsv::filtered_string_view fsv1{"corgi"};
	std::vector<char> result{fsv1.begin(), fsv1.end()};
	REQUIRE(result == std::vector<char>{'c', 'o', 'r', 'g', 'i'});
}

TEST_CASE("Custom predicate iterator test") {
	fsv::filtered_string_view fsv2{"samoyed", [](const char& c) {
		                               return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                               }};
	auto it = fsv2.begin();
	REQUIRE(*it == 's');
	REQUIRE(*std::next(it) == 'm');
	REQUIRE(*std::next(it, 2) == 'y');
	REQUIRE(*std::next(it, 3) == 'd');
}

TEST_CASE("Reverse iterator test") {
	fsv::filtered_string_view fsv3{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	std::vector<char> result{fsv3.rbegin(), fsv3.rend()};
	REQUIRE(result == std::vector<char>{'l', 'm'});
}

TEST_CASE("Const iterator test") {
	const auto str = std::string("tosa");
	const fsv::filtered_string_view fsv4{str};
	auto it = fsv4.cend();
	REQUIRE(*std::prev(it) == 'a');
	REQUIRE(*std::prev(it, 2) == 's');
}

TEST_CASE("Range iteration test") {
	const auto str = std::string("puppy");
	const fsv::filtered_string_view fsv5{str, [](const char& c) { return !(c == 'u' || c == 'y'); }};
	std::vector<char> result{fsv5.begin(), fsv5.end()};
	REQUIRE(result == std::vector<char>{'p', 'p', 'p'});
}

TEST_CASE("Reverse range iteration test") {
	const auto str = std::string("tosa");
	const fsv::filtered_string_view fsv6{str, [](const char& c) { return !(c == 'o' || c == 's'); }};
	std::vector<char> result{fsv6.rbegin(), fsv6.rend()};
	REQUIRE(result == std::vector<char>{'a', 't'});
}

TEST_CASE("Iterator const correctness") {
	fsv::filtered_string_view fsv1{"corgi"};
	auto it = fsv1.begin();
	REQUIRE(std::is_same_v<decltype(*it), const char&>);
}

TEST_CASE("Const iterator const correctness") {
	const auto str = std::string("tosa");
	const fsv::filtered_string_view fsv2{str};
	auto it = fsv2.cbegin();
	REQUIRE(std::is_same_v<decltype(*it), const char&>);
}

TEST_CASE("Reverse iterator const correctness") {
	fsv::filtered_string_view fsv3{"milo"};
	auto it = fsv3.rbegin();
	REQUIRE(std::is_same_v<decltype(*it), const char&>);
}

TEST_CASE("Const reverse iterator const correctness") {
	const auto str = std::string("tosa");
	const fsv::filtered_string_view fsv4{str};
	auto it = fsv4.crbegin();
	REQUIRE(std::is_same_v<decltype(*it), const char&>);
}