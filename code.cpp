#include<iostream>
#include<vector>
#include <cstddef>
#include <ctime>
#include<unordered_map>
#include <algorithm>
#include <random>
using namespace std;

class Player;
extern Player* GLOBAL_PLAYER;
Player* GLOBAL_PLAYER = nullptr;   // <<< REAL definition

class Item;
unordered_map<int, Item*> ID_MAP;

class Item 
{
public:
    int id;
    static int next_id;
    string name;
    string description;
    
    Item(string name = "null", string desc = "non")
        : name(name), description(desc)
    {
        id = next_id++;
        ID_MAP[id] = this;
    }

    virtual ~Item() // NEW: make destructor virtual (safer for inheritance)
    {
        ID_MAP.erase(id);
    }
};

int Item::next_id = 1; //global static id for every item created

class Healing : public Item
{
    public:
    int heal_value;
    Healing(string name, int val)
    : Item(name), heal_value(val) {}
};
class Food : public Item
{
    public:
    int sat_value;
    Food(string name, int val)
    : Item(name), sat_value(val) {}
};
class Drink : public Item
{
    public:
    int hyd_value;
    Drink(string name, int val)
    : Item(name), hyd_value(val) {}
};
class Cure : public Item
{
    public:
    int cure_value;
    Cure(string name, int val)
    : Item(name), cure_value(val) {}
};

class Weapon : public Item
{
    public:
    int damage;
    Weapon(int damage=0):damage(damage){}
    virtual ~Weapon() {} // NEW: virtual destructor for polymorphic delete
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

    Mag(int max_size=5)
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
    Revolver(string name="null_rev", int damage=0)
    {
        this->name=name;
        this->damage=damage;
        mag = new Mag<Light_rounds>(6); // fixed cylinder

        // NEW: load all 6 slots with rounds
        for(int i=0; i<mag->max; ++i)
        {
            mag->load(new Light_rounds(10)); // 10 is example damage_boost
        }
        // chamber starts empty; first shot will use mag->cursor logic in use()
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

class Hand_gun : public Gun<Light_rounds>
{
public:
    Hand_gun(string name="null_handgun", int damage=0, Mag<Light_rounds>* mag=nullptr)
    {
        this->name=name;
        this->damage=damage;

        if(mag) {
            this->mag = mag;
        } else {
            // NEW: create a default mag and fill it
            this->mag = new Mag<Light_rounds>(5); // pistol mag size (can change)
            for(int i=0; i<this->mag->max; ++i)
            {
                this->mag->load(new Light_rounds(10));
            }
        }

        // NEW: load first round into chamber
        if(this->mag && this->mag->cursor!=nullptr)
        {
            this->load_from_mag();
        }
    }

    // Can discard magazine freely
    void discard_mag() override {
        mag = nullptr;
    }
    // load_from_mag(), use() inherited from Gun works as usual
   
};


class Rifle : public Gun<Heavy_rounds>
{
public:
    Rifle(string name="null_rifle", int damage=0, Mag<Heavy_rounds>* mag=nullptr)
    {
        this->name=name;
        this->damage=damage;

        if(mag) {
            this->mag = mag;
        } else {
            // NEW: rifle mag size is 5
            this->mag = new Mag<Heavy_rounds>(5);
            for(int i=0; i<this->mag->max; ++i)
            {
                this->mag->load(new Heavy_rounds(15)); // example damage_boost
            }
        }

        // NEW: load first round into chamber
        if(this->mag && this->mag->cursor!=nullptr)
        {
            this->load_from_mag();
        }
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
    Shotgun(string name="null_shotgun", int damage=0) 
    {
        this->name=name;
        this->damage=damage;
        mag = new Mag<Shell>(4); // shotguns have unique magazines (or tubes)

        // NEW: fill the tube
        for(int i=0; i<mag->max; ++i)
        {
            mag->load(new Shell(20)); // example boost
        }

        // NEW: load shell to chamber from mag
        if(mag && mag->cursor!=nullptr)
        {
            this->load_from_mag();
        }
    }

    // Loading shell directly into chamber
    void load_to_chamber(Shell* shell) override {
        chamber = shell;
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

    DBarrel_Shotgun(string name="null_DBarrel", int damage=0) : chamber1(nullptr), chamber2(nullptr) 
    {
        this->name=name;
        this->damage=damage;

        // NEW: start with both barrels loaded
        chamber1 = new Shell(20);
        chamber2 = new Shell(20);
    }

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
    float saturationDrain; // dynamic drain amount
    float hydrationDrain;

    Player(string name="zero", Bag* b=nullptr, Weapon* main=nullptr, Weapon* off=nullptr, int position=1000, float velocity=4.0)
    :name(name), bag(b), main_hand(main), off_hand(off), position(position), velocity(velocity)
    {
        health = 100;
        saturation = 100;
        hydration = 100;
        illness = 1;

        saturationDrain = 10;  // default (rest)
        hydrationDrain = 10;
    }
    bool equip_weapon(Weapon* w)
    {
        cout << "You found a weapon: " << w->name << " (DMG: " << w->damage << ")\n";

        cout << "Your weapons:\n";
        cout << "1) Main hand: " << (main_hand ? main_hand->name : "None") << "\n";
        cout << "2) Off hand:  " << (off_hand ? off_hand->name : "None") << "\n";

        // If player has a free slot, auto fill
        if (!main_hand)
        {
            main_hand = w;
            cout << "Equipped to MAIN HAND.\n";
            return true;
        }
        if (!off_hand)
        {
            off_hand = w;
            cout << "Equipped to OFF HAND.\n";
            return true;
        }

        // Player already has two weapons — must choose
        cout << "\nYou already carry 2 weapons.\n";
        cout << "Replace one?\n";
        cout << "1) Replace MAIN HAND\n";
        cout << "2) Replace OFF HAND\n";
        cout << "3) Ignore weapon\n";
        cout << "Choice: ";

        int c;
        cin >> c;

        switch(c)
        {
            case 1:
                main_hand = w;
                cout << "Main hand replaced.\n";
                return true;
            case 2:
                off_hand = w;
                cout << "Off hand replaced.\n";
                return true;
            default:
                cout << "Weapon ignored.\n";
                return false;
        }
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
    float velocity = 4.017361; // m/s
    float rest_velocity = 0.00926;//  m/s
    Hand(float pos=0.0): position(pos) {}
};
bool hand_catch_check(Player& p, Hand& h)
{
    if (h.position >= p.position)
    {
        cout << "\n⚠️  GAME OVER — The Hand has caught you.\n";
        return true;
    }
    return false;
}

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
        cout << "Attacked with " << weapon->name << " for " << damage << " damage!\n";
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
        cout << "Attacked with " << weapon->name << " for " << damage << " damage!\n";
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

class LootPool {
public:
    std::vector<Weapon*> weapons;
    std::vector<Healing*> healingItems;   // restores HP
    std::vector<Cure*>    cureItems;      // reduces infection
    std::vector<Food*> foodItems;
    std::vector<Drink*> drinkItems;
    std::vector<Light_rounds*> lRounds;
    std::vector<Heavy_rounds*> hRounds;
    std::vector<Shell*> shellies;
    std::vector<Mag<Light_rounds>*> light_mags;
    std::vector<Mag<Heavy_rounds>*> heavy_mags;
    std::vector<Mag<Shell>*> shell_mags;

    void addHealing(Healing* item) { healingItems.push_back(item); }
    void addCure(Cure* item)       { cureItems.push_back(item); }

    void addFoodItem(Food* item)   { foodItems.push_back(item); }
    void addDrinkItem(Drink* item) { drinkItems.push_back(item); }

    void addWeapon(Weapon* item)   { weapons.push_back(item); }

    void add_light_round(Light_rounds* item) { lRounds.push_back(item); }
    void add_heavy_round(Heavy_rounds* item) { hRounds.push_back(item); }
    void add_shell(Shell* item)              { shellies.push_back(item); }

    void add_Lmagazine(Mag<Light_rounds>* item) { light_mags.push_back(item); }
    void add_Hmagazine(Mag<Heavy_rounds>* item) { heavy_mags.push_back(item); }
    void add_Smagazine(Mag<Shell>* item)        { shell_mags.push_back(item); }
};

bool player_has_weapon(Player* p, const string& name)
{
    // main hand
    if (p->main_hand && p->main_hand->name == name)
        return true;

    // off hand
    if (p->off_hand && p->off_hand->name == name)
        return true;

    // melee (rare but included)
    if (p->melee && p->melee->name == name)
        return true;

    // bag
    for (Item* it : p->bag->items)
    {
        if (Weapon* w = dynamic_cast<Weapon*>(it))
        {
            if (w->name == name)
                return true;
        }
    }

    return false;
}


class Location
{
public:
    string name;
    int tier;           // 1 to 6
    LootPool* LOOT_POOL;
    vector<Enemy*> enemies;

    Location(string n, int t)
        : name(n), tier(t)
    {
        LOOT_POOL = new LootPool();
        generate_enemies();
        generate_loot();
    }

    void generate_enemies()
    {
        int maxEnemies = 3;
        int count = rand() % (maxEnemies + 1); // 0–3

        for(int i=0; i<count; i++)
        {
            Enemy* e = new Enemy();
            e->name = "Infected";
            e->health = 50 + tier * 20;

            e->weapon = new Melee();
            e->weapon->name = "Claws";
            e->weapon->damage = 10 + tier * 2;

            enemies.push_back(e);   // ★ REQUIRED
        }

    }

    void generate_loot()
    {
        // FOOD & DRINK always appear
        int foodCount = rand() % (tier + 1);
        int drinkCount = rand() % (tier + 1);
        int cureChance = rand() % 100;

        for(int i=0;i<foodCount;i++)
            LOOT_POOL->addFoodItem(new Food("Ration", 15));

        for(int i=0;i<drinkCount;i++)
            LOOT_POOL->addDrinkItem(new Drink("Bottled Water", 20));

        // Healing (HP recovery):
        int healChance = rand() % 100;
        if (healChance < tier * 15)   // heal spawns ~slightly more often than cure
            LOOT_POOL->addHealing(new Healing("Medkit", 25));

        // Cure (infection recovery):
        //int cureChance = rand() % 100;
        if (cureChance < tier * 20)   // Cure now spawns frequently
            LOOT_POOL->addCure(new Cure("Vaccine", 30));



        // AMMO only tiers 3–6
        if(tier >= 3)
        {
            int ammoCount = rand() % (tier + 2);
            for(int i=0;i<ammoCount;i++)
            {
                LOOT_POOL->add_light_round(new Light_rounds(10));
                LOOT_POOL->add_heavy_round(new Heavy_rounds(15));
                LOOT_POOL->add_shell(new Shell(20));
            }

            // magazines sometimes appear
            if(rand() % 100 < tier * 12)
                LOOT_POOL->add_Lmagazine(new Mag<Light_rounds>(5));

            if(rand() % 100 < tier * 12)
                LOOT_POOL->add_Hmagazine(new Mag<Heavy_rounds>(5));

            if(rand() % 100 < tier * 12)
                LOOT_POOL->add_Smagazine(new Mag<Shell>(4));
        }

        if(rand()%100 < tier*5)
        {
            Weapon* w = (tier <= 3)
                        ? random_tier1_weapon()
                        : random_tier2_weapon();

            // only spawn if player does NOT already have it
            if (!player_has_weapon(GLOBAL_PLAYER, w->name))
                LOOT_POOL->addWeapon(w);
        }

    }

private:

    Weapon* random_tier1_weapon()
    {
        vector<Weapon*> w = {
            new Revolver("Clot Pothyn", 40),
            new Hand_gun("Clog 17", 25),
            new Rifle("Car 98C", 70),
            new Shotgun("Jury", 50),
            new DBarrel_Shotgun("Shawn", 70)
        };
        return w[rand() % w.size()];
    }

    Weapon* random_tier2_weapon()
    {
        vector<Weapon*> w = {
            new Revolver("Hand Cannon", 80),
            new Hand_gun("1191", 35),
            new Rifle("Outlaw", 140),
            new Shotgun("Judgement", 135),
            new DBarrel_Shotgun("Buckle-up", 200)
        };
        return w[rand() % w.size()];
    }
};
void update_survival(Player& p)
{
    float infectionFactor = p.illness / 100.0f;  // e.g. 20% infection = 0.20

    // Drain stats depending on infection
    p.saturation -= p.saturationDrain * (1 + infectionFactor);
    p.hydration -= p.hydrationDrain * (1 + infectionFactor);

    if (p.saturation < 0) p.saturation = 0;
    if (p.hydration < 0) p.hydration = 0;

    // Infection rises +10 per hour
    p.illness += 10;
    if (p.illness > 100) p.illness = 100;
}

int count_items(Bag* bag, string typeName)
{
    int count = 0;
    for (Item* i : bag->items)
    {
        if (typeid(*i).name() == typeName) count++;
    }
    return count;
}
bool consume_item(Player& p, string typeName)
{
    for (int i=0; i < p.bag->items.size(); i++)
    {
        Item* it = p.bag->items[i];

        if (typeid(*it).name() == typeName)
        {
            // apply effect
            if (Food* f = dynamic_cast<Food*>(it))
            {
                p.saturation += f->sat_value;
                if (p.saturation > 100) p.saturation = 100;
            }
            else if (Drink* d = dynamic_cast<Drink*>(it))
            {
                p.hydration += d->hyd_value;
                if (p.hydration > 100) p.hydration = 100;
            }
            else if (Healing* h = dynamic_cast<Healing*>(it))
            {
                p.health += h->heal_value;
                if (p.health > 100) p.health = 100;
            }
            else if (Cure* c = dynamic_cast<Cure*>(it))
            {
                p.illness -= c->cure_value;
                if (p.illness < 0) p.illness = 0;
            }

            p.bag->items.erase(p.bag->items.begin() + i);
            return true;
        }
    }

    cout << "No item available.\n";
    return false;
}
void load_single_weapon(Weapon* w, Player& p);
void load_weapons_menu(Player& p);

void rest_menu(Player& p)
{
    while (true)
    {
        cout << "\n==========================\n";
        cout << "      REST & BAG MENU\n";
        cout << "==========================\n\n";

        cout << "1) Eat Food (x" << count_items(p.bag, typeid(Food).name()) << ")\n";
        cout << "2) Drink Water (x" << count_items(p.bag, typeid(Drink).name()) << ")\n";
        cout << "3) Heal (x" << count_items(p.bag, typeid(Healing).name()) << ")\n";
        cout << "4) Cure Infection (x" << count_items(p.bag, typeid(Cure).name()) << ")\n\n";

        cout << "5) Load Weapons\n";
        cout << "6) Finish (end hour)\n";
        cout << "Choice: ";

        int c; cin >> c;

        switch(c)
        {
            case 1: consume_item(p, typeid(Food).name()); break;
            case 2: consume_item(p, typeid(Drink).name()); break;
            case 3: consume_item(p, typeid(Healing).name()); break;
            case 4: consume_item(p, typeid(Cure).name()); break;

            case 5:
                load_weapons_menu(p);
                break;

            case 6:
                return; // return to main loop

            default:
                cout << "Invalid.\n";
        }
    }
}
void load_weapons_menu(Player& p)
{
    while (true)
    {
        cout << "\n==========================\n";
        cout << "     LOAD WEAPONS MENU\n";
        cout << "==========================\n\n";

        cout << "Weapon 1: " << (p.main_hand ? p.main_hand->name : string("None")) << "\n";
        cout << "Weapon 2: " << (p.off_hand ? p.off_hand->name : string("None")) << "\n\n";

        cout << "Ammo in Bag:\n";
        cout << " Light Rounds: " << count_items(p.bag, typeid(Light_rounds).name()) << "\n";
        cout << " Heavy Rounds: " << count_items(p.bag, typeid(Heavy_rounds).name()) << "\n";
        cout << " Shells:       " << count_items(p.bag, typeid(Shell).name()) << "\n\n";

        cout << "1) Load Weapon 1\n";
        cout << "2) Load Weapon 2\n";
        cout << "9) Back\n";
        cout << "0) Finish\n";

        cout << "Choice: ";
        int c; cin >> c;

        if (c == 9) return;
        if (c == 0) return;

        if (c == 1 && p.main_hand)
            load_single_weapon(p.main_hand, p);
        else if (c == 2 && p.off_hand)
            load_single_weapon(p.off_hand, p);
        else
            cout << "Invalid or weapon missing.\n";
    }
}
void load_single_weapon(Weapon* w, Player& p)
{
    cout << "Loading: " << w->name << "\n";

    // LIGHT GUN
    if (Gun<Light_rounds>* g = dynamic_cast<Gun<Light_rounds>*>(w))
    {
        cout << "Add 1 light round? (1=yes,0=no): ";
        int x; cin >> x;
        if (x == 1 && consume_item(p, typeid(Light_rounds).name()))
            g->load_to_chamber(new Light_rounds(10));
    }

    // HEAVY GUN
    else if (Gun<Heavy_rounds>* g = dynamic_cast<Gun<Heavy_rounds>*>(w))
    {
        cout << "Add 1 heavy round? (1=yes,0=no): ";
        int x; cin >> x;
        if (x == 1 && consume_item(p, typeid(Heavy_rounds).name()))
            g->load_to_chamber(new Heavy_rounds(15));
    }

    // SHOTGUN
    else if (Gun<Shell>* g = dynamic_cast<Gun<Shell>*>(w))
    {
        cout << "Add 1 shell? (1=yes,0=no): ";
        int x; cin >> x;
        if (x == 1 && consume_item(p, typeid(Shell).name()))
            g->load_to_chamber(new Shell(20));
    }

    // DOUBLE BARREL
    else if (DBarrel_Shotgun* d = dynamic_cast<DBarrel_Shotgun*>(w))
    {
        cout << "Load shell? (1=yes,0=no): ";
        int x; cin >> x;
        if (x == 1 && consume_item(p, typeid(Shell).name()))
            d->load_shell(new Shell(20));
    }
}


int main()
{
    srand(time(NULL));

    // ---------------------------------------------
    // LootPool initialization
    // ---------------------------------------------
    LootPool pool1;

    // Example mag & ammo fill
    pool1.add_Lmagazine(new Mag<Light_rounds>(5));
    for(int i=0; i<pool1.light_mags[0]->max; i++)
    {
        pool1.add_light_round(new Light_rounds(10));
        pool1.light_mags[0]->load(pool1.lRounds[i]);
    }

    // ---------------------------------------------
    // ALL weapons defined here (Tier 1 + Tier 2)
    // ---------------------------------------------
    // Tier 1
    pool1.addWeapon(new Revolver("Clot Pothyn", 40));
    pool1.addWeapon(new Hand_gun("Clog 17", 25));
    pool1.addWeapon(new Rifle("Car 98C", 70));
    pool1.addWeapon(new Shotgun("Jury", 50));
    pool1.addWeapon(new DBarrel_Shotgun("Shawn", 70));

    // Tier 2
    pool1.addWeapon(new Revolver("Hand Cannon", 80));
    pool1.addWeapon(new Hand_gun("1191", 35));
    pool1.addWeapon(new Rifle("Outlaw", 140));
    pool1.addWeapon(new Shotgun("Judgement", 135));
    pool1.addWeapon(new DBarrel_Shotgun("Buckle-up", 200));


    Player p1;
    Bag pBag;
    p1.bag = &pBag;

    GLOBAL_PLAYER = &p1;   // MUST COME FIRST
    // ---------------------------------------------
    // Create ALL 6 Locations
    // ---------------------------------------------
    vector<Location*> ALL_LOCATIONS = {
        new Location("Abandoned Civilian House", 1),
        new Location("Picnic Camp", 2),
        new Location("Gas Station", 3),
        new Location("Hospital", 4),
        new Location("Abandoned Troop Camp", 5),
        new Location("Top Secret Facility", 6)
    };

    // ---------------------------------------------
    // Player Setup
    // ---------------------------------------------
    

    // Player starts with only a melee weapon: Kitchen Knife
    p1.main_hand = nullptr;
    p1.off_hand  = nullptr;
    p1.melee = new Melee();
    p1.melee->name = "Kitchen Knife";
    p1.melee->damage = 25;

    // global pointer assignment
    GLOBAL_PLAYER = &p1;

    Hand h1; // The Hand (monster)

    // ---------------------------------------------
    // Start 24-hour game loop
    // ---------------------------------------------
    for(int pass = 1; pass <= 24; pass++)
    {
        cout << "Hour: " << pass << "#\n";
        cout << "The Hand is " << p1.position - h1.position << "m(s) away\n";
        cout << "Health: " << p1.health << "\n"
             << "Food: " << p1.saturation << "\n"
             << "Water: " << p1.hydration << "\n"
             << "Infection: " << p1.illness << "%\n";

        cout << "----------------------------\n"
                "|           ACTIONS         |\n"
                "|   1. Find a place to loot |\n"
                "|   2. Rest and open bag    |\n"
                "-----------------------------\n\n"
                "choice: ";

        int choice;
        cin >> choice;
        cout << "\n";

        // starvation / dehydration damage
        if (p1.saturation <= 0 || p1.hydration <= 0)
        {
            p1.health -= 15;
            if (p1.health <= 0)
            {
                cout << "You died from starvation/dehydration.\n";
                return 0;
            }
        }

        switch(choice)
        {
            // ---------------------------------------------
            // 1. LOOT A LOCATION
            // ---------------------------------------------
            case 1:
            {
                // Player + hand movement
                p1.position += (p1.velocity * 3600);
                h1.position += (h1.velocity * 3600);

                // GAME OVER IF HAND PASSED PLAYER
                if (h1.position >= p1.position)
                {
                    cout << "GAME OVER — The Hand caught you.\n";
                    return 0;
                }

                // stat drain
                p1.saturationDrain = 20;
                p1.hydrationDrain = 20;
                update_survival(p1);

                // ============================================
                // RANDOM LOCATION SELECTION (3 OPTIONS)
                // ============================================
                vector<int> unlocked;

                unlocked.push_back(1);
                unlocked.push_back(2);
                unlocked.push_back(3);
                unlocked.push_back(4);

                if (pass > 8)  unlocked.push_back(5);
                if (pass > 16) unlocked.push_back(6);

                std::shuffle(unlocked.begin(), unlocked.end(), std::mt19937(std::random_device{}()));

                vector<int> shown;
                for (int i = 0; i < 3 && i < unlocked.size(); i++)
                    shown.push_back(unlocked[i]);

                cout << "----------------------------\n";
                cout << "     Choose a location:\n";
                cout << "----------------------------\n";

                for (int i = 0; i < shown.size(); i++)
                {
                    cout << (i+1) << ") "
                         << ALL_LOCATIONS[shown[i] - 1]->name << "\n";
                }

                cout << "----------------------------\n\n";
                cout << "Choice: ";

                int pick;
                cin >> pick;

                if (pick < 1 || pick > shown.size())
                {
                    cout << "Invalid.\n";
                    break;
                }

                Location* chosen = ALL_LOCATIONS[shown[pick - 1] - 1];
                cout << "You arrive at: " << chosen->name << "\n\n";

                // ---------------------------------------------
                // ENEMIES (Melee only)
                // ---------------------------------------------
                if (!chosen->enemies.empty())
                {
                    cout << chosen->enemies.size() << " enemies appear.\n";

                    for (Enemy* enemy : chosen->enemies)
                    {
                        enemy->weapon = nullptr;      // melee only
                        enemy->health = 40;           // weaker enemies

                        Combat fight(&p1, enemy);
                        fight.start();

                        if (p1.health <= 0)
                        {
                            cout << "GAME OVER — You died.\n";
                            return 0;
                        }
                    }
                }
                else cout << "Area is empty.\n";

                // ---------------------------------------------
                // LOOT COLLECTION
                // ---------------------------------------------
                cout << "\nSearching the area...\n";
                for (Weapon* w : chosen->LOOT_POOL->weapons)
                    p1.equip_weapon(w);

                for (auto i : chosen->LOOT_POOL->healingItems)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->cureItems)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->foodItems)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->drinkItems)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->lRounds)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->hRounds)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->shellies)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->light_mags)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->heavy_mags)
                    p1.bag->add_to_bag(i);

                for (auto i : chosen->LOOT_POOL->shell_mags)
                    p1.bag->add_to_bag(i);


                cout << "Loot collected.\n\n";

                break;
            }

            // ---------------------------------------------
            // 2. REST & OPEN BAG
            // ---------------------------------------------
            case 2:
            {
                h1.position += (h1.rest_velocity * 3600);

                p1.saturationDrain = 10;
                p1.hydrationDrain  = 10;

                rest_menu(p1);
                update_survival(p1);

                // Hand catch-up check
                if (h1.position >= p1.position)
                {
                    cout << "GAME OVER — The Hand caught you.\n";
                    return 0;
                }

                break;
            }

            default:
                cout << "Invalid choice.\n";
        }
    }

    cout << "You survived all 24 hours!\n";
    return 0;
}
