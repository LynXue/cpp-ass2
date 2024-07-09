#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <cstring>
#include <set>

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

TEST_CASE("Iterators") {
	std::string str = "iterator";
	fsv::filtered_string_view sv(str);
	std::string result;
	for (auto it = sv.begin(); it != sv.end(); ++it) {
		result.push_back(*it);
	}
	REQUIRE(result == str);
}
