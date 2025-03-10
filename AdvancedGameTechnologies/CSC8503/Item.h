#pragma once
#include <iostream>

class Item {
public:
	Item(int id, std::string name, int stackSize, int count);
	~Item();

	int id; //item id
	std::string name; // item name
	int stackSize = 0; //maximum grid threshold
	int count = 0; // Total collected item value

	//TODO: texture to be implemented 

};
