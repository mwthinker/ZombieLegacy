#ifndef ARRAY_H
#define ARRAY_H

#include <array>
#include <utility>
#include <cassert>

namespace zombie {

	template <class E, unsigned int SIZE>
	class Array {
	public:
		inline Array() : size_(0) {
		}

		template <class... Args>
		E* emplaceBack(Args&&... args) {
			assert(size_ < SIZE);
			return &(data_[size_++] = E(std::forward<Args>(args)...));
		}

		inline E* pushBack(const E& e) {
			assert(size_ < SIZE);
			return &(data_[size_++] = e);
		}

		inline typename std::array<E, SIZE>::iterator begin() {
			return data_.begin();
		}

		inline typename std::array<E, SIZE>::iterator end() {
			return data_.begin() + size_;
		}

		inline typename std::array<E, SIZE>::const_iterator begin() const {
			return data_.cbegin();
		}

		inline typename std::array<E, SIZE>::const_iterator end() const {
			return data_.cbegin() + size_;
		}

		inline unsigned int getSize() const {
			return size_;
		}

		inline unsigned int getMaxSize() const {
			return data_.size();
		}

		inline E& operator[](unsigned int index) {
			assert(index < size_);
			return data_[index];
		}

		const E& operator[](unsigned int index) const {
			assert(index < size_);
			return data_[index];
		}

	private:
		std::array<E, SIZE> data_;
		unsigned int size_;
	};

} // Namespace zombie.

#endif // ARRAY_H
