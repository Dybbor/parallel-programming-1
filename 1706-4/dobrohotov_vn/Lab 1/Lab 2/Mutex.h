#pragma once
class Mutex 
{
private:
	bool val;
public:
	Mutex() { val = true; }
	void lock() { val = false;  }
	void unlock() { val = true; }
	bool Info() { return val; }
};