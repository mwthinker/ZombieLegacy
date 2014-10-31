#ifndef ARRAY_H
#define ARRAY_H

#include <array>

namespace zombie {

	template <class E, int _size>
	class Array {
	public:
		inline Array() : size_(0) {
		}

		inline E* create(const E& e) {
			return &(data_[size_++] = e);
		}

		inline typename std::array<E, _size>::iterator begin() {
			return data_.begin();
		}

		inline typename std::array<E, _size>::iterator end() {
			return data_.end();
		}

		inline typename std::array<E, _size>::const_iterator begin() const {
			return data_.cbegin();
		}

		inline typename std::array<E, _size>::const_iterator end() const {
			return data_.cend();
		}

		inline int getSize() const {
			return size_;
		}

		inline int getMaxSize() const {
			return data_.size();
		}

		inline E& operator[](int index) {
			return data_[index];
		}

		inline E operator[](int index) const {
			return data_[index];
		}

	private:
		std::array<E, _size> data_;
		int size_;
	};

} // Namespace zombie.

#endif // ARRAY_H
