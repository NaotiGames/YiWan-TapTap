#pragma once
#include <mutex>

template<typename T>
class FTapSingleton {
public:
	static T &GetInstance() {
		std::call_once(initFlag, [] {
			instance = MakeShared<T>();
		});
		return *instance;
	}

private:
	static TSharedPtr<T> instance;
	static std::once_flag initFlag;
};

template<typename T>
TSharedPtr<T> FTapSingleton<T>::instance = nullptr;

template<typename T>
std::once_flag FTapSingleton<T>::initFlag;

