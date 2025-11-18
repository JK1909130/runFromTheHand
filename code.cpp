
#include <iostream>
#include <ctime>
using namespace std;
class Food
{   
    public:
    int chance = 0;
    int distance = 0; 
};
class Drink
{   public:
    int chance = 0;
    int distance = 0;
};
class Weapon
{
    public:
    string name;
    int durability;
    float damage;
    int slot; // 1 for two-handed , 2 for small-dual-wieldable,
    float weight;
};
class Firearm : public Weapon
{
    float range;
    int accessory; // 1 silencer, 2 stablizer, 3 enhancer
    int max_ammo;
    int current_ammo;
};
class Melee : public Weapon
{

};
class Player
{
public:
double velocity;
int thirst;
int hunger;
double coords;
Weapon main_hand;
Weapon off_hand;
Player()
{
    velocity = 0; //human max walking speed here is 5km/h or 5000m/h
    thirst = 0;
    hunger = 0;
    coords = 10000.0;
}
Player(char diff) //set player coordinates based on diff,  normally the hand spawns at coords 0 and thee player somewhere further. By reeducing the player's coords thee game geets harder
{
    if(diff == '1')
    {
        coords = 30000.0;
    }else if(diff == '2')
    {
        coords = 20000.0;
    }else if(diff == '3')
    {
        coords = 10000.0;
    }else throw "invalid difficulty\n";
} 
void setDiff(int diff)
{
    this->coords = 30000.0/diff;
}
};
class Hand
{
public:
    double velocity;
    double coords;

    Hand(): coords(0.0), velocity(0.0){}
    void refresh(){}
};
int main()
{
    srand(time(NULL));
    Hand h;
    Player p;
    ah:
    int diffi = 1;
    cout<<"\n\nChoose difficulty:\n 1. Easy  2. Medium  3. Hard\n and press Enter\n";
    cin>>diffi;
    switch(diffi)
    {
        case 1: 
        {
            cout<<"- The hand always move slightly faster than you.\n- You have 24hrs until it switches victim.\n";
            cout<<"The Hand is 30 km from you.\n";
            p.setDiff(1);
            break;
        }
        case 2: 
        {
            cout<<"- The hand always move slightly faster than you.\n- You have 24hrs until it switches victim.\n";
            cout<<"The Hand is 15 km from you.\n";
            p.setDiff(2);
            break;
        }
        case 3: 
        {
            cout<<"- The hand always move slightly faster than you.\n- You have 24hrs until it switches victim.\n";
            cout<<"The Hand is 10 km from you.\n";
            p.setDiff(3);
            break;
        }
        default: {
                cout<<"invalid difficulty\n";
                goto ah;
                 }
    }
    for(int i=0;i<24;i++)
    {
        cout<<"Hr #"<<i+1<<": \n";
        cout<<"Saturation: "<<100-p.hunger<<"\n";
        cout<<"Hydration: "<<100-p.thirst<<"\n";
        cout<<"The Hand's distance from you: "<<p.coords-h.coords<<"m(s)"<<"\n\n";
        Food f;
        Drink d;
        f.chance = (rand()%3)+1;
        if(f.chance == 1)
        {   
            int posNeg = rand()%2;
            if(posNeg == 0)
            {
                f.distance = (rand()%300)+1;
            }else {f.distance = ((rand()%300)+1)*-1;}
        }
        d.chance = (rand()%3)+1;
        if(d.chance == 1)
        {   
            int posNeg = rand()%2;
            if(posNeg == 0)
            {
                d.distance = (rand()%300)+1;
            }else {d.distance = ((rand()%300)+1)*-1;}
        }//////////////////////////////////////////////////////
        if(p.thirst>=100||p.hunger>=100||h.coords>=p.coords)
        {
            cout<<"GAME OVER\n\n";
            
        }
        if(i==23)
        {
            cout<<"YOU WIN\n\n";
            
        }/////////////////////////////////////////////////////
        if(f.chance==1)
        {
            if(f.distance>0)
            {
                cout<<"food location: "<<f.distance<<"m away from the Hand\n";
            }else if(f.distance<0)
            {
                cout<<"food location: "<<abs(f.distance)<<"m towards the Hand\n";
            }
        }else{cout<<"no food available nearby\n";}

        if(d.chance==1)
        {
            if(d.distance>0)
            {
                cout<<"drinks location: "<<d.distance<<"m away from the Hand\n";
            }else if(d.distance<0)
            {
                cout<<"drinks location: "<<abs(d.distance)<<"m towards the Hand\n";
            }
        }else{cout<<"no drinks available nearby\n";}
        //add spawn chance and show distance and direction to player  DONE
        cout<<"\n\n";
        cout<<"-------------------------\n";
        cout<<"| Actions:              |\n";
        cout<<"| 1. move towards food  |\n";
        cout<<"| 2. move towards water |\n";
        cout<<"| 3. move...            |\n";
        cout<<"| 4. stay still         |\n";
        cout<<"| 5. Exit               |\n";
        cout<<"-------------------------\n";
        cout<<"If there are no sustenance and you try to move towards them, nothing will happen\n";
        int input;
        cin>>input;
        switch(input)
        {
            case 1: //towards food
                if(f.chance==1)
                {
                    p.coords+=f.distance;
                    p.velocity=f.distance/60;
                    h.coords+=(p.velocity+p.velocity*0.1)*60;
                    p.hunger-=40;
                    p.thirst+=20;
                    if(p.hunger<0){p.hunger=0;}
                }else{
                    p.velocity=0.0;
                    h.coords+=(1+1*0.1)*60;
                    p.hunger+=20;
                    p.thirst+=20;
                }
                break;
            case 2:  //towards water
                if(d.chance==1)
                {
                    p.coords+=d.distance;
                    p.velocity=d.distance/60;
                    h.coords+=(p.velocity+p.velocity*0.1)*60;
                    p.thirst-=40;
                    p.hunger+=20;
                    if(p.thirst<0){p.thirst=0;}
                }else{
                    p.velocity=0.0;
                    h.coords+=(1+1*0.1)*60;
                    p.thirst+=10;
                    p.hunger+=10;
                }
                break;
            case 3: //move in a direction
                {
                    cout<<"enter coordinate to move to (m)(negative to move towards the Hand): ";
                    double movement;
                    cin>>movement;
                    if(abs(movement)>0)
                    {
                        p.coords+=movement;
                        p.velocity=movement/60;
                        h.coords+=(p.velocity+p.velocity*0.1)*60;
                        p.thirst+=20;
                        p.hunger+=20;
                    }else{
                        p.velocity=0.0;
                        h.coords+=(1+1*0.1)*60;
                        p.thirst+=10;
                        p.hunger+=10;
                    }
                }
                break;
            case 4:
                {
                        p.velocity=0.0;
                        h.coords+=(1+1*0.1)*60;
                        p.thirst+=10;
                        p.hunger+=10;
                }break;
            case 5: return 0; break;
        }
        cout<<"\n\n\n";
    }
    return 0;
}



