#include <iostream>
#include "Inventory.h"

Inventory::Inventory() {
	Grid.resize(r);
	for (auto& row : Grid) {
		row[c] = Item(-1, "NULL", 0, 0);
	}
}

Inventory::~Inventory() {
	Grid.clear();
}

void Inventory::AddItems(Item item, int xPos, int yPos) {
	Grid[xPos][yPos] = item;
	item.count++;

}

void Inventory::RemoveItems(Item item, int xPos, int yPos) {
	item.count--;
}

void Inventory::CraftItems() {

}

void Inventory::SwapItems() {

}