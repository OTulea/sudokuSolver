#pragma once
#include <iostream>
#include <string>
#include <mutex>
#include <queue>

class inputBuffer {
public:
	void push(std::string input){
		std::lock_guard<std::mutex> lock(bufferMutex);
		buffer.push(input);
	}
	void pop(){
		std::lock_guard<std::mutex> lock(bufferMutex);
		buffer.pop();
	}
private:
	std::queue <std::string> buffer;
	mutable std::mutex bufferMutex;
};