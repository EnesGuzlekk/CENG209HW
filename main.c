#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_ITEMS 5
#define MAX_ROOMS 7
#define MAX_INVENTORY 5
#define MAX_MONSTERS 5

typedef struct ItemNode {
    char *name;
    struct ItemNode *next;
} ItemNode;

typedef struct MonsterNode {
    char *name;
    int health;
    struct MonsterNode *next;
} MonsterNode;

typedef struct RoomNode {
    char *description;
    ItemNode *items;
    MonsterNode *monster;

    struct RoomNode *north;
    struct RoomNode *south;
    struct RoomNode *east;
    struct RoomNode *west;
    struct RoomNode *previous;
} RoomNode;

typedef struct PlayerInventoryNode {
    char *item;
    struct PlayerInventoryNode *next;
} PlayerInventoryNode;

typedef struct {
    char *name;
    int health;
    int strength;
    PlayerInventoryNode *inventory;
    int inventory_count;
} Player;

RoomNode* create_room(const char *description);
ItemNode* create_item_node(const char *name);
MonsterNode* create_monster_node(const char *name);
void add_item_to_room(RoomNode *room, const char *item);
void add_monster_to_room(RoomNode *room, const char *monster);
void connect_rooms(RoomNode *room1, RoomNode *room2, const char *direction);
void initialize_player(Player *player);
void free_room_network(RoomNode *start_room);
void free_player(Player *player);
void print_map(char grid[MAX_ROOMS][MAX_ROOMS], RoomNode *current);

void move_player(Player *player, RoomNode **current_room, const char *direction);
void look(RoomNode *current_room, Player *player,char grid[MAX_ROOMS][MAX_ROOMS]);
void pickup_item(Player *player, RoomNode *current_room, const char *item);
void show_inventory(Player *player);
void attack(Player *player, RoomNode *current_room);
void save_game(Player *player, RoomNode *current_room, const char *filepath);
void load_game(Player *player, RoomNode *rooms[], RoomNode **current_room, const char *filepath);
void list_saved_games();
void help_command();

char *random_item();
char *random_monster();
const char *directions[] = {"north", "south", "east", "west"};

const char* get_random_direction() {
    return directions[rand() % 4];
}

static int startX = MAX_ROOMS / 2;
static int startY = MAX_ROOMS / 2;

static void place_room(int x, int y, char grid[MAX_ROOMS][MAX_ROOMS]) {
    if (x >= 0 && x < MAX_ROOMS && y >= 0 && y < MAX_ROOMS) {
        grid[x][y] = 'P';
    }
}

int main() {
    srand(time(NULL));

    Player player;
    initialize_player(&player);

    RoomNode *start_room = create_room("1.Room : Stark");
    RoomNode *room2 = create_room("2.Room : Baratheon");
    RoomNode *room3 = create_room("3.Room : Greyjoy");
    RoomNode *room4 = create_room("4.Room : Tully");
    RoomNode *room5 = create_room("5.Room : Arryn");
    RoomNode *room6 = create_room("6.Room : Tyrell");
    RoomNode *room7 = create_room("7.Room : Lannister");

    RoomNode *rooms[] = {start_room, room2, room3, room4, room5, room6, room7};
    int length = sizeof(rooms) / sizeof(rooms[0]);
    for (int i = 0; i < length; i++) {
        // Random items
        for (int j = 0; j < 3; j++) {
            char *item = random_item();
            add_item_to_room(rooms[i], item);
            free(item);
        }

        char *monster = random_monster();
        add_monster_to_room(rooms[i], monster);
        free(monster);
    }

    for (int i = 0; i < length - 1; i++) {
        while (1) {
            const char *direction = get_random_direction();
            if ((strcmp(direction, "north") == 0 && rooms[i]->north == NULL) ||
                (strcmp(direction, "south") == 0 && rooms[i]->south == NULL) ||
                (strcmp(direction, "east") == 0 && rooms[i]->east == NULL) ||
                (strcmp(direction, "west") == 0 && rooms[i]->west == NULL)) {
                connect_rooms(rooms[i], rooms[i + 1], direction);
                break;
            }

        }
    }

    char grid[MAX_ROOMS][MAX_ROOMS];
    for (int i = 0; i < MAX_ROOMS; i++) {
        for (int j = 0; j < MAX_ROOMS; j++) {
            grid[i][j] = '.';
        }
    }

    RoomNode *current_room = start_room;

    printf("Welcome to Node-Based Dungeon Adventure!\n");
    help_command(); // Show initial help

    char command[256];
    while (1) {
        printf("dungeon> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) break;

        char *cmd = strtok(command, " ");
        char *arg = strtok(NULL, " ");

        if (strcmp(cmd, "move") == 0 && arg) {
            move_player(&player, &current_room, arg);
        } else if (strcmp(cmd, "look") == 0) {
            look(current_room, &player, grid);
        } else if (strcmp(cmd, "pickup") == 0 && arg) {
            pickup_item(&player, current_room, arg);
        } else if (strcmp(cmd, "inventory") == 0) {
            show_inventory(&player);
        } else if (strcmp(cmd, "attack") == 0) {
            attack(&player, current_room);
        } else if (strcmp(cmd, "help") == 0) {
            help_command();
        } else if (strcmp(cmd, "save") == 0 && arg) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s.sav", arg);
            save_game(&player, current_room, filepath);
        } else if (strcmp(cmd, "load") == 0 && arg) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s.sav", arg);
            load_game(&player, rooms, &current_room, filepath);
        } else if (strcmp(cmd, "list") == 0 && arg && strcmp(arg, "save") == 0) {
            system("ls *.sav 2>/dev/null || echo No saved games found.");
        } else {
            printf("Unknown command. Type 'help' for a list of commands.\n");
        }
    }
    free_room_network(start_room);
    free_player(&player);
    return 0;
}
void help_command() {
    printf("\n===== Game Commands =====\n");
    printf("move <direction>   - Move in a direction (north, south, east, west)\n");
    printf("look               - Examine current room\n");
    printf("pickup <item>      - Pick up an item in the room\n");
    printf("inventory          - Show your current inventory\n");
    printf("attack             - Fight the monster in the current room\n");
    printf("save <name>        - Save game\n");
    printf("load <name>        - Load game\n");
    printf("list save          - List save game\n");
    printf("help               - Show this help menu\n");
    printf("exit               - Exit the game\n");
    printf("========================\n\n");
}

RoomNode* create_room(const char *description) {
    RoomNode *room = malloc(sizeof(RoomNode));
    room->description = strdup(description);
    room->items = NULL;
    room->monster = NULL;
    room->north = room->south = room->east = room->west = room->previous = NULL;
    return room;
}

ItemNode* create_item_node(const char *name) {
    ItemNode *node = malloc(sizeof(ItemNode));
    node->name = strdup(name);
    node->next = NULL;
    return node;
}

MonsterNode* create_monster_node(const char *name) {
    MonsterNode *node = malloc(sizeof(MonsterNode));
    node->name = strdup(name);
    node->health = 100;
    node->next = NULL;
    return node;
}

void add_item_to_room(RoomNode *room, const char *item) {
    ItemNode *new_item = create_item_node(item);
    new_item->next = room->items;
    room->items = new_item;
}

void add_monster_to_room(RoomNode *room, const char *monster) {
    if (!room->monster) {
        room->monster = create_monster_node(monster);
    }
}

void connect_rooms(RoomNode *room1, RoomNode *room2, const char *direction) {
    if (strcmp(direction, "north") == 0) {
        room1->north = room2;
        room2->south = room1;
        room2->previous = room1;
    } else if (strcmp(direction, "south") == 0) {
        room1->south = room2;
        room2->north = room1;
        room2->previous = room1;
    } else if (strcmp(direction, "east") == 0) {
        room1->east = room2;
        room2->west = room1;
        room2->previous = room1;
    } else if (strcmp(direction, "west") == 0) {
        room1->west = room2;
        room2->east = room1;
        room2->previous = room1;
    }
}

void initialize_player(Player *player) {
    player->name = strdup("Hero");
    player->health = 100;
    player->strength = 20;
    player->inventory = NULL;
    player->inventory_count = 0;
}

void free_room_network(RoomNode *start_room) {
    RoomNode *rooms_to_free[7] = {0};
    int room_count = 0;

    RoomNode *current = start_room;
    while (current) {
        int found = 0;
        for (int i = 0; i < room_count; i++) {
            if (rooms_to_free[i] == current) {
                found = 1;
                break;
            }
        }
        if (!found && room_count < 7) {
            rooms_to_free[room_count++] = current;
        }

        current = current->south ? current->south :
                  current->north ? current->north :
                  current->east ? current->east :
                  current->west;
    }

    for (int i = 0; i < room_count; i++) {
        RoomNode *room = rooms_to_free[i];

        ItemNode *item = room->items;
        while (item) {
            ItemNode *next_item = item->next;
            free(item->name);
            free(item);
            item = next_item;
        }

        if (room->monster) {
            free(room->monster->name);
            free(room->monster);
        }
        free(room->description);
        free(room);
    }
}

void free_player(Player *player) {
    free(player->name);

    PlayerInventoryNode *current = player->inventory;
    while (current) {
        PlayerInventoryNode *next = current->next;
        free(current->item);
        free(current);
        current = next;
    }
}

void move_player(Player *player, RoomNode **current_room, const char *direction) {
    (void)player; // kullanilmayan parametreye referans
    bool arr[4] = {false};
    (void)arr; // kullanilmayan degiskene referans

    RoomNode *next_room = NULL;
    if (strcmp(direction, "north") == 0) next_room = (*current_room)->north;
    else if (strcmp(direction, "south") == 0) next_room = (*current_room)->south;
    else if (strcmp(direction, "east") == 0) next_room = (*current_room)->east;
    else if (strcmp(direction, "west") == 0) next_room = (*current_room)->west;

    if (next_room) {
        *current_room = next_room;
        if (strcmp(direction, "north") == 0) startX -= 1;
        else if (strcmp(direction, "south") == 0) startX += 1;
        else if (strcmp(direction, "east") == 0) startY += 1;
        else if (strcmp(direction, "west") == 0) startY -= 1;
        printf("You moved %s to %s.\n", direction, (*current_room)->description);
    } else {
        printf("You can't move %s from here.\n", direction);
    }
}

void look(RoomNode *current_room, Player *player,char grid[MAX_ROOMS][MAX_ROOMS]) {
    (void)player; // kullanilmayan parametreye referans
    printf("====================================\n");
    printf("You are in: %s\n", current_room->description);

    if (current_room->monster) {
        printf("A wild %s is here! (Health: %d)\n",
               current_room->monster->name,
               current_room->monster->health);
    }
    printf("Items in this room:\n");
    ItemNode *item = current_room->items;
    if (!item) {
        printf("No items in this room.\n");
    } else {
        while (item) {
            printf("- %s\n", item->name);
            item = item->next;
        }
    }
    print_map(grid,current_room);

    printf("====================================\n");
}

void pickup_item(Player *player, RoomNode *current_room, const char *item) {
    ItemNode **current = &(current_room->items);
    while (*current) {
        if (strcmp((*current)->name, item) == 0) {
            PlayerInventoryNode *inv_node = malloc(sizeof(PlayerInventoryNode));
            inv_node->item = strdup(item);
            inv_node->next = player->inventory;
            player->inventory = inv_node;
            if (strcmp(item, "Potion") == 0) {
                player->health = 100;
                printf("Your health has been fully restored to 100!\n");
            }

            ItemNode *to_remove = *current;
            *current = (*current)->next;
            free(to_remove->name);
            free(to_remove);

            player->inventory_count++;
            printf("You picked up: %s\n", item);
            return;
        }
        current = &((*current)->next);
    }

    printf("Item '%s' not found in this room.\n", item);
}

void show_inventory(Player *player) {
    if (player->inventory == NULL) {
        printf("Your inventory is empty.\n");
        return;
    }

    printf("Your inventory:\n");
    PlayerInventoryNode *current = player->inventory;
    while (current) {
        printf("- %s\n", current->item);
        current = current->next;
    }
}

void attack(Player *player, RoomNode *current_room) {
    if (current_room->monster != NULL) {
        int player_damage = rand() % 15 + 5;
        printf("You attack the %s and deal %d damage!\n", current_room->monster->name, player_damage);
        current_room->monster->health -= player_damage;
        if (current_room->monster->health <= 0) {
            printf("You defeated the %s!\n", current_room->monster->name);
            free(current_room->monster->name);
            free(current_room->monster);
            current_room->monster = NULL;
        } else {
            printf("The %s has %d health left.\n", current_room->monster->name, current_room->monster->health);
            int monster_damage = rand() % 15 + 5;
            player->health -= monster_damage;
            printf("The %s counterattacks and deals %d damage!\n", current_room->monster->name, monster_damage);

            if (player->health <= 0) {
                printf("You have been defeated by the %s...\n", current_room->monster->name);
                printf("Game over.\n");
                exit(0);
            } else {
                printf("Your health is now %d.\n", player->health);
            }
        }
    } else {
        printf("There's no monster to fight in this room.\n");
    }
}

void print_map(char grid[MAX_ROOMS][MAX_ROOMS], RoomNode *current) {
    printf("\nPossible Movements\n");
    if (current->north) {
        printf("  North: %s\n", current->north->description);
    } else {
        printf("  North: No room\n");
    }

    if (current->south) {
        printf("  South: %s\n", current->south->description);
    } else {
        printf("  South: No room\n");
    }

    if (current->east) {
        printf("  East: %s\n", current->east->description);
    } else {
        printf("  East: No room\n");
    }

    if (current->west) {
        printf("  West: %s\n", current->west->description);
    } else {
        printf("  West: No room\n");
    }
    grid[startX][startY] = 'P';

    if (current->north) {
        place_room(startX - 1, startY, grid);
    }
    if (current->south) {
        place_room(startX + 1, startY, grid);
    }
    if (current->east) {
        place_room(startX, startY + 1, grid);
    }
    if (current->west) {
        place_room(startX, startY - 1, grid);
    }

    printf("\nRoom Map (size %dx%d)\n", MAX_ROOMS, MAX_ROOMS);
    for (int i = 0; i < MAX_ROOMS; i++) {
        for (int j = 0; j < MAX_ROOMS; j++) {
            printf(" %c ", grid[i][j]);
        }
        printf("\n");
    }

}


void save_game(Player *player, RoomNode *current_room, const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (!file) {
        printf("Error: Cannot create save file '%s'.\n", filepath);
        return;
    }

    fprintf(file, "PLAYER\n");
    fprintf(file, "Name:%s\n", player->name);
    fprintf(file, "Health:%d\n", player->health);
    fprintf(file, "Strength:%d\n", player->strength);

    fprintf(file, "ROOM\n");
    fprintf(file, "Description:%s\n", current_room->description);

    fprintf(file, "INVENTORY\n");
    fprintf(file, "Count:%d\n", player->inventory_count);
    PlayerInventoryNode *current = player->inventory;
    while (current) {
        fprintf(file, "Item:%s\n", current->item);
        current = current->next;
    }

    fclose(file);
    printf("Game saved successfully to '%s'.\n", filepath);
}

void load_game(Player *player, RoomNode *rooms[], RoomNode **current_room, const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("Error: Cannot open save file '%s'.\n", filepath);
        return;
    }

    char buffer[256];
    char section[32];
    char key[32];
    char value[256];

    PlayerInventoryNode *cur_inv = player->inventory;
    while (cur_inv) {
        PlayerInventoryNode *next = cur_inv->next;
        free(cur_inv->item);
        free(cur_inv);
        cur_inv = next;
    }
    player->inventory = NULL;
    player->inventory_count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) continue;

        if (buffer[0] != ' ' && !strchr(buffer, ':')) {
            strncpy(section, buffer, sizeof(section)-1);
            continue;
        }

        char *separator = strchr(buffer, ':');
        if (!separator) continue;

        strncpy(key, buffer, separator - buffer);
        key[separator - buffer] = '\0';
        strcpy(value, separator + 1);

        if (strcmp(section, "PLAYER") == 0) {
            if (strcmp(key, "Name") == 0) {
                free(player->name);
                player->name = strdup(value);
            } else if (strcmp(key, "Health") == 0) {
                player->health = atoi(value);
            } else if (strcmp(key, "Strength") == 0) {
                player->strength = atoi(value);
            }
        } else if (strcmp(section, "ROOM") == 0) {
            if (strcmp(key, "Description") == 0) {
                for (int i = 0; i < MAX_ROOMS; i++) {
                    if (rooms[i] && strcmp(rooms[i]->description, value) == 0) {
                        *current_room = rooms[i];
                        break;
                    }
                }
            }
        } else if (strcmp(section, "INVENTORY") == 0) {
            if (strcmp(key, "Item") == 0) {
                PlayerInventoryNode *new_item = malloc(sizeof(PlayerInventoryNode));
                new_item->item = strdup(value);
                new_item->next = player->inventory;
                player->inventory = new_item;
                player->inventory_count++;
            }
        }
    }
    fclose(file);
    printf("Game loaded successfully from '%s'.\n", filepath);
}

void list_saved_games() {
    printf("Saved games:\n");
    printf("- game1.txt\n");
    printf("- game2.txt\n");
}

char *random_item() {
    const char *items[] = {"Axe","Sword", "Shield", "Potion", "Gold Coin"};
    int index = rand() % 5;  // Randomly select an index from 0 to 4
    return strdup(items[index]);
}

char *random_monster() {
    const char *monsters[] = {"Goblin", "Orc", "Dragon", "Troll", "Vampire", "Zombie"};
    int index = rand() % 5;  // Randomly select an index from 0 to 4
    return strdup(monsters[index]);
}

