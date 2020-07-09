#include <string>

#include <test/catch.hpp>
#include <util/prefixtrie.hpp>

namespace salmon {

	SCENARIO("insert_or_assign works as expected") {
		PrefixTrie<std::string, int> trie;
		WHEN("Two items with the same key are added") {
			std::vector<std::string> prefix = { "a" };
			bool isNew = trie.insert_or_assign(prefix, 4);
			THEN("The first time it is reported as new") {
				REQUIRE(isNew == true);
			}
			isNew = trie.insert_or_assign(prefix, 4);
			THEN("The second time it is reported as old") {
				REQUIRE(isNew == false);
			}
		}

		WHEN("A null prefix is added") {
			std::vector<std::string> prefix;
			bool isNew = trie.insert_or_assign(prefix, 1);
			THEN("The first time it is reported as new") {
				REQUIRE(isNew == true);
			}
			isNew = trie.insert_or_assign(prefix, 2);
			THEN("The second time it is reported as old") {
				REQUIRE(isNew == false);
			}
			THEN("The value is overwritten") {
				REQUIRE(trie.at(prefix) == 2);
			}
		}
	}

	SCENARIO("insert works as expected") {
		PrefixTrie<std::string,int> trie;
		WHEN("a new value is added") {
			std::vector<std::string> prefix = { "a", "b" };
			bool isNew = trie.insert(prefix, 1);
			THEN("isNew is true") {
				REQUIRE(isNew == true);
			}
			THEN("The value is correct") {
				REQUIRE(trie.at(prefix) == 1);
			}
		}
		WHEN("a value is overwritten") {
			std::vector<std::string> prefix = { "a", "b" };
			trie.insert(prefix, 1);
			bool isNew = trie.insert(prefix, 2);
			THEN("isNew is false") {
				REQUIRE(isNew == false);
			}
			THEN("the value wasn't overwritten") {
				REQUIRE(trie.at(prefix) == 1);
			}
		}
	}

	SCENARIO("Getting items works") {
		PrefixTrie<std::string, int> trie;

		WHEN("An attempt is made to extract an element from an empty trie") {
			std::vector<std::string> prefix;
			THEN("an exception is thrown") {
				REQUIRE_THROWS(trie.at(prefix));
			}
		}

		WHEN("A item with no prefix is added") {
			std::vector<std::string> prefix;
			trie.insert_or_assign(prefix, 4);
			int answer = trie.at(prefix);
			THEN("It is added correctly") {
				REQUIRE(answer == 4);
			}
		}

		WHEN("A item with one prefix is added") {
			std::vector<std::string> prefix = { "a" };
			trie.insert_or_assign(prefix, 4);
			int answer = trie.at(prefix);
			THEN("It is added correctly") {
				REQUIRE(answer == 4);
			}
		}

		WHEN("A item with two prefixes is added") {
			std::vector<std::string> prefix = { "a", "b" };
			trie.insert_or_assign(prefix, 4);
			int answer = trie.at(prefix);
			THEN("It is added correctly") {
				REQUIRE(answer == 4);
			}
		}

		WHEN("A item with one prefix is added") {
			std::vector<std::string> prefix = { "a" };
			trie.insert_or_assign(prefix, 4);
			int answer = trie.at(prefix);
			THEN("It is added correctly") {
				REQUIRE(answer == 4);
			}
		}

		WHEN("A item with one prefix is added") {
			std::vector<std::string> prefix = { "a" };
			trie.insert_or_assign(prefix, 4);
			prefix.push_back("b");
			trie.insert_or_assign(prefix, 2);
			int answer = trie.at(prefix);
			THEN("It is added correctly") {
				REQUIRE(answer == 2);
			}
		}
	}

	SCENARIO("Using a different comparator compiles") {
		PrefixTrie<std::string,int,std::greater<>> trie;
		WHEN("insert is used") {
			std::vector<std::string> prefix = {"a", "b"};
			bool isNew = trie.insert(prefix, 1);
			THEN("It works!") {
				REQUIRE(isNew == true);
			}
		}
		WHEN("insert_or_assign is used") {
			std::vector<std::string> prefix = {"a", "b"};
			bool isNew = trie.insert_or_assign(prefix, 1);
			THEN("It works!") {
				REQUIRE(isNew == true);
			}
		}

		WHEN("at is used") {
			std::vector<std::string> prefix = {"a", "b"};
			THEN("It works!") {
				REQUIRE_THROWS(trie.at(prefix));
			}
		}
	}

	SCENARIO("all_values works") {
		PrefixTrie<std::string,int,std::greater<>> trie;
		GIVEN("A tree with multiple levels") {
			trie.insert_or_assign({}, 0);
			trie.insert_or_assign({ "a" }, 1);
			trie.insert_or_assign({ "b" }, 2);
			trie.insert_or_assign({ "c" }, 3);
			trie.insert_or_assign({ "b", "a" }, 4);
			trie.insert_or_assign({ "b", "b" }, 5);
			trie.insert_or_assign({ "c", "d" }, 6);
			WHEN("The roots are queried") {
				std::set<std::string> keys;
				std::set<int> values;
				trie.all_values([&keys](const std::string &item) {
					keys.insert(item);
				}, [&values](int item) {
					values.insert(item);
				});
				THEN("Everything is accounted for") {
					std::set<std::string> required_keys = {"a","b","c","d"};
					std::set<int> required_values = {0, 1, 2, 3, 4, 5, 6};
					REQUIRE(required_keys == keys);
					REQUIRE(required_values == values);
				}
			}
		}

	}
}
