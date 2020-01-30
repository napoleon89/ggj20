#pragma once 

#include <functional>

template<typename T>
struct DynamicArray;

template<typename ...Args>
struct Event {
public:
private:
	typedef std::function<void(Args...)> StaticCallback;
	typedef std::function<void(void*, Args...)> MemberCallback;

	struct MemberCallbackObj {
		MemberCallback callback;
		void *instance;
	};

	DynamicArray<StaticCallback> static_callbacks;
	DynamicArray<MemberCallbackObj> member_callbacks;

	template<class C, void (C:: * Function)(Args...)>
	static void classMethodStub(void *instance, Args... args) {
		return (static_cast<C *>(instance)->*Function)(args...);
	}
	
public:

	void addCallback(const StaticCallback &callback) {
		static_callbacks.add(callback);
	}

	template<class C, void (C:: * Function)(Args...)>
	void addMemberCallback(C *instance) {
		MemberCallbackObj obj = {};
		obj.instance = static_cast<void *>(instance);
		obj.callback = &classMethodStub<C, Function>;
		member_callbacks.add(obj);
	}

	void emit(const Args&... args) {
		for(auto &callback : static_callbacks) {
			callback(args...);
		}
		
		for(auto &member_callback : member_callbacks) {
			member_callback.callback(member_callback.instance, args...);
		}
	}


};