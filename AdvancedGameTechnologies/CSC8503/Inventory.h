#pragma once
#include <iostream>
#include <vector>
#include "Item.h"

using namespace std;

class Inventory {
private:
	const int r = 3; //row
	const int c = 3; //column
	vector<vector<Item>> Grid; //Inventory system grid

public:
	Inventory();
	~Inventory();

	/*
	* TODO:
	* 1. Add/Remove items - so items should be added at specific position in the gird
	* HealthPorition = 0, sword - 1, gun - 2
	* 2. Slots
	* 3. Stacking
	* 4. Swaping (swap guns in loadout)
	*/

	void AddItems(Item item, int xPos, int yPos);
	void RemoveItems(Item item, int xPos, int yPos);

	//TODO: Rework on crafting logic after implementing add and remove items
	void CraftItems();
	void SwapItems();

};
