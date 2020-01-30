#pragma once

#include <core/std.h>
#include <core/platform.h>
#include <core/logger.h>
#include <core/collections/iterator.h>
#include <core/memory.h>

template<typename T>
struct DynamicArray : Iterable<DynamicArray<T>, T> {
	T* data = nullptr;
	u32 count = 0;
	u32 capacity = 0;

	DynamicArray() {
	}

	DynamicArray(const std::initializer_list<T>&& list) {
		for(const T& item : list) {
			add(item);
		}
	}

	DynamicArray(const DynamicArray<T>& rhs);
	DynamicArray(DynamicArray<T>&& rhs);
	~DynamicArray();

	DynamicArray<T>& operator=(const DynamicArray<T>& rhs);
	DynamicArray<T>& operator=(DynamicArray<T>&& rhs);

	T& getRef(u32 i) const;
	T* getPtr(u32 i) const;
	const T& getRefConst(u32 i) const;
	const T* getPtrConst(u32 i) const;
	T getCopy(u32 i) const;

	void add(const T& element);
	void removeAtIndexUnordered(const u32 index); // NOTE(nathan): unordered remove and swap
	void clear();
	void reserve(u32 count);
	bool contains(const T& query) const;


	// ======= Iterator overrides =======
	u32 getFirstIndex() const final;
	u32 getLastIndex() const final;
	s32 getNextIndex(const int current_index) const final;
	T& getNthItem(const int n) final;
	const T& getNthItem(const int n) const final;


private:

	void freeData();
	void copyOther(const DynamicArray<T>& rhs);
	void moveOther(DynamicArray<T>& rhs);
};

// ===============================================================

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& rhs) {
	copyOther(rhs);
}

template<typename T>
DynamicArray<T>::DynamicArray(DynamicArray<T>&& rhs) {
	moveOther(rhs);
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& rhs) {
	if(this != &rhs) {
		if(data != nullptr) {
			freeData();
		}
		copyOther(rhs);
	}
	return *this;
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(DynamicArray<T>&& rhs) {
	if(this != &rhs) {
		if(data != nullptr) {
			freeData();
		}
		moveOther(rhs);
	}
	return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray() {
	freeData();
}

template<typename T>
T& DynamicArray<T>::getRef(u32 i) const {
	Assert(i >= 0 && i < count);
	return data[i];
}

template<typename T>
T* DynamicArray<T>::getPtr(u32 i) const {
	Assert(i >= 0 && i < count);
	return &data[i];
}

template<typename T>
const T& DynamicArray<T>::getRefConst(u32 i) const {
	Assert(i >= 0 && i < count);
	return data[i];
}

template<typename T>
const T* DynamicArray<T>::getPtrConst(u32 i) const {
	Assert(i >= 0 && i < count);
	return &data[i];
}

template<typename T>
T DynamicArray<T>::getCopy(u32 i) const {
	Assert(i >= 0 && i < count);
	return data[i];
}

template<typename T>
void DynamicArray<T>::add(const T& element) {
	if(count == capacity) {
		capacity += 16;
		T* new_data = (T*)Platform::alloc(capacity * sizeof(T));
		if(std::is_destructible<T>::value) {
			for(u32 i = 0; i < count; i++) {
				new(&new_data[i]) T();
				new_data[i] = data[i];
			}
		}

		// data = (T *)Platform::reAlloc(data, capacity * sizeof(T));
		Platform::free(data);
		data = new_data;
	}
	if(std::is_constructible<T>::value) {
		new(&data[count]) T();
	}
	data[count] = element; // copy
	count++;
}

template<typename T>
void DynamicArray<T>::removeAtIndexUnordered(const u32 index) {
	Assert(index >= 0 && index < count);
	if(count > 1 && index != count-1) {
		T& last = getRef(count-1);
		T& to_remove = getRef(index);
		to_remove = last;
	}

	count--;
}

template<typename T>
void DynamicArray<T>::clear() {
	count = 0;
}

template<typename T>
void DynamicArray<T>::reserve(u32 count) {
	capacity = count;
	data = (T *)Platform::reAlloc(data, capacity * sizeof(T));
}

template<typename T>
bool DynamicArray<T>::contains(const T& query) const {
	for(u32 i = 0; i < count; i++) {
		if(data[i] == query) {
			return true;
		}
	}
	return false;
}

template<typename T>
u32 DynamicArray<T>::getFirstIndex() const {
	return 0;
}

template<typename T>
u32 DynamicArray<T>::getLastIndex() const {
	return count;
}

template<typename T>
s32 DynamicArray<T>::getNextIndex(const int current_index) const {
	return current_index + 1;
}

template<typename T>
T& DynamicArray<T>::getNthItem(const int n) {
	return data[n];
}

template<typename T>
const T& DynamicArray<T>::getNthItem(const int n) const {
	return data[n];
}

template<typename T>
void DynamicArray<T>::freeData() {
	if(std::is_destructible<T>::value) {
		for(u32 i = 0; i < count; i++) {
			data[i].~T();
		}
	}

	Platform::free(data);
	data = nullptr;
	count = 0;
	capacity = 0;
}

template<typename T>
void DynamicArray<T>::copyOther(const DynamicArray<T>& rhs) {
	count = rhs.count;
	capacity = rhs.capacity;
	data = (T *)Memory_alloc(capacity * sizeof(T));
	
	for(u32 i = 0; i < count; i++) {
		if(std::is_constructible<T>::value) {
			new(&data[i]) T();
		}
		T copy = rhs.getCopy(i);
		data[i] = copy;
	}
}

template<typename T>
void DynamicArray<T>::moveOther(DynamicArray<T>& rhs) {
	Swap(data, rhs.data);
	Swap(count, rhs.count);
	Swap(capacity, rhs.capacity);
}
