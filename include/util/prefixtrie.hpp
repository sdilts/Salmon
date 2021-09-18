#include <map>
#include <vector>
#include <memory>
#include <stdexcept>

#include <assert.h>
#include <iostream>
#include <functional>
#include <set>
#include <optional>

namespace salmon {

	template<typename K, typename V, typename Compare = std::less<K>>
	class PrefixTrie {
		struct Node {
			std::optional<V> item;
			std::map<K,std::unique_ptr<Node>,Compare> tree;
		};
		size_t _size = 0;
		Node tree;

	    Node *insert_uninitialized(Node *cur,
								   typename std::vector<K>::const_iterator start,
								   typename std::vector<K>::const_iterator end) {
			while(start != end) {
				std::unique_ptr<Node> new_tree;
				new_tree = std::make_unique<Node>();
				auto [iter, is_new] = cur->tree.emplace(*start, std::move(new_tree));
				cur = iter->second.get();
				start++;
			}
			return cur;
		}

		static V &at_helper(Node *cur, const std::vector<K> &prefixes) {
			for(const auto &item : prefixes) {
				const auto place = cur->tree.find(item);
				if(place == cur->tree.end()) {
					throw new std::out_of_range("Invalid prefix");
				} else {
					cur = place->second.get();
				}
			}
			if(cur->item.has_value()) {
				return *(cur->item);
			} else {
				throw new std::out_of_range("Invalid prefix");
			}
		}

	public:
		std::ostream &operator<<(std::ostream &out) const {
			out << "{PrefixTrie: ";
			if(tree.item.has_value()) {
				out << tree.item.value() << " ";
			} else {
				out << " ";
			}
			for(const auto &[key, subNode] : tree) {
				out << " { " << key << " : "
					<< subNode
					<< "}";
			}
			out << "}";
			return out;
		}

		size_t size() const {
			return size;
		}

		bool insert(const std::vector<K> &prefixes, const V &item) {
			V copy_of_item(item);
			return insert(prefixes, std::move(copy_of_item));
		}

		bool insert(const std::vector<K> &prefixes, const V &&item) {
			auto iter = prefixes.cbegin();
		    Node *cur = &tree;
			while(iter != prefixes.cend()) {
				const auto place = cur->tree.lower_bound(*iter);
				if(place == cur->tree.end() || place->first != *iter) {
					// emplace this one, as we already know where it goes:
					auto new_tree = std::make_unique<Node>();
					auto new_place = cur->tree.emplace_hint(place, *iter, std::move(new_tree));
					assert(new_place != cur->tree.end());
					iter++;
					// now do the rest:
				    Node *tail = insert_uninitialized(new_place->second.get(),
																 iter, prefixes.end());
					tail->item = item;
					_size++;
					return true;
				} else {
					cur = (*place).second.get();
				}
				iter++;
			}
			if(cur->item.has_value()) {
				return false;
			} else {
				cur->item = item;
				_size++;
				return true;
			}
		}

		bool insert_or_assign(const std::vector<K> &prefixes, const V &item) {
			// This assumes that moving is really cheap:
			V copy_of_item(item);
			return insert_or_assign(prefixes, std::move(copy_of_item));
		}

		//! returns true if the element is new:
		bool insert_or_assign(const std::vector<K> &prefixes, V &&item) {
			auto iter = prefixes.cbegin();
			Node *cur = &tree;
			while(iter != prefixes.cend()) {
				const auto place = cur->tree.lower_bound(*iter);
				if(place == cur->tree.end() || place->first != *iter) {
					// emplace this one, as we already know where it goes:
					auto new_tree = std::make_unique<Node>();
					auto new_place = cur->tree.emplace_hint(place, *iter, std::move(new_tree));
					assert(new_place != cur->tree.end());
					iter++;
					// now do the rest:
					Node *tail = insert_uninitialized(new_place->second.get(),
																 iter, prefixes.end());
					tail->item = item;
					_size++;
					return true;
				} else {
					cur = (*place).second.get();
				}
				iter++;
			}
			if(cur->item.has_value()) {
				cur->item = item;
				return false;
			} else {
				cur->item = item;
				_size++;
				return true;
			}
		}

		V &operator[](const std::vector<K> &prefixes) {
			// TODO: use unchecked function:
			return this->at(prefixes);
		}

		const V &operator[](const std::vector<K> prefixes) const {
			// TODO: use unchecked function:
			return this->at(prefixes);
		}

		const V &at(const std::vector<K> &prefixes) const {
			return at_helper(&tree, prefixes);
		}

		V &at(const std::vector<K> &prefixes) {
			return at_helper(&tree, prefixes);
		}

	    void all_values(const std::function<void(const K&)> &key_fn,
						const std::function<void(const V&)> &value_fn) const {
			std::vector<const Node*> stack;
			stack.push_back(&tree);

			while(!stack.empty()) {
				const Node *cur = stack.back();
				stack.pop_back();

				if(cur->item) {
				    value_fn(*cur->item);
				}
				for(const auto &[key, child] : cur->tree) {
					key_fn(key);
					stack.push_back(child.get());
				}
			}
		}
	};
}
