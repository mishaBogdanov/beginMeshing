#pragma once
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <cassert>



class MainEngine
{
public:
	template <class T, class B>
	int64_t AddOnUpdate(T func, B obj);
	template <class T>
	int64_t AddOnUpdate(T func);
	void    BeginExecution();
	void    RemoveOnUpdate(int64_t key) { mToExecute.erase(key); };
private:
	float time;
	std::unordered_map<int64_t, std::function<void()>> mToExecute;
};

template<class T, class B> int64_t
MainEngine::AddOnUpdate(T func, B obj)
{
	auto key = mToExecute.size();
	mToExecute[key] = std::bind_front(func, obj);
	return key;
}
template<class T> int64_t
MainEngine::AddOnUpdate(T func)
{
	auto key = mToExecute.size();
	mToExecute[key] = func;
	return key;
}

