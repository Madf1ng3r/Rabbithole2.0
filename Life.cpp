#include <sstream>
#include <Windows.h>
#include <cstdlib> 
#include <ctime>    
#include <fstream>
#include "Mainframe.h"
using namespace std;
// Spielerposition auf der Karte
struct Position {
    int x;
    int y;
};
struct PlayerColor {
    int red;
    int green;
    int blue;
};
void setCursorPosition(int x, int y);  // Deklaration

void drawPlayer(const Position& pos, char symbol, WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;

    SetConsoleTextAttribute(hConsole, color);
    setCursorPosition(pos.x, pos.y);  // Cursor richtig setzen
    cout << symbol;
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

// ASCII Art für die Spielflächen     BKCPDSRWFT
const string homeMap = R"(
+------------------------------------------------------------------------------------------------------------+
|                                                                                                            |   
|                                                                                                            |   
|   BK                                    C P D S R W F                                                     T|   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |
|                                                                                                            |    
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |   
|                                                                                                            |
|                                                                                                            |
|                                                                                                            |   
|                                                                                                            |    
|                                                                                                            |    
|                                                                                                            |   
|                                                                                                            |   
+------------------------------------------------------------------------------------------------------------+
)";

const string balconyMap = R"(
+--------------+
|              |
|     BB       |  
|              |
|              |
|              |
+--------------+
)";

const string hallwayMap = R"(
+--------------+
|  T   HT  KT   |  
|   H    K      |
|              |
|   B          |
|              |
+--------------+
)";

const string kitchenMap = R"(
+--------------+
|  KT          |   
|    K         |
|              |
+--------------+
)";

const string bathroomMap = R"(
+--------------+
|    BT        |   
|    B         |
|              |
+--------------+
)";

const string schoolMap = R"(
+------------------+
|   M    L         |   
|     C            |  
|  R  S  B         |
|                  | 
+------------------+
)";

const string streetMap = R"(
+------------------------+
|   N  L                 |   
|    A    P              |
|                        |
|                        |
| K                      |   
+------------------------+
)";
// Klasse für interaktive Objekte
class InteractiveObject {
public:
    InteractiveObject(const string& name, char symbol, const Position& position)
        : name(name), symbol(symbol), position(position) {}
    const string& getName() const { return name; }
    char getSymbol() const { return symbol; }
    const Position& getPosition() const { return position; } 
    Position& getPosition() { return position; } 
private:
    string name;   // Name des Objekts
    char symbol;        // Symbol, das das Objekt auf der Karte repräsentiert
    Position position;  // Position des Objekts auf der Karte
};
// Klasse für Spielfläche
class GameMap {
public:
    GameMap(const string& map)
        : map(map), width(28), height(16) {}
    void display() const {
        cout << map << endl;
    }
    void addObject(const InteractiveObject& object) {
        objects.push_back(object);
    }
    const string& getMap() const {
        return map;
    }
    const vector<InteractiveObject>& getObjects() const {
        return objects;
    }
    InteractiveObject* getObjectAtPosition(const Position& position) {
        for (auto& object : objects) {
            if (object.getPosition().x == position.x && object.getPosition().y == position.y) {
                return &object;
            }
        }
        return nullptr;
    }
    void updateMovingObjects() {
        for (auto& object : objects) {
            if (isMovingObject(object)) {
                moveObject(object);
            }
        }
    }
    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }
private:
    string map;                            // Spielfläche
    int width;                             // Breite der Spielfläche
    int height;                            // Höhe der Spielfläche
    vector<InteractiveObject> objects;     // Vektor der interaktiven Objekte auf der Spielfläche
    bool isMovingObject(const InteractiveObject& object) const {
        // Überprüfen, ob das Objekt ein bewegliches Objekt ist (Nachbarn, andere Leute, Mitschüler)
        return object.getName() == "Nachbarn" || object.getName() == "Andere Leute" || object.getName() == "Mitschüler";
    }
    void moveObject(InteractiveObject& object) {
        // Zufällige Bewegung in eine der vier Richtungen (oben, unten, links, rechts)
        int direction = rand() % 4;
        Position newPosition = object.getPosition();
        switch (direction) {
        case 0:
            if (newPosition.y > 0) {
                newPosition.y--;
            }
            break;
        case 1:
            if (newPosition.y < height - 1) {
                newPosition.y++;
            }
            break;
        case 2:
            if (newPosition.x > 0) {
                newPosition.x--;
            }
            break;
        case 3:
            if (newPosition.x < width - 1) {
                newPosition.x++;
            }
            break;
        }
        // Aktualisieren der Position des Objekts
        object.getPosition() = newPosition;
    }
};
void printColored(char c, WORD color); // Deklaration VOR der Klasse Inventory
// Inventar-Klasse
class Inventory {
public:
    Inventory() : slots(10, "") {}
    bool addItem(const string& item) {
        for (auto& slot : slots) {
            if (slot.empty()) {
                slot = item;
                return true;
            }
        }
        return false; // Inventar voll
    }
    void display() const {
        cout << "\nInventar:          \n";
        for (int i = 0; i < 10; ++i) {
            cout << "[" << (slots[i].empty() ? "  " : slots[i]) << "]";
            if (i % 5 == 4) cout << "\n";
        }
    }
    void displayHighlighted(int selectedIndex) const {
        cout << "\nInventar:\n";
        for (int i = 0; i < 10; ++i) {
            if (i == selectedIndex) {
                printColored('[', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                if (slots[i].empty())
                    printColored(' ', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                else
                    cout << slots[i];
                printColored(']', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
            } else {
                cout << "[" << (slots[i].empty() ? "  " : slots[i]) << "]";
            }
            if (i % 5 == 4) cout << "\n";
        }
    }
    string getItem(int index) const {
        if (index >= 0 && index < static_cast<int>(slots.size())) return slots[index];
        return "";
    }
private:
    vector<string> slots;
};

static int getLineStart(const string& map, int y);
int getLineLength(const string& map) {
    size_t pos = map.find('\n');
    if (pos != string::npos)
        return static_cast<int>(pos + 1); // Zeilenlänge inkl. '\n'
    return static_cast<int>(map.length());
}

bool isBlocked(const GameMap& map, const Position& pos) {
    string m = map.getMap();
    if (pos.x < 0 || pos.y < 0) return true;
    size_t index = static_cast<size_t>(getLineStart(m, pos.y)) + static_cast<size_t>(pos.x);
    if (index >= m.size()) return true;
    char c = m[index];
    return c == '|' || c == '_' || c == '-' || c == '+';
}

static int getLineStart(const string& map, int y) {
    int line = 0;
    size_t idx = 0;
    while (line < y && idx < map.size()) {
        if (map[idx] == '\n') line++;
        idx++;
    }
    return static_cast<int>(idx);
}
// Function to show the current map and interactive objects
bool saveGame(const Position& playerPosition, const Inventory& inventory, const PlayerColor& playerColor);
void printColored(char c, WORD color) { // Deklaration
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, color);
    cout << c;
    SetConsoleTextAttribute(hConsole, saved_attributes);
}
void showMap(const GameMap& map, const Position& playerPosition) {
    clearScreen();
    string currentMap = map.getMap();

    for (const auto& object : map.getObjects()) {
        int index = getLineStart(currentMap, object.getPosition().y) + object.getPosition().x;
        currentMap[index] = object.getSymbol();
    }
    int playerIndex = getLineStart(currentMap, playerPosition.y) + playerPosition.x;
    currentMap[playerIndex] = 'P';

    for (size_t i = 0; i < currentMap.size(); ++i) {
        if (currentMap[i] == 'P' && i == playerIndex) {
            printColored('P', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        } else {
            cout << currentMap[i];
        }
    }
    cout << endl;
}
// Funktion zum Steuern des Spielercharakters
void setCursorPosition(int x, int y) {
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     COORD pos = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
     SetConsoleCursorPosition(hConsole, pos);
     }
void controlPlayer(Position& playerPosition, GameMap& currentMap, Inventory& inventory, bool isMorning, PlayerColor& playerColor){
    char input;
    Position oldPosition = playerPosition;
    bool inventoryMode = false;
    int selectedIndex = 0;
    cout << "| Steuerung: (W = Hoch, S = Runter, A = Links, D = Rechts, E = Interagieren, Q = Inventar [1] Speichern | [2] Optionen | [3] Beenden ) |\n";
    cout << "  ___________________________________________________________________________________________________________\n";
    this_thread::sleep_for(chrono::milliseconds(400));
    drawPlayer(playerPosition, 'P', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    do {
        input = _getch();
        if (inventoryMode) {
            switch (input) {
                case 'w': if (selectedIndex >= 5) selectedIndex -= 5; break;
                case 's': if (selectedIndex < 5) selectedIndex += 5; break;
                case 'a': if (selectedIndex > 0) selectedIndex -= 1; break;
                case 'd': if (selectedIndex < 9) selectedIndex += 1; break;
                case 'q': inventoryMode = false; break;
            }
            setCursorPosition(0, currentMap.getHeight() + 4);
            inventory.displayHighlighted(selectedIndex);
            cout << "  ___________________________________________________________________________________________________________\n";
            cout << "\n|    |\n";
            cout << "  ___________________________________________________________________________________________________________\n";
            continue;
        }
        Position newPos = playerPosition;
        switch (input) {
            case 'w': newPos.y = playerPosition.y - 1; break;
            case 's': newPos.y = playerPosition.y + 1; break;
            case 'a': newPos.x = playerPosition.x - 1; break;
            case 'd': newPos.x = playerPosition.x + 1; break;
            case 'q': inventoryMode = true; selectedIndex = 0; break;
            case '1': saveGame(playerPosition, inventory, playerColor); break;
        }
        if (!isBlocked(currentMap, newPos) && !inventoryMode) {
            drawPlayer(playerPosition, ' ', 0);
            playerPosition = newPos;
            drawPlayer(playerPosition, 'P', FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        }
        if (input == 'e' && !inventoryMode) {
            InteractiveObject* object = currentMap.getObjectAtPosition(playerPosition);
            if (object) {
                setCursorPosition(0, currentMap.getHeight() + 2);
                cout << "Interaktion mit " << object->getName() << endl;
                inventory.addItem(object->getName());
            }
        }
        setCursorPosition(0, currentMap.getHeight() + 4);
        inventory.display();
        cout << "  ___________________________________________________________________________________________________________\n";
        cout << "\n|\n";
        cout << "  ___________________________________________________________________________________________________________\n";
    } while (input != '3');
}
// Funktion zum Speichern des Spielstands
bool saveGame(const Position& playerPosition, const Inventory& inventory, const PlayerColor& playerColor) {
    setCursorPosition(0, 25);
    cout << "Spielstand speichern? (1 = Ja, 0 = Nein): ";
    char confirm = _getch();
    if (confirm == '1') {
        std::ofstream file("savegame.txt");
        if (!file) {
            setCursorPosition(0, 25);
            cout << "Fehler beim Speichern!";
            this_thread::sleep_for(chrono::seconds(2));
            setCursorPosition(0, 25);
            cout << string(80, ' ');
            return false;
        }
        file << playerPosition.x << " " << playerPosition.y << "\n";
        file << playerColor.red << " " << playerColor.green << " " << playerColor.blue << "\n";
        for (int i = 0; i < 10; ++i) file << inventory.getItem(i) << "\n";
        file.close();
        setCursorPosition(0, 25);
        cout << "Spielstand gespeichert!";
        this_thread::sleep_for(chrono::seconds(2));
        setCursorPosition(0, 25);
        cout << string(80, ' ');
        return true;
    }
    else {
        setCursorPosition(0, 25);
        cout << "Speichern abgebrochen.";
        this_thread::sleep_for(chrono::seconds(2));
        setCursorPosition(0, 25);
        cout << string(80, ' ');
        return false;
    }
}
bool loadGame(Position& playerPosition, Inventory& inventory, PlayerColor& playerColor) {
    ifstream file("savegame.txt");
    if (!file) return false;
    file >> playerPosition.x >> playerPosition.y;
    file >> playerColor.red >> playerColor.green >> playerColor.blue;
    file.ignore();
    for (int i = 0; i < 10; ++i) {
        string item;
        getline(file, item);
        if (!item.empty()) inventory.addItem(item);
    }
    file.close();
    return true;
}
int lifemain() {
    GameMap currentMap(homeMap);
    Position playerPosition;
    Inventory inventory;
    bool isMorning = true;
    // Versuche Spielstand zu laden, sonst in der Mitte spawnen
    PlayerColor playerColor;
    if (!loadGame(playerPosition, inventory, playerColor)) {
        playerPosition = {
            currentMap.getWidth() / 2,
            currentMap.getHeight() / 2
        };
    }
    controlPlayer(playerPosition, currentMap, inventory, isMorning, playerColor);
    return 0;
}