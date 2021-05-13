#ifndef T_ENTITY_
#define T_ENTITY_

#include "tile.h"
#include "mazeGen.h"
#include <string>  //"Symbols" are strings. They can get quite long for 1 char!
#include <iostream>//I want ostream and also cerr, dammit! (stdostream when?)

/* All Entities below */
    //big poppa -- Tile Entity.    
    
constexpr short PLAYER_INIT_HP = 150,
                PLAYER_INIT_VRANGE = 8,
                SKELETON_HP = 20;
    
class T_Entity {

    public: //ugh
    point location;
    private:
          //default is a blank space--because default T_Entitys should not have
          //a practicable application
    std::string symbol;
    
    
    short health,
          wealth; //value--the gold value of any given entity
         
    bool    awake = false,
          pursuit = false,
          
          //all entities should fill their space.
         obstruct = true;
         

         
    public: // No, this is not meant to look like a gun. That would be
            // much too powerful for the setting of this game. Pfft...
        T_Entity(point loc = point{1,1},std::string c = color("40; "),
                 short hp = 0, short val = 0)
            :location{loc.x, loc.y}//   // just a
            ,  symbol{c}/////// c'tor
            ,  health{hp}
            ,  wealth{val}
                {}
                
    Dir nextMove = Dir::UP;
    
        virtual ~T_Entity() = default;
    
        //all T_Entitys have bat-ears!!
        point echolocate() const
        { return location; }
        
        //they also have midget gnome physicians in their pocketses.
        short doPhysical() const
        { return health; }
        
        //...and, uh, prefer assessing to getting.
        short assessWealth() const
        { return wealth; }
        
        bool isAwake() const
        { return awake; }
        
        bool inPursuit() const
        { return pursuit; }
        
        bool obstructs() const
        { return obstruct; }
        
        //me am wanting unsame names for these -- it tongue flow gooder
        void rouse() 
        { awake = true; }
        
        void pacify()
        { awake = false; }
        
                //checks one adjacent tile. should never OOB 
        virtual Tile look(Dir direction, Dungeon& d)
        {
            switch(direction)
            {
                case Dir::LEFT:
                    return d.at( location + point{-1 , 0} );
                    break;
                    
                case Dir::RIGHT:
                    return d.at( location + point{1 , 0} );
                    break;
                    
                case Dir::UP:
                    return d.at( location + point{0 , -1} );
                    break;
                    
                case Dir::DOWN:
                    return d.at( location + point{0 , 1} );
                    break;
                    
                default: //?
                    return Tile();
                    break;
            }
        }
        
        virtual void move(Dir direction, Dungeon& d) 
        {
            point cur = location;
            switch(direction)
            {
                case Dir::UP: //TODO: move narration and door-closing ->T_Player
                        //I could use look, but that would be more keystrokes
                    cur += point{0 , -1}; 
                    //The door's gotta open before we walk through it! (QOL)
                    if(d.at(cur).isDoor()) 
                    {  
                       d.at(cur).open(); 
                    }
                    if(d.at(cur).isPassable())
                    {   
                    
                        location = cur; //the actual movement
                        cur += point{0 , 1};
                    }
                    break;
                    
                case Dir::LEFT:
                
                    cur += point{-1 , 0};
                    
                    if(d.at(cur).isDoor())
                    {  
                       d.at(cur).open(); 
                    }
                    if(d.at(cur).isPassable())
                    {
                        
                        location = cur;
                        cur += point{1 , 0};
                    }
                    break;
                    
                case Dir::DOWN:
                
                    cur += point{0 , 1};
                    
                    if(d.at(cur).isDoor())
                    {
                       d.at(cur).open(); 
                    }
                    if(d.at(cur).isPassable())
                    {
                        
                        location = cur;
                        cur += point{0 , -1};
                    }
                    break;
                    
                case Dir::RIGHT:
                
                    cur += point{1 , 0};
                    
                    if(d.at(cur).isDoor())
                    {
                       d.at(cur).open(); 
                    }                    
                    if(d.at(cur).isPassable())
                    {
                        
                        location = cur;
                        cur += point{-1 , 0};
                    }
                    break;
                    
                default:
                    break;
            }
        }
        
        virtual void see(Dungeon&) = 0;
        virtual void calcSight(Dungeon& d, short s, point p) = 0;
        
    protected: //this works...
        virtual void toStream(std::ostream& os) const
        {   os << symbol; }
    public:
        friend std::ostream& operator<< (std::ostream& os, const T_Entity& e)
        { e.toStream(os); return os; }
};

class T_Player : public T_Entity
{
    //private:
    //short vis_Range = PLAYER_INIT_VRANGE;
    
    public:
        T_Player(point loc = {0,0}) //A salmon ampersand on forest green.
        : T_Entity(loc, color("38;5;203;48;22m@"), PLAYER_INIT_HP)
        { }
        
      //recursiveness means we can't make this too large
        void see(Dungeon& d) override
        { calcSight(d, PLAYER_INIT_VRANGE, location); }
      
        void calcSight(Dungeon& d, short s, point p) override;
        
      //provides console output and automatic door-closing.
        void move(Dir direction, Dungeon& d);
};

/* class T_Treasure : public T_Entity
{
    public:
        T_Treasure(point loc = {0,0}) //a golden 'o'
        : T_Entity(loc, color("38;5;208;40mo"), -1)
        { }
}; */

class Skeleton : public T_Entity
{

    
    public:
        Skeleton(point loc = {0,0})
        : T_Entity(loc, color("40;38;5;29mg"), SKELETON_HP)
        { }
    
    void see(Dungeon& d) override;
  //literally wasting resources because this would be abstract class w/o def'n
    void calcSight(Dungeon& d, short s, point p) override
    { std::cout << d.at(p).isDoor() << s; }
};


#endif