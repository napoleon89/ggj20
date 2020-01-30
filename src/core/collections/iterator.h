#pragma once

#include <core/std.h>

template<typename CollectionType, typename ReturnType>
struct BaseIterator {
	CollectionType *data = nullptr;
	u32 index = 0;

	bool operator==(const BaseIterator &rhs) const;
	bool operator!=(const BaseIterator &rhs) const;

	BaseIterator &operator++();
	BaseIterator operator++(int);

	ReturnType &operator*();
};

template<typename CollectionType, typename ReturnType>
struct Iterable {
	typedef BaseIterator<CollectionType, ReturnType> Iterator;
	typedef BaseIterator<const CollectionType, const ReturnType> ConstIterator;

	virtual u32 getFirstIndex() const = 0;
	virtual u32 getLastIndex() const = 0;
	virtual s32 getNextIndex(const int current_index) const = 0;

	virtual ReturnType &getNthItem(const int n) = 0;
	virtual const ReturnType &getNthItem(const int n) const = 0;

	Iterator begin() { return  Iterator{ (CollectionType *)this, getFirstIndex() }; }
	Iterator end() { return  Iterator{ (CollectionType *)this, getLastIndex() }; }

	ConstIterator begin() const { return  ConstIterator{ (const CollectionType *)this, getFirstIndex() }; }
	ConstIterator end() const { return  ConstIterator{ (const CollectionType *)this, getLastIndex() }; }
};

template<typename T, typename U>
bool BaseIterator<T, U>::operator==(const BaseIterator<T, U> &rhs) const {
	return data == rhs.data && index == rhs.index;
}

template<typename T, typename U>
bool BaseIterator<T, U>::operator!=(const BaseIterator<T, U> &rhs) const {
	return data != rhs.data || index != rhs.index;
}

template<typename T, typename U>
BaseIterator<T, U> BaseIterator<T, U>::operator++(int) {
	BaseIterator<T, U> result = *this;
	++ *this;
	return result;
}

template<typename T, typename U>
BaseIterator<T, U> &BaseIterator<T, U>::operator++() {
	T *collection = (T *)data;
	index = collection->getNextIndex(index);
	return *this;
}

template<typename T, typename U>
U &BaseIterator<T, U>::operator*() {
	T *collection = (T *)data;
	return collection->getNthItem(index);
}
