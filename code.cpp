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
    float damage;
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

// template <typename T>
// class MagStack : public Magazine
// {
// public:
//     int current_count;
//     T* cursor; // top of the stack

//     MagStack(int max_size = 5)
//     {
//         cursor = nullptr;
//         current_count = 0;
//         max = max_size;
//     }

//     void load(T* round)
//     {
//         if (current_count >= max)
//         {
//             std::cout << "mag full\n";
//             return;
//         }

//         round->prev = nullptr;
//         round->next = cursor;

//         if (cursor)
//             cursor->prev = round;

//         cursor = round;
//         current_count++;
//     }

//     T* unload() // remove top round
//     {
//         if (!cursor) return nullptr;

//         T* top = cursor;
//         cursor = cursor->next;

//         if (cursor)
//             cursor->prev = nullptr;

//         top->next = nullptr;
//         top->prev = nullptr;

//         current_count--;
//         return top;
//     }

//     ~MagStack()
//     {
//         T* tmp;
//         while (cursor)
//         {
//             tmp = cursor->next;
//             delete cursor;
//             cursor = tmp;
//         }
//     }
// };



////////////////////////////////////////////////
//Learn this
class Rifle_mag : public Magazine //the stack
{
   int current_rounds;
   Heavy_rounds* cursor; //to point at the wanted round 
   Rifle_mag()
   {
    cursor = nullptr;
    max = 5;
    current_rounds = 0;
   } 
   void load(Heavy_rounds* round)
   {
    if(current_rounds==0)
    {
        round->next = nullptr;
        cursor = round;
        current_rounds++;
    }else if(current_rounds>0 && current_rounds<max)
    {
        round->prev = nullptr;
        round->next = cursor;
        cursor->prev = round;
        cursor = round;
        current_rounds++;
    }else {cout<<"mag full\n";}
   }
   ~Rifle_mag()
    {
        // Clean up all nodes in the stack
        Heavy_rounds* tmp;
        while (cursor)
        {
            tmp = cursor->next;
            delete cursor;
            cursor = tmp;
        }
    }
};


class Shotgun_mag : public Magazine
{   
    public:
   int current_shells;
   Shell* cursor; //to point at the wanted shell 
   Shotgun_mag()
   {
    cursor = nullptr;
    max = 5;
    current_shells = 0;
   } 
   void load(Shell* shell)
   {
    if(current_shells==0)
    {
        shell->next = nullptr;
        cursor = shell;
        current_shells++;
    }else if(current_shells>0 && current_shells<max)
    {
        shell->prev = nullptr;
        shell->next = cursor;
        cursor->prev = shell;
        cursor = shell;
        current_shells++;
    }else {cout<<"mag full\n";}
   }
   ~Shotgun_mag()
    {
        // Clean up all nodes in the stack
        Shell* tmp;
        while (cursor)
        {
            tmp = cursor->next;
            delete cursor;
            cursor = tmp;
        }
    }
    
};
class Handgun_mag : public Magazine
{
   int current_rounds;
   Light_rounds* cursor; //to point at the wanted round 
   Handgun_mag()
   {
    cursor = nullptr;
    max = 5;
    current_rounds = 0;
   } 
   void load(Light_rounds* round)
   {
    if(current_rounds==0)
    {
        round->next = nullptr;
        cursor = round;
        current_rounds++;
    }else if(current_rounds>0 && current_rounds<max)
    {
        round->prev = nullptr;
        round->next = cursor;
        cursor->prev = round;
        cursor = round;
        current_rounds++;
    }else {cout<<"mag full\n";}
   }
   ~Handgun_mag()
    {
        // Clean up all nodes in the stack
        Light_rounds* tmp;
        while (cursor)
        {
            tmp = cursor->next;
            delete cursor;
            cursor = tmp;
        }
    }
};
class Gun : public Weapon
{
    public:

};
class Revolver : public Gun
{
    public:
};
class Rifle : public Gun
{
    public:
};
class Shotgun : public Gun
{
    public:
};
class DBarrel_Shotgun : public Gun
{
    public:
};
class Melee : public Weapon
{
    int reach;
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


class Combat
{
    Player* p;
    Enemy* e;
    Combat()
    {
        p = new Player();
        e = new Enemy();
    }
    Combat(Player* p, Enemy* e): p(p), e(e){}
    ~Combat() 
    {
        delete p;
        delete e;
    }
    void playerAttacks(Weapon* w)
    {
        if(w != nullptr)
        {
            e->health-=w->damage;
            cout<<p->name<<" dealt "<<w->damage<<" to "<<e->name<<"\n";
        }else{
            cout<<"No weapon! "<<p->name<<" can't attack\n";
        }
    }
    void playerAttacks(Revolver* w)
    {
        if(w != nullptr)
        {
            e->health-=w->damage;
            cout<<p->name<<" dealt "<<w->damage<<" to "<<e->name<<"\n";

        }else{
            cout<<"No weapon! "<<p->name<<" can't attack\n";
        }
    }
    void enemyAttacks(Melee* w)
    {
        p->health-=w->damage;
        cout<<e->name<<" dealt "<<w->damage<<" to "<<p->name<<"\n";
    }
    void playerHeals(Healing* h)
    {
        if(h != nullptr)
        {p->health+=h->heal_value;}
        else{cout<<"No healing items\n";}
    }
};




int main()
{
    for(int hour=1;hour<=24;hour++)
    {

    }
    return 0;
}
