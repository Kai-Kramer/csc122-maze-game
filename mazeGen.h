#ifndef KAI_MAZE_GEN
#define KAI_MAZE_GEN
/* Kai Kramer - CSC 122-001 (Fall 2019)
 * I am going to utilize a binary tree to build a dungeon.
 */

#include "tile.h"

#include <random> //I don't have to worry about signed/unsigned types like
                  //with cstdlib::rand()
#include <vector> //because arrays are scary.
//#include <memory> //std::unique_ptr so I don't have use delete/new mid-loop.
//RNG code copied from 
//en.cppreference.com/w/cpp/numeric/random/uniform_(int | rand)_distribution


using shortRNG = std::uniform_int_distribution<short>;

using std::vector; //these allow me to smush a c'tor to acceptable length
using vec = vector<Tile>; //see Dungeon::Dungeon()
 
constexpr short SUBROOM_MIN_SIZE = 5,
                   ROOM_MIN_SIZE = SUBROOM_MIN_SIZE + 2,
                   ROOM_MAX_SIZE = 24,
               
                    LEVEL_LENGTH = 80, // recommend 80
                    LEVEL_HEIGHT = 40; // reccomend 40

struct R2Vec 
{
    point head, tail, vectorDiff;
    enum dir {
        north = 0,
        neast,
        eeast,
        seast,
        south,
        swest, 
        wwest, 
        nwest,
        invalid
    } rotation;
            //defaults to unit vector i
    R2Vec(point a = point{0,0} , point b = point{0,1}) 
    : head{b} //to b
    , tail{a} //from a
    , vectorDiff{b - a} //a raw "vector"
    , rotation{orientDiff()}
                
                {}
            //unary op-
    void operator-() //swap head/tail, recalculate vectorDiff
    { 
    point c = tail; tail = head; head = c; 
    //TODO: replace with std::swap when I templat-ify
    vectorDiff = point{head - tail}; rotation = orientDiff(); return;
    }
    
    /* perhaps add more operators if I want more sophisticated corridor gen? */
    
    dir orientDiff() //tells me (roughly) in what direction p1 is w.r.t. p2
    {
        dir rot;
        if (vectorDiff.x < 0)
        {   if(vectorDiff.y < 0)
            {   rot = neast; }
        
       else if(vectorDiff.y > 0)
            {   rot = seast; }
        
       else //vectorDiff.y == 0)
            {   rot = eeast; }
        }
   else if (vectorDiff.x > 0) 
        {   if(vectorDiff.y < 0)
            {   rot = nwest; }
        
       else if(vectorDiff.y > 0)
            {   rot = swest; }
        
       else //vectorDiff.y == 0)
            {   rot = wwest; }
        }
   else //vectorDiff.x == 0)
        {   if(vectorDiff.y < 0)
            {   rot = north; }
        
       else if(vectorDiff.y > 0)
            {   rot = south; }
        
       else 
            {   rot = invalid; }
        }
        return rot;
    }
};
               
struct Rectangle : public point
{       //if 'width' is 'wide-ness',
    short width,
        //then it logically follows: 
          talth;
        //is 'tall-ness' :^)
        
    bool doorMade;
        
                    //top-left corner. The full name was too long :(
                    //not "The Learning Channel", ever.
        Rectangle(point tlc = point{0,0}, short wid = 0, short tal = 0)
        :    point{tlc.x, tlc.y}
        ,    width{wid}
        ,    talth{tal}
        , doorMade{false}
            {}
        
        //[t]op [R]ight [X]-coordinate of a Rectangle.
    short tRX() const
    { return static_cast<short>(x + width); }
    
        //[b]ottom [L]eft [Y]-coordinate of a Rectangle.
    short bLY() const
    { return static_cast<short>(y + talth); }
    
    point base() const
    { return point{y, x}; }
    
    point topRight() const
    { return point{static_cast<short>(x + width), y}; }
    
    point bottomLeft() const
    { return point{x, static_cast<short>(y + talth)}; }
    
    point opposite() const
    { return point{this->tRX(), this->bLY()}; }
    
    bool operator !() //iff EVERYTHING is 0.
    { return (!x && !y && !width && !talth); }
    
    void set(point p)
    { x = p.x; y = p.y; }
};

//analogous to a Leaf
struct Sector
{
    point root;
    
    short width, talth;
    
  //'room' is essentially a sub-Rectangle within Sector.
  //The outer Rectangle provides padding to the inner Rectangle, for variation.
    Rectangle room, hallA, hallB;
    
    Sector*  leftKid;
    Sector* rightKid;
          
    static vector<Rectangle> corridors;
    static vector<Rectangle> allRooms;
    
    Sector(point p, short t, short w)
    :      root{p}
    ,     width{w}
    ,     talth{t}
    ,      room{Rectangle()}
    ,     hallA{Rectangle()}
    ,     hallB{Rectangle()}
    ,   leftKid{nullptr}
    ,  rightKid{nullptr}
        {}
    Sector(const Sector&) = delete;
    Sector& operator=(const Sector&) = delete;
  //allows for exponential growth. Like organic tissue. Only this isn't.
    bool split();
    
      //badda bing,
    void createRooms();
    
      //badda boom,
    void createHall(Rectangle, Rectangle);
    
      
    Rectangle getRoom();
    
};

struct Dungeon
{
    vector<vec> level;
    //vector<Rectangle> halls;
    vector<Sector*> rooms;
                //(2,2) is an offset from the room's top-left corner
    point curPos, startTile{2,2}, endTile{2,2};
    
    Dungeon()               //create a rectangle of walls, more or less.
    : level{vector<vec>(LEVEL_HEIGHT, vec(LEVEL_LENGTH, Tile()))} 
    //, halls{vector<Rectangle>()}
    , rooms{vector<Sector*>()}
    , curPos{point{0,0}}
    {}
    
  //fill up rooms
    void populate(Sector& r);
    
    
  //write to level
    void inscribe();
  
  //print out level
    void print();
    
    Tile& at(point p)
    { return level[p.y][p.x]; }
    
    Tile& at(size_t x, size_t y)
    { return level[y][x]; }
};

inline short max(short a, short b)
{   return (a > b)? a : b;   }
#endif