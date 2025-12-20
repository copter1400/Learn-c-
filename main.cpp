#include <iostream>
#include <cstdint>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>

enum class itemType {Weapon, Book, Armor, Potion};
enum class modeType {NoMode, Inventory, Attack, Exit};

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

item inventory[5];
bool is_Done;
std::string command;
modeType mode;

void sleep_ms(int milliseconds) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
void clearScreen() {
  std::cout << "\033[2J\033[1;1H";
}

void initializeInventory(item* inventory_p) {
  const size_t INV_SIZE = sizeof(inventory) / sizeof(inventory[0]);
  for (size_t i = 0; i < INV_SIZE; ++i) {
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
void showInventory(const item* inventory_p) {
    const int idxWidth = 10;
    const int nameWidth = 20;
    const int titleWidth = 25;
    const int typeWidth = 10;
    const int levelWidth = 7;

    int totalWidth = idxWidth + nameWidth + titleWidth + typeWidth + levelWidth;

    const size_t INV_SIZE = sizeof(inventory) / sizeof(inventory[0]);



    // Header
    std::cout << std::left
              << std::setw(idxWidth)   << "Index"
              << std::setw(nameWidth)  << "Name"
              << std::setw(titleWidth) << "Title"
              << std::setw(typeWidth)  << "Type"
              << std::setw(levelWidth) << "Level"
              << "\n";

    std::cout << std::string(totalWidth, '-') << "\n";

    // Items
    for (size_t i = 0; i < INV_SIZE; ++i) {
        const item& it = inventory_p[i];

        // truncate strings if too long
        auto displayName  = it.name.empty()  ? "<empty>" : it.name.substr(0, nameWidth-2);
        auto displayTitle = it.title.empty() ? "<empty>" : it.title.substr(0, titleWidth-2);

        std::cout << std::left
                  << std::setw(idxWidth)   << i
                  << std::setw(nameWidth)  << displayName
                  << std::setw(titleWidth) << displayTitle
                  << std::setw(typeWidth)  << itemTypeToString(it.type)
                  << std::setw(levelWidth) << static_cast<int>(it.level)
                  << "\n";
    }
}
void setItem(item* inventory_p, size_t index, const item& value) {
  const size_t INV_SIZE = sizeof(inventory) / sizeof(inventory[0]);
  if (index < INV_SIZE) {
    inventory_p[index] = value;
  }
}

int main() {
  //initialize
  clearScreen();
  
  initializeInventory(inventory);
  is_Done = false;
  command = "";
  mode = modeType::NoMode;
  
  while (!is_Done) {
    std::cout << "Command :";
    std::cin  >> command;

    if (command == "exit") { mode = modeType::Exit; is_Done = true; }
    else if (command == "inventory") { mode = modeType::Inventory; }
    else if (command == "attack") { mode = modeType::Attack; }
    else { mode = modeType::NoMode; }

    switch (mode) {
      case modeType::Inventory:
        clearScreen();
        showInventory(inventory);
        break;
      case modeType::Attack:
        clearScreen();
        std::cout << "Attack mode\n";
        break;
      case modeType::Exit:
        clearScreen();
        std::cout << "Exit\n";
        break;
      case modeType::NoMode:
        std::cout << "No command. Please select command.\n";
        break;
      default:
        clearScreen();
        std::cout << "Unknown command. Please try again.\n";
        break;
    }
    
  }
  

  return 0;
}