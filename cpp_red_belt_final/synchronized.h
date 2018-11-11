#pragma once

#include <mutex>

using namespace std;

template <typename T>
struct Access {
	T& ref_to_value;
	lock_guard<mutex> guard;
};

template <typename T>
class Synchronized {
public:
	explicit Synchronized(T initial = T())
		: value(move(initial))
	{
	}
	
	Access<T> GetAccess() {
		return { value, lock_guard(m) };
	}

private:
	T value;
	mutex m;
};