#include<iostream>
#include<vector>
#include <cstddef>
#include <ctime>
using namespace std;


class Item 
{
    public:
    int id;
    string name;
    string description;
    float weight; //in grams
};
class Healing : public Item
{
    public:
    int heal_value;
    Healing(string name, int val): heal_value(val)
    {
        this->name = name;
    }
};

class Weapon : public Item
{
    public:
    int damage;
};

class Light_rounds : public Item
{
    public:
    int damage_boost;
    Light_rounds* prev;
    Light_rounds* next;
    Light_rounds()
    {
        damage_boost = 0;
        this->prev = nullptr;
        this->next = nullptr;
    }
    Light_rounds(int boost): damage_boost(boost), prev(nullptr), next(nullptr) {}
    ~Light_rounds()
    {
        
    }
};
class Heavy_rounds : public Item //it's gonna be a node in a stack data structure
{
    public:
    int damage_boost;
    Heavy_rounds* prev;
    Heavy_rounds* next;
    Heavy_rounds()
    {
        damage_boost = 0;
        this->prev = nullptr;
        this->next = nullptr;
    }
    Heavy_rounds(int boost): damage_boost(boost), prev(nullptr), next(nullptr) {}
    ~Heavy_rounds()
    {
        
    }
};
class Shell : public Item
{
    public:
    int damage_boost;
    Shell* prev;
    Shell* next;
    Shell()
    {
        damage_boost = 0;
        this->prev = nullptr;
        this->next = nullptr;
    }
    Shell(int boost): damage_boost(boost), prev(nullptr), next(nullptr) {}
    ~Shell()
    {
        
    }
};
class Magazine : public Item
{
    public:
    int max;
};

////////////////////////////////////////////////

template <typename T>
class Mag : public Magazine
{
public:
    int current_count;
    T* cursor; // top of the stack

    Mag(int max_size = 5)
    {
        cursor = nullptr;
        current_count = 0;
        max = max_size;
    }

    void load(T* round)
    {
        if (current_count >= max)
        {
            std::cout << "mag full\n";
            return;
        }

        round->prev = nullptr;
        round->next = cursor;

        if (cursor)
            cursor->prev = round;

        cursor = round;
        current_count++;
    }

    T* unload() // remove top round
    {
        if (!cursor) return nullptr;

        T* top = cursor;
        cursor = cursor->next;

        if (cursor)
            cursor->prev = nullptr;

        top->next = nullptr;
        top->prev = nullptr;

        current_count--;
        return top;
    }

    ~Mag()
    {
        T* tmp;
        while (cursor)
        {
            tmp = cursor->next;
            delete cursor;
            cursor = tmp;
        }
    }
};

////////////////////////////////////////////////

template<typename T>
class Gun : public Weapon
{
    public:
    T* chamber;
    Mag<T>* mag;
    Gun(): chamber(nullptr), mag(nullptr){}
    virtual void load_to_chamber(T* round)
    {
        chamber = round;
    }
    virtual void load_from_mag()
    {
        if(mag && mag->cursor!=nullptr)
        {
            chamber = mag->unload();
        }
        else {cout<<"mag empty\n";}

    }
    void discard_round()
    {
        chamber = nullptr;
    }
    virtual void discard_mag()
    {
        mag = nullptr;
    }
    virtual void load_mag(Mag<T>* magazine)
    {
        mag = magazine;
    }
    virtual T* use()
    {
        T* out = chamber;
        if(chamber!=nullptr)
        {
            chamber = nullptr;
        }else{
            cout<<"chamber empty\n";
            return nullptr;
        }
        if( mag && mag->cursor!=nullptr)
        {
            chamber = mag->unload();
        }

        return out;
    }
};
class Revolver : public Gun<Light_rounds>
{
public:
    Revolver() 
    {
        mag = new Mag<Light_rounds>(6); // fixed cylinder
    }

    void discard_mag() override {
        std::cout << "Cannot remove revolver cylinder!\n";
    }

    Light_rounds* use() override {
        if (!chamber && mag && mag->cursor) {
            // load current round
            chamber = mag->cursor;
        }

        if (!chamber) {
            std::cout << "Chamber empty!\n";
            return nullptr;
        }
        Light_rounds* out = chamber;
        chamber = nullptr;

        // move cursor to next round in the cylinder
        if (mag && mag->cursor) 
        {
            if (mag->cursor->next) 
            {
                mag->cursor = mag->cursor->next;  // advance to next round
            } else 
            {
                // wrap to first round in the linked list
                Light_rounds* tmp = mag->cursor;
                while (tmp->prev) tmp = tmp->prev;
                mag->cursor = tmp;
            }
        }
        return out;
    }
};


class Rifle : public Gun<Heavy_rounds>
{
public:
    Rifle() {
        mag = nullptr; // magazine is detachable
    }

    // Can discard magazine freely
    void discard_mag() override {
        mag = nullptr;
    }
    // load_from_mag(), use() inherited from Gun works as usual
};

class Shotgun : public Gun<Shell>
{
public:
    Shotgun() {
        mag = nullptr; // shotguns have no magazine
    }

    // Loading shell directly into chamber
    void load_to_chamber(Shell* shell) override {
        chamber = shell;
    }

    void load_from_mag() override {
        std::cout << "Shotgun has no magazine!\n";
    }

    void discard_mag() override {
        std::cout << "Shotgun has no magazine!\n";
    }
};

class DBarrel_Shotgun : public Weapon
{
public:
    Shell* chamber1;
    Shell* chamber2;

    DBarrel_Shotgun() : chamber1(nullptr), chamber2(nullptr) {}

    // Load into first empty chamber
    void load_shell(Shell* shell) {
        if (!chamber1) {
            chamber1 = shell;
        } else if (!chamber2) {
            chamber2 = shell;
        } else {
            std::cout << "Both barrels loaded!\n";
        }
    }

    // Fire a specific chamber (1 or 2)
    Shell* use(int barrel = 1) {
        if (barrel == 1) {
            if (!chamber1) {
                std::cout << "Chamber 1 empty!\n";
                return nullptr;
            }
            Shell* out = chamber1;
            chamber1 = nullptr;
            return out;
        } else if (barrel == 2) {
            if (!chamber2) {
                std::cout << "Chamber 2 empty!\n";
                return nullptr;
            }
            Shell* out = chamber2;
            chamber2 = nullptr;
            return out;
        } else {
            std::cout << "Invalid barrel number!\n";
            return nullptr;
        }
    }

    void discard_shells() {
        chamber1 = nullptr;
        chamber2 = nullptr;
    }
};

class Melee : public Weapon
{
    public:
};
class Bag
{   
    public:
    vector<Item*> items;
    void add_to_bag(Item* thing)
    {
        items.push_back(thing);
    }
};

class Player
{
    public:
    string name;
    float health;
    float saturation;
    float hydration;
    float illness;
    float position; //in meters
    float velocity;
    Weapon* main_hand;
    Weapon* off_hand;
    Weapon* melee;
    Bag* bag;

    Player()
    {   name = "zero";
        health = 100;
        saturation = 100;
        hydration = 100;
        illness = 1;
        main_hand = nullptr;
        off_hand = nullptr;
        melee = nullptr;
        bag = nullptr;
        position = 0;
        velocity = 4.0; //  m/s
    }
    Player(string name, Bag* b, Weapon* main , Weapon* off, int position):name(name), bag(b), main_hand(main), off_hand(off), position(position){
        health = 100;
        saturation = 100;
        hydration = 100;
        illness = 1;
    }
    
};
class Enemy
{   public:
    string name;
    int health;
    Weapon* weapon;
};
class Hand
{
    public:
    float position; //also in meters
    float velocity = 4.0 + 4.0 * ((0.023148148 - 0.01) / 4.0) + 0.01;
};

class Combat {
public:
    Player* player;
    Enemy* enemy;

    Combat(Player* p, Enemy* e) : player(p), enemy(e) {}

    void start() {
        cout << "Combat begins: " << player->name << " vs " << enemy->name << "\n";

        bool player_turn = true;

        while (player->health > 0 && enemy->health > 0) {
            if (player_turn) {
                player_action();
            } else {
                enemy_action();
            }
            player_turn = !player_turn;
        }

        if (player->health <= 0) {
            cout << player->name << " has been defeated!\n";
        } else {
            cout << enemy->name << " has been defeated!\n";
        }
    }

private:
    void player_action() {
        cout << "\nYour turn! Choose an action:\n";
        cout << "1) Attack with main hand\n";
        cout << "2) Attack with off hand\n";
        cout << "3) Attack with melee\n";
        cout << "4) Use item from bag\n";
        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            attack(player->main_hand, enemy);
            break;
        case 2:
            attack(player->off_hand, enemy);
            break;
        case 3:
            attack(player->melee, enemy);
            break;
        case 4:
            use_item();
            break;
        default:
            cout << "Invalid choice. Skipping turn.\n";
        }
    }

    void enemy_action() {
        cout << "\nEnemy's turn: " << enemy->name << " attacks!\n";
        attack(enemy->weapon, player);
    }

    void attack(Weapon* weapon, Player* target) {
        if (!weapon) {
            cout << "No weapon equipped! Attack does 1 damage.\n";
            target->health -= 1;
            return;
        }

        int damage = weapon->damage;  // You can later add bullet/mag damage boost
        cout << "Attacked with " << typeid(*weapon).name() << " for " << damage << " damage!\n";
        target->health -= damage;
        if (target->health < 0) target->health = 0;

        cout << target->name << " HP: " << target->health << "\n";
    }

    void attack(Weapon* weapon, Enemy* target) {
        if (!weapon) {
            cout << enemy->name << " has no weapon! Attack does 1 damage.\n";
            target->health -= 1;
            return;
        }

        int damage = weapon->damage; // can extend with rounds later
        cout << enemy->name << " attacks with " << typeid(*weapon).name() << " for " << damage << " damage!\n";
        target->health -= damage;
        if (target->health < 0) target->health = 0;

        cout << target->name << " HP: " << target->health << "\n";
    }

    void use_item() {
        if (!player->bag || player->bag->items.empty()) {
            cout << "Bag is empty!\n";
            return;
        }

        cout << "Items in bag:\n";
        for (size_t i = 0; i < player->bag->items.size(); ++i) {
            cout << i << ": " << typeid(*player->bag->items[i]).name() << "\n";
        }

        int choice;
        cin >> choice;
        if (choice < 0 || choice >= player->bag->items.size()) {
            cout << "Invalid item!\n";
            return;
        }

        Item* item = player->bag->items[choice];
        // For now just heal 10 HP if it's a consumable, can extend later
        player->health += 10;
        if (player->health > 100) player->health = 100;
        cout << "Used item. HP restored to " << player->health << "\n";

        // remove item from bag
        player->bag->items.erase(player->bag->items.begin() + choice);
    }
};





int main()
{
    for(int hour=1;hour<=24;hour++)
    {

    }
    return 0;
}
