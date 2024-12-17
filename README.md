# CENG209HW
CENG209 homework repository - Dungeon Adventure Game 
A. Enes Güzlek - 22050161002

# Adventure Game in C

This repository contains a text-based adventure game written in C. The game features rooms, monsters, and items, all interconnected to create an interactive and immersive gameplay experience.

## Features
- **Dynamic Room Management**: Rooms are dynamically linked with items and monsters, allowing for flexible game design and exploration.
- **Item Inventory**: Players can pick up, drop, and store items in their inventory, which has a limited capacity.
- **Monster Encounters**: Each room may contain a monster with health stats, adding combat challenges to the game.
- **Game Exploration**: Players can move between rooms, interact with items, and face various challenges in a non-linear fashion.
- **Dynamic Gameplay**: The game dynamically generates rooms, items, and monsters, ensuring replayability.

## Code Structure
The program is structured using linked lists to manage rooms, items, and monsters efficiently. Below are the key components:

### Data Structures
- **ItemNode**: Represents items in the game.
    ```c
    typedef struct ItemNode {
        char *name;
        struct ItemNode *next;
    } ItemNode;
    ```
    - **name**: The name of the item (e.g., "Sword", "Shield").
    - **next**: Pointer to the next item in the list.

- **MonsterNode**: Represents monsters with health stats and a name.
    ```c
    typedef struct MonsterNode {
        char *name;
        int health;
        struct MonsterNode *next;
    } MonsterNode;
    ```
    - **name**: The name of the monster (e.g., "Goblin", "Orc").
    - **health**: The health points of the monster.
    - **next**: Pointer to the next monster in the list.

- **RoomNode**: Represents rooms, each containing a description, items, and monsters.
    ```c
    typedef struct RoomNode {
        char *description;
        ItemNode *items;
        MonsterNode *monster;
        struct RoomNode *next;
    } RoomNode;
    ```
    - **description**: A brief description of the room.
    - **items**: Pointer to the list of items in the room.
    - **monster**: Pointer to the list of monsters in the room.
    - **next**: Pointer to the next room in the game.

### Functions
The following functions manage the core gameplay:

1. **createRoom**: Dynamically allocates and initializes a new room.
    ```c
    RoomNode* createRoom(char *description);
    ```
2. **addItem**: Adds an item to a room's item list.
    ```c
    void addItem(RoomNode *room, char *itemName);
    ```
3. **addMonster**: Adds a monster to a room's monster list.
    ```c
    void addMonster(RoomNode *room, char *monsterName, int health);
    ```
4. **displayRoom**: Displays the details of the current room, including items and monsters.
    ```c
    void displayRoom(RoomNode *room);
    ```
5. **moveToNextRoom**: Moves the player to the next room.
    ```c
    RoomNode* moveToNextRoom(RoomNode *currentRoom);
    ```

### Game Loop
The main game loop allows the player to:
1. Explore rooms.
2. Pick up items.
3. Encounter and interact with monsters.
4. Move to the next room.

## Compilation
To compile the code, use the following command:
```bash
gcc -o adventure_game main.c
```

## Execution
Run the compiled program:
```bash
./adventure_game
```

## Requirements
- A C compiler (e.g., GCC)
- Compatible with Linux, macOS, and Windows (with MinGW or similar).
- Terminal or command-line access.

## Gameplay Instructions
1. **Start the Game**: The program will begin in the starting room.
2. **Explore Rooms**: Read the room descriptions, check for items, and look for monsters.
3. **Interact with Items**:
    - Pick up items and add them to your inventory.
    - Drop items if your inventory is full.
4. **Encounter Monsters**:
    - Monsters will appear in rooms. Players can choose to fight or flee.
    - Monsters have health points that can be reduced during combat.
5. **Move Between Rooms**: Progress through rooms to explore the game world.

### Example Gameplay
```
You are in the starting room.
Description: A dark and dusty room with cobwebs in the corners.
Items: Sword, Shield
Monsters: Goblin (Health: 30)

Options:
1. Pick up Sword
2. Pick up Shield
3. Attack Goblin
4. Move to the next room
5. Quit

Enter your choice: 3
You attack the Goblin! Goblin's health is now 20.
```

## Example Commands
- **Pick up an item**: Adds the item to your inventory.
- **Attack a monster**: Reduces the monster's health.
- **Move to the next room**: Advances the game to the next room.
- **Quit**: Exits the game.


**Happy Adventuring!**
