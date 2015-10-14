#ifndef CS_PARTICLELIST_H_
#define CS_PARTICLELIST_H_

#include <algorithm>
#include <vector>
#include "particle.h"

namespace cs {

/**
 * @class cs::ParticleListT
 * @brief A collection of particles.
 * NOTE: This is currently a completely abstract collection class that
 * basically just prettifies the vector API. That said, I wouldn't be
 * too surprised if some particle-only behaviour appeared.
 */
template<typename T>
class ParticleListT {
public:
	using Container = std::vector<T>;
	using Iter = typename Container::iterator;
	using CIter = typename Container::const_iterator;

	ParticleListT<T>()			{ }

	ParticleListT<T>&		operator+=(const ParticleListT<T>&);
	ParticleListT<T>&		operator-=(const ParticleListT<T>&);
	T&						operator[](const size_t n);
	const T&				operator[](const size_t n) const;

	bool					empty() const { return mContainer.empty(); }
	void					clear()	{ mContainer.clear(); }
	size_t					size() const { return mContainer.size(); }
	void					push_back(const T&);
	T						get_and_pop_back();
	void					erase(Iter);
	void					erase(const T&);

	bool					contains(const T&) const;

	Iter					begin() { return mContainer.begin(); }
	Iter					end() { return mContainer.end(); }
	CIter					begin() const { return mContainer.begin(); }
	CIter					end() const { return mContainer.end(); }

	void					forEach(const std::function<void(T&)>&);

	void					sort(const std::function<bool(const T&, const T&)>&);

private:
	Container				mContainer;
};

using ParticleList = ParticleListT<ParticleRef>;

/**
 * cs::JotListT IMPLEMENTATION
 */
template<typename T>
ParticleListT<T>& ParticleListT<T>::operator+=(const ParticleListT<T> &o) {
	if (this != &o && !(o.mContainer.empty())) {
		mContainer.reserve(mActors.size() + o.mContainer.size());
		mContainer.insert(mActors.end(), o.mContainer.begin(), o.mContainer.end());
	}
	return *this;
}

template<typename T>
ParticleListT<T>& ParticleListT<T>::operator-=(const ParticleListT<T> &o) {
	if (this != &o && !mContainer.empty() && !(o.mContainer.empty())) {
		for (const auto& it : o.mContainer) erase(it);
	}
	return *this;
}

template<typename T>
T& ParticleListT<T>::operator[](const size_t n) {
	return mContainer[n];
}

template<typename T>
const T& ParticleListT<T>::operator[](const size_t n) const {
	return mContainer[n];
}

template<typename T>
void ParticleListT<T>::push_back(const T &a) {
	if (!a) return;
	try {
		mContainer.push_back(a);
	} catch (std::exception const&) {
	}
}

template<typename T>
T ParticleListT<T>::get_and_pop_back() {
	T t;
	if (!mContainer.empty()) {
		t = mContainer.back();
		mContainer.pop_back();
	}
	return t;
}

template<typename T>
void ParticleListT<T>::erase(Iter it) {
	try {
		mContainer.erase(it);
	} catch (std::exception const&) {
	}
}

template<typename T>
void ParticleListT<T>::erase(const T &t) {
	try {
		mContainer.erase(std::remove(mContainer.begin(), mContainer.end(), t), mContainer.end());
	} catch (std::exception const&) {
	}
}

template<typename T>
bool ParticleListT<T>::contains(const T &needle) const {
	return std::find(mContainer.begin(), mContainer.end(), needle) != mContainer.end();
}

template<typename T>
void ParticleListT<T>::forEach(const std::function<void(T&)> &fn) {
	if (!fn) return;
	for (auto& it : mContainer) fn(it);
}

template<typename T>
void ParticleListT<T>::sort(const std::function<bool(const T&, const T&)> &fn) {
	if (!fn) return;
	std::sort(mContainer.begin(), mContainer.end(), fn);
}

} // namespace cs

#endif
