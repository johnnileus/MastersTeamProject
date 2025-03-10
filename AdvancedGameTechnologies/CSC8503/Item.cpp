#include <iostream>
#include "Item.h"

Item::Item(int id, std::string name, int stackSize, int count) {
	this->id = id;
	this->name = name;
	this->stackSize = stackSize;
	this->count = count;
}

Item::~Item() {

}