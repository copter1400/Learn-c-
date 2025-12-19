#include <iostream>
#include <cstdint>
#include <string>
#include <iomanip>

enum class itemType {Weapon, Book, Armor, Potion};

struct item {
  std::string name;    //Name of the item
  itemType type;       //Type of the item
  uint8_t level;       //Level of the item

  //Weapon
  uint16_t damage;     //How much damage it can deal
  uint16_t weapon_durability; //How much durability it has left
  //Book
  std::string author;  //Author of the book
  std::string title;   //Title of the book
  std::string text;    //Content of the book
  //Armor
  uint8_t defense;     //How much damage it can reduce
  uint8_t armor_durability;  //How much durability it has left
};

item inventory[10];


void initializeInventory(item* inventory_p, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    inventory_p[i] = item{}; // value-initialize
    inventory_p[i].name = "";
    inventory_p[i].type = itemType::Potion;
    inventory_p[i].level = 0;
    inventory_p[i].damage = 0;
    inventory_p[i].weapon_durability = 0;
    inventory_p[i].author = "";
    inventory_p[i].title = "";
    inventory_p[i].text = "";
    inventory_p[i].defense = 0;
    inventory_p[i].armor_durability = 0;
  }
}
std::string itemTypeToString(itemType type) {
    switch (type) {
        case itemType::Weapon: return "Weapon";
        case itemType::Book:   return "Book";
        case itemType::Armor:  return "Armor";
        case itemType::Potion: return "Potion";
        default:               return "Unknown";
    }
}
void showInventory(const item* inventory_p, size_t count) {
    // Print table header
    std::cout << std::left
              << std::setw(5)  << "Index"
              << std::setw(15) << "Name"
              << std::setw(10) << "Type"
              << std::setw(7)  << "Level"
              << "\n";

    std::cout << std::string(37, '-') << "\n"; // separator

    // Print each item
    for (size_t i = 0; i < count; ++i) {
        const item& it = inventory_p[i];
        std::cout << std::left
                  << std::setw(5)  << i
                  << std::setw(15) << (it.name.empty() ? "<empty>" : it.name)
                  << std::setw(10) << itemTypeToString(it.type)
                  << std::setw(7)  << static_cast<int>(it.level)
                  << "\n";
    }
}
void setItem(item* inventory_p, size_t count, size_t index, const item& value) {
  if (index < count) {
    inventory_p[index] = value;
  }
}

int main() {
  constexpr size_t INV_SIZE = sizeof(inventory) / sizeof(inventory[0]);
  initializeInventory(inventory, INV_SIZE);
  showInventory(inventory, INV_SIZE);

  return 0;
}
