#pragma once

#include <core/std.h>
#include <core/platform.h>
#include <core/logger.h>
#include <core/collections/iterator.h>
#include <core/memory.h>

template<typename T>
struct Buffer : Iterable<Buffer<T>, T> {
	T* data = nullptr;
	u32 capacity = 0;

	Buffer() {
	}

	Buffer(const std::initializer_list<T>&& list) {
		for(const T& item : list) {
			add(item);
		}
	}

	Buffer(const Buffer<T>& rhs);
	Buffer(Buffer<T>&& rhs);
	~Buffer();

	Buffer<T>& operator=(const Buffer<T>& rhs);
	Buffer<T>& operator=(Buffer<T>&& rhs);

	T& getRef(u32 i) const;
	T* getPtr(u32 i) const;
	const T& getRefConst(u32 i) const;
	const T* getPtrConst(u32 i) const;
	T getCopy(u32 i) const;

	void reserve(u32 capacity);
	bool contains(const T& query) const;


	// ======= Iterator overrides =======
	u32 getFirstIndex() const final;
	u32 getLastIndex() const final;
	s32 getNextIndex(const int current_index) const final;
	T& getNthItem(const int n) final;
	const T& getNthItem(const int n) const final;


private:

	void freeData();
	void copyOther(const Buffer<T>& rhs);
	void moveOther(Buffer<T>& rhs);
};

// ===============================================================

template<typename T>
Buffer<T>::Buffer(const Buffer<T>& rhs) {
	copyOther(rhs);
}

template<typename T>
Buffer<T>::Buffer(Buffer<T>&& rhs) {
	moveOther(rhs);
}

template<typename T>
Buffer<T>& Buffer<T>::operator=(const Buffer<T>& rhs) {
	if(this != &rhs) {
		if(data != nullptr) {
			freeData();
		}
		copyOther(rhs);
	}
	return *this;
}

template<typename T>
Buffer<T>& Buffer<T>::operator=(Buffer<T>&& rhs) {
	if(this != &rhs) {
		if(data != nullptr) {
			freeData();
		}
		moveOther(rhs);
	}
	return *this;
}

template<typename T>
Buffer<T>::~Buffer() {
	freeData();
}

template<typename T>
T& Buffer<T>::getRef(u32 i) const {
	Assert(i >= 0 && i < capacity);
	return data[i];
}

template<typename T>
T* Buffer<T>::getPtr(u32 i) const {
	Assert(i >= 0 && i < capacity);
	return &data[i];
}

template<typename T>
const T& Buffer<T>::getRefConst(u32 i) const {
	Assert(i >= 0 && i < capacity);
	return data[i];
}

template<typename T>
const T* Buffer<T>::getPtrConst(u32 i) const {
	Assert(i >= 0 && i < capacity);
	return &data[i];
}

template<typename T>
T Buffer<T>::getCopy(u32 i) const {
	Assert(i >= 0 && i < capacity);
	return data[i];
}

template<typename T>
void Buffer<T>::reserve(u32 capacity) {
	this->capacity = capacity;
	data = (T *)Platform::reAlloc(data, capacity * sizeof(T));
}

template<typename T>
bool Buffer<T>::contains(const T& query) const {
	for(u32 i = 0; i < capacity; i++) {
		if(data[i] == query) {
			return true;
		}
	}
	return false;
}

template<typename T>
u32 Buffer<T>::getFirstIndex() const {
	return 0;
}

template<typename T>
u32 Buffer<T>::getLastIndex() const {
	return capacity;
}

template<typename T>
s32 Buffer<T>::getNextIndex(const int current_index) const {
	return current_index + 1;
}

template<typename T>
T& Buffer<T>::getNthItem(const int n) {
	return data[n];
}

template<typename T>
const T& Buffer<T>::getNthItem(const int n) const {
	return data[n];
}

template<typename T>
void Buffer<T>::freeData() {
	if(std::is_destructible<T>::value) {
		for(u32 i = 0; i < capacity; i++) {
			data[i].~T();
		}
	}

	Platform::free(data);
	data = nullptr;
	capacity = 0;
}

template<typename T>
void Buffer<T>::copyOther(const Buffer<T>& rhs) {
	capacity = rhs.capacity;
	data = (T *)Memory_alloc(capacity * sizeof(T));
	
	for(u32 i = 0; i < capacity; i++) {
		if(std::is_constructible<T>::value) {
			new(&data[i]) T();
		}
		T copy = rhs.getCopy(i);
		data[i] = copy;
	}
}

template<typename T>
void Buffer<T>::moveOther(Buffer<T>& rhs) {
	Swap(data, rhs.data);
	Swap(capacity, rhs.capacity);
}
