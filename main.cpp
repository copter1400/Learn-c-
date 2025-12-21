#include <iostream>
#include <cstdint>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <vector>

enum class itemType {Weapon, Book, Armor, Potion, NoneType};
enum class modeType {NoMode, Code, Inventory, Navigation, Help, DebugMode, Attack, Exit};

struct Item {
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
struct Tile {
    char symbol;
    bool walkable;
};
struct Map {
    uint8_t player_starter_x, player_starter_y;
    uint8_t player_x, player_y;
    uint8_t width, height;
    const uint8_t* data;
};
struct Position {
  uint8_t x, y;
};
struct Player {
  Position position;
};
struct Delta {
    int dx;
    int dy;
};


constexpr size_t INV_SIZE = 5;
bool Debug_mode = 0;
Item inventory[INV_SIZE];
bool is_Done = false;
std::string command = "";
std::string line = "";
char nav_command = ' ';
std::string code = "";
modeType mode = modeType::NoMode;
Player player = {0, 0};
Position old_position = {0, 0};
bool is_nav_done = false;
Delta dir;
Position next;

std::vector<Tile> tiles = {
    {' ', false},
    {'#', true},
    {'X', false},
    {'*', false}
};
//Map data
uint8_t map_A_data[] = {
  2, 2, 2, 2, 2, 2, 2,
  2, 0, 0, 0, 1, 1, 2,
  2, 0, 1, 1, 1, 1, 2,
  2, 1, 1, 1, 1, 0, 2,
  2, 1, 1, 1, 1, 0, 2,
  2, 0, 0, 0, 0, 0, 2,
  2, 2, 2, 2, 2, 2, 2
};

Map map_A = {
  1, 3,      // player start position
  0, 0,      // player actual position
  7, 7,      // Width, Height
  map_A_data
};

//global function
void sleep_ms(int milliseconds_p) {
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_p));
}
void clearScreen() {
  std::cout << "\033[2J\033[1;1H";
}
Position operator+(const Position& p, const Delta& d) {
    return {
        static_cast<uint8_t>(p.x + d.dx),
        static_cast<uint8_t>(p.y + d.dy)
    };
}
uint64_t hash(const std::string& s) {
    uint64_t h = 1469598103934665603ULL; // offset
    for (int i=0;i<10000;i++) {
      for (char c : s) {
        h ^= (uint64_t)c;
        h *= 1099511628211ULL; // prime
      }
    }
    return h;
}

//inventory system
void initializeInventory(Item* inventory_p, size_t INV_SIZE) {
  for (size_t i = 0; i < INV_SIZE; ++i) {
    inventory_p[i] = Item{}; // value-initialize
    inventory_p[i].name = "";
    inventory_p[i].type = itemType::NoneType;
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
        case itemType::Weapon:   return "Weapon";
        case itemType::Book:     return "Book";
        case itemType::Armor:    return "Armor";
        case itemType::Potion:   return "Potion";
        case itemType::NoneType: return "NoneType";
        default:                 return "Unknown";
    }
}
void showInventory(const Item* inventory_p, size_t INV_SIZE) {
    const int idxWidth = 10;
    const int nameWidth = 20;
    const int titleWidth = 25;
    const int typeWidth = 10;
    const int levelWidth = 7;

    int totalWidth = idxWidth + nameWidth + titleWidth + typeWidth + levelWidth;

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
        const Item& it = inventory_p[i];

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
void setItem(Item* inventory_p, size_t index, const Item& value, size_t INV_SIZE) {
  if (index < INV_SIZE) {
    inventory_p[index] = value;
  }
}

//Navigation
void showMap(Map& map_p, std::vector<Tile>& tiles_p, Player& player_p) {
  uint8_t width  = map_p.width;
  uint8_t height = map_p.height;

  for (size_t i = 0;i < height;i++) {
    for (size_t j = 0;j < width;j++) {
      if (player_p.position.x == j && player_p.position.y == i) {
        std::cout << "@" << " ";
      } else {
        std::cout << tiles_p[ map_p.data[ (i * width) + j] ].symbol << " ";
      }
    }
    std::cout << "\n";
  }
}
void movePlayer(Map& map_p, std::vector<Tile>& tile_p, Player& player_p, Position& old_position_p, Position& new_position_p) {
  uint8_t width  = map_p.width;
  uint8_t height = map_p.height;

  // bounds check
  if (new_position_p.x >= width || new_position_p.y >= height) {
      return;
  }

  // get tile id
  uint8_t tile_id = map_p.data[new_position_p.y * width + new_position_p.x];

  // walkable check
  if (!tile_p[tile_id].walkable) {
      return;
  }

  // save old position
  old_position_p.x = player_p.position.x;
  old_position_p.y = player_p.position.y;

  // move player
  player_p.position = new_position_p;

  // sync map metadata
  map_p.player_x = new_position_p.x;
  map_p.player_y = new_position_p.y;
}

int main() {
  //initialize
  initializeInventory(inventory, INV_SIZE);
  player.position = {map_A.player_starter_x, map_A.player_starter_y};

  clearScreen();
  
  while (!is_Done) {
    std::cout << "Command ['h' or 'help' for help info]:";
    std::cin  >> command;

    if (command == "exit") { mode = modeType::Exit; is_Done = true; }
    else if (command == "help" || command == "h") { mode = modeType::Help; }
    else if (command == "inventory" || command == "inv") { mode = modeType::Inventory; }
    else if (command == "navigation" || command == "nav") { mode = modeType::Navigation; }
    else if (command == "attack" || command == "atk") { mode = modeType::Attack; }
    else if (command == "code") { mode = modeType::Code; }
    else { mode = modeType::NoMode; }

    switch (mode) {
      case modeType::Exit:
        clearScreen();
        std::cout << "Exit, Come back soon!\n";
        break;
      case modeType::Help:
        clearScreen();
        std::cout << "Help mode\n"
                 << "help       | h       : showing all commands\n"
                 << "exit       |         : exit the program\n"
                 << "inventory  | inv     : showing inventory\n"
                 << "navigation | nav     : navigation mode\n"
                 << "attack     | atk     : attack mode\n"
                 << "code       |         : enter code\n";
        break;
      case modeType::Inventory:
        clearScreen();
        showInventory(inventory, INV_SIZE);
        break;
      case modeType::Navigation:
        is_nav_done = false;
        line = "";
        while (!is_nav_done) {
          clearScreen();
          showMap(map_A, tiles, player);
          
          std::cout << "Move (w,a,s,d) [q to exit] :";
          std::getline(std::cin, line);

          if (line.empty()) { continue; }
          
          nav_command = line[0];

          dir = {0, 0};

          switch (nav_command) {
            case 'w': dir = { 0, -1 }; break;
            case 's': dir = { 0,  1 }; break;
            case 'a': dir = { -1, 0 }; break;
            case 'd': dir = { 1,  0 }; break;
            case 'q': is_nav_done=true; break;
          }

          if (!is_nav_done) {
            next = player.position + dir;
            movePlayer(map_A, tiles, player, old_position, next);
          }
        }
        break;
      case modeType::Attack:
        clearScreen();
        std::cout << "Attack mode\n";
        break;
      case modeType::Code:
        clearScreen();
        std::cout << "Enter code :";
        std::cin >> code;

        if (hash(code) == 3222955091911998211ULL) {
          Debug_mode = !Debug_mode;
          if (Debug_mode) {
            std::cout << "Debug mode activated.\n";
          } else {
            std::cout << "Debug mode deactivated.\n";
          }
        }
        break;
      case modeType::NoMode:
        clearScreen();
        std::cout << "No command. Please select command, type 'help' or 'h' for command info\n";
        break;
      default:
        clearScreen();
        std::cout << "An error command has occur.\n";
        break;
    }
    
  }
  

  return 0;
}