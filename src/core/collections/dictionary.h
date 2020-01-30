#pragma once

#include <core/platform.h>
#include <core/logger.h>
#include <core/math.h>
#include <core/utils/hashing.h>
#include <core/collections/iterator.h>
#include <tuple>
#include <core/memory.h>
#include <cstring>

template<typename K, typename V>
struct DictionaryPair {
	K key;
	V value;
};

template<typename KeyType, typename ValueType>
struct Dictionary : Iterable<Dictionary<KeyType, ValueType>, DictionaryPair<KeyType, ValueType>> {

	enum class ItemState {
		Empty,
		Full,
		Deleted,
	};

	inline static const u32 INITIAL_BASE_SIZE = 10;

	ItemState* item_states = nullptr;
	DictionaryPair<KeyType, ValueType>* items = nullptr;
	u32 size = 0;
	u32 count = 0;
	u32 base_size = 0;

	Dictionary() : Dictionary(INITIAL_BASE_SIZE) {
	}

	Dictionary(const u32 new_base_size);
	Dictionary(const Dictionary<KeyType, ValueType>& rhs);
	Dictionary(Dictionary<KeyType, ValueType>&& rhs);
	~Dictionary();

	Dictionary<KeyType, ValueType>& operator=(const Dictionary<KeyType, ValueType>& rhs);
	Dictionary<KeyType, ValueType>& operator=(Dictionary<KeyType, ValueType>&& rhs);

	void resize(const u32 new_size);
	ValueType* insertItem(const KeyType& key, const ValueType& value);
	ValueType* search(const KeyType& key) const;
	void deleteItem(const std::string& key);

	// ======= Iterator overrides =======
	u32 getFirstIndex() const final;
	u32 getLastIndex() const final;
	s32 getNextIndex(const int current_index) const final;
	DictionaryPair<KeyType, ValueType>& getNthItem(const int n) final;
	const DictionaryPair<KeyType, ValueType>& getNthItem(const int n) const final;

private:
	void freeData();
	void allocData();
	void copyOther(const Dictionary<KeyType, ValueType>& rhs);
	void moveOther(Dictionary<KeyType, ValueType>& rhs);
};

// ===============================================================================

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary(const u32 new_base_size) {
	count = 0;
	base_size = new_base_size;
	size = Math::nextPrime(base_size);

	allocData();
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary(const Dictionary<KeyType, ValueType>& rhs) {
	copyOther(rhs);
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::Dictionary(Dictionary<KeyType, ValueType>&& rhs) {
	moveOther(rhs);
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>::~Dictionary() {
	freeData();
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>& Dictionary<KeyType, ValueType>::operator=(const Dictionary<KeyType, ValueType>& rhs) {
	if(this != &rhs) {
		if(items != nullptr && item_states != nullptr) {
			freeData();
		}
		copyOther(rhs);
	}
	return *this;
}

template<typename KeyType, typename ValueType>
Dictionary<KeyType, ValueType>& Dictionary<KeyType, ValueType>::operator=(Dictionary<KeyType, ValueType>&& rhs) {
	if(this != &rhs) {
		if(items != nullptr && item_states != nullptr) {
			freeData();
		}
		moveOther(rhs);
	}
	return *this;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::resize(const u32 new_size) {
	if(new_size < INITIAL_BASE_SIZE) return;

	Dictionary<KeyType, ValueType> new_dictionary(new_size);

	for(u32 i = 0; i < size; i++) {
		const DictionaryPair<KeyType, ValueType>& item = items[i];
		const ItemState item_state = item_states[i];
		if(item_state == ItemState::Full) {
			new_dictionary.insertItem(item.key, item.value);
		}
	}

	// move current data into stack backup, so it's cleared at the end of the scope
	Swap(new_dictionary.items, items);
	Swap(new_dictionary.item_states, item_states);
	Swap(new_dictionary.count, count);
	Swap(new_dictionary.size, size);
	Swap(new_dictionary.base_size, base_size);
}

template<typename KeyType, typename ValueType>
ValueType* Dictionary<KeyType, ValueType>::insertItem(const KeyType& key, const ValueType& value) {
	const u32 load = count * 100 / size;
	if(load > 70) {
		const u32 new_size = base_size * 2;
		resize(new_size);
	}

	u32 index = Hashing::getHashindex(key, size, 0);
	ItemState current_item_state = item_states[index];
	DictionaryPair<KeyType, ValueType>* pair = &items[index];
	u32 i = 1;
	while(current_item_state != ItemState::Empty) {
		if(current_item_state != ItemState::Deleted) {
			if(pair->key == key) {
				items[index].value = value;
				return &pair->value;
			}
		}
		index = Hashing::getHashindex(key, size, i);
		current_item_state = item_states[index];
		pair = &items[index];
		i++;
	}

	new(pair) DictionaryPair<KeyType, ValueType>();
	pair->value = value;
	pair->key = key;
	item_states[index] = ItemState::Full;
	count++;
	return &(items[index].value);
}

template<typename KeyType, typename ValueType>
ValueType* Dictionary<KeyType, ValueType>::search(const KeyType& key) const {
	u32 index = Hashing::getHashindex(key, size, 0);
	ItemState item_state = item_states[index];
	DictionaryPair<KeyType, ValueType>* pair = &items[index];
	u32 i = 1;
	while(item_state != ItemState::Empty) {
		if(item_state != ItemState::Deleted) {
			if(pair->key == key) {
				return &pair->value;
			}
		}

		index = Hashing::getHashindex(key, size, i);
		item_state = item_states[index];
		pair = &items[index];
		i++;
	}

	return nullptr;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::deleteItem(const std::string& key) {
	u32 index = Hashing::getHashindex(key, size, 0);
	ItemState* item_state = &item_states[index];
	DictionaryPair<KeyType, ValueType>* pair = &items[index];
	u32 i = 1;
	while(*item_state != ItemState::Empty) {
		if(*item_state != ItemState::Deleted) {
			if(pair->key == key) {
				*item_state = ItemState::Deleted;
				*pair = {};
				count--;

				const u32 load = count * 100 / size;
				if(load < 10) {
					const u32 new_size = base_size / 2;
					resize(new_size);
				}

				return;
			}
		}
		index = Hashing::getHashindex(key, size, i);
		item_state = &item_states[index];
		pair = &items[index];
		i++;
	}
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::freeData() {
	for(u32 i = 0; i < size; i++) {
		if(item_states[i] == ItemState::Full) {
			items[i].~DictionaryPair<KeyType, ValueType>();
		}
	}
	Platform::free(items);
	Platform::free(item_states);
	items = nullptr;
	item_states = nullptr;
	size = 0;
	count = 0;
	base_size = 0;
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::allocData() {
	items = (DictionaryPair<KeyType, ValueType> *)Memory_alloc(size * sizeof(DictionaryPair<KeyType, ValueType>));
	item_states = (ItemState *)Memory_alloc(size * sizeof(ItemState));
	std::memset(items, 0, size * sizeof(DictionaryPair<KeyType, ValueType>));
	std::memset(item_states, 0, size * sizeof(ItemState));
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::copyOther(const Dictionary<KeyType, ValueType>& rhs) {
	count = rhs.count;
	base_size = rhs.base_size;
	size = rhs.size;

	allocData();
	for(u32 i = 0; i < size; i++) {
		const ItemState state = rhs.item_states[i];
		item_states[i] = state;
		if(state == ItemState::Full) {
			new(&items[i]) DictionaryPair<KeyType, ValueType>();
			items[i] = rhs.items[i];
		}
		

	}
}

template<typename KeyType, typename ValueType>
void Dictionary<KeyType, ValueType>::moveOther(Dictionary<KeyType, ValueType>& rhs) {
	Swap(items, rhs.items);
	Swap(item_states, rhs.item_states);
	Swap(count, rhs.count);
	Swap(size, rhs.size);
	Swap(base_size, rhs.base_size);
}

template<typename KeyType, typename ValueType>
u32 Dictionary<KeyType, ValueType>::getFirstIndex() const {
	for(u32 i = 0; i < size; i++) {
		if(item_states[i] == ItemState::Full) {
			return i;
			break;
		}
	}
	return size;
}

template<typename KeyType, typename ValueType>
u32 Dictionary<KeyType, ValueType>::getLastIndex() const {
	return size;
}

template<typename KeyType, typename ValueType>
s32 Dictionary<KeyType, ValueType>::getNextIndex(const int current_index) const {
	for(u32 i = current_index + 1; i < size; i++) {
		if(item_states[i] == ItemState::Full) {
			return i;
			break;
		}
	}
	return size;
}

template<typename KeyType, typename ValueType>
DictionaryPair<KeyType, ValueType>& Dictionary<KeyType, ValueType>::getNthItem(const int n) {
	return items[n];
}

template<typename KeyType, typename ValueType>
const DictionaryPair<KeyType, ValueType>& Dictionary<KeyType, ValueType>::getNthItem(const int n) const {
	return items[n];
}
