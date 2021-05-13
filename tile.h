#ifndef KAI_GAME_
#define KAI_GAME_

#include <limits>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
//#include <type_traits>

constexpr char ESC = 27;                                  
                                  
        //ansi escape sequence -- ESC[ <params>
        //0 resets all formatting, the ; allows for additional parameters.
const std::string SEQ{ESC,'[','0',';'}; 

        //ansi escape sequence -- ESC[0m
        //simply clears all formatting, allowing no additional parameters.
const std::string RESET{ESC,'[','0','m'};

        //qol helper function so I don't have to write ' seq + symbol + reset '
        //a brazillian times.
inline std::string color(const std::string& str)
{ return SEQ + str + RESET; }

constexpr unsigned short BIG_SHORT = std::numeric_limits<unsigned short>::max(),
                         CRUMBLE_WALL_HP = 50;
                     

inline short sc(int i) //I really hope this does what I want it to.
{   return static_cast<short>(i); }

enum class Tile_T 
{
    Wall = 0,
    Floor,
    Door,
    UpStairs, 
    DownStairs 
};

enum class Dir
{
    UP = 0, // w
      LEFT, // a
      DOWN, // s
     RIGHT, // d
};

//template<typename T>
struct point 
{
    /* T */short x, //I decided not to do templatey stuff because I don't think
                 y; //I need to write an entire library just for maze generation
                    // *avoiding eye contact*
    
  //math operators
  
  //useful for "A*" variation of djikstra algorithm
    friend double euclidean_D(point lhs, point rhs) 
    {   return std::hypot( sc(lhs.x - rhs.x) , sc(lhs.y - rhs.y) ); }
    
                //distance on a grid
    friend short manhattan_D(point lhs, point rhs) 
    {   return sc( std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y) ); }
    
            //returning std::common_type means I ain't gotta cast
    //template<typename T>      //point + point
    point/*<std::common_type_t<T,U> >*/ operator+(point/*<U>*/ a) const
    {return point{sc(x + a.x), sc(y + a.y)}; }
    
    void operator+=(point a)
    { 
        x = sc(x + a.x); 
        y = sc(y + a.y);
    }

    //template<typename U>      //point - point
    point/*<std::common_type_t<T,U> >*/ operator-(point/*<U>*/ a) const
    {return point{sc(x - a.x), sc(y - a.y)}; }

    void operator-=(point a)
    { 
        x = sc(x - a.x);
        y = sc(y - a.y);
    }

  //logical comparison

    friend bool operator ==(point const& lhs, point const& rhs)
    { return (lhs.x == rhs.x && lhs.y == rhs.y); }
    
    friend bool operator !=(point const& lhs, point const& rhs)
    { return !(lhs == rhs); }
    
    friend std::ostream& operator<<(std::ostream& os, point p)
    { return os << "(" << p.x << "," << p.y << ")"; }
};

/* The physical building blocks */
//A basic Tile is a floor space.
class Tile
{
    Tile_T tileType;
    
        //this tile's position within the current floor
    point location;
    
              //string allows me to utilize ANSI terminal highlighting
    std::string symbol;
    
       //can we walk through this?
    bool passable,
    
       //does this stop light from penetrating it?
         opaque,
         
       //does the player know what this tile looks like?
         visited = false,
         
       //is this tile the border of a structure?
         borderness = false;
    
    
                 //how well lit is this tile?
    short brightness;
  public:
        //operator is this long to allow for "custom" tiles (closer to recolors)
    Tile(Tile_T type = Tile_T::Wall, point loc = {0, 0},
         std::string sym = color("37;40m#"), bool pass = false, bool opaque = true) 
            :      tileType{type}
            ,      location{loc}
            ,        symbol{sym} //force black background
            ,      passable{pass}
            ,        opaque{opaque}
            ,    brightness{0}
                { /* c'tor body */ }
    
    point locate() const
    { return location; }
    
    bool isPassable() const
    { return passable; }
    
    bool isOpaque()
    { return opaque; }
    
    short light() const
    { return brightness; }
    
    bool affirmBorderness() const
    { return borderness; }
    
    bool recall() const
    { return visited; }
    
    void assertBorderness()
    { borderness = true; }
    
    void setPosition(point p)
    { location = p; }
    
    void setBrightness(short b)
    { brightness = b; }
    
    void visit()
    { visited = true; }
    
    void setSymbol(std::string s)
    { symbol = color(s); }
    
  //Tile-ination
    bool isFloor() const
    { return (tileType == Tile_T::Floor); }
    void beFloor();
    
  //Wall-becomingness
    bool isWall() const
    { return (tileType == Tile_T::Wall); }
    void beWall();
      
  //Door-transformation
    bool isDoor() const
    { return (tileType == Tile_T::Door); }
    void beDoor();
    void open();
    void close();
    
  //To become an upStair
    bool isUpStair() const
    { return (tileType == Tile_T::UpStairs); }
    void beUpStair();
    
    //actual stair-ness TBD, just a starting point, for now.
    
  //Or instead for being downStair
  
    bool isDownStair() const
    { return tileType == Tile_T::DownStairs; }
    void beDownStair();
    
    //actual stair-ness TBD, just an endin point, for now.
    
        //this was giving me trouble so I stuck it in the header.
    std::string illumiPrint() 
    {  
        if(brightness > 0)
        {
           brightness = 0;
           return symbol;
        }
        else if(visited)
        {
            switch(tileType)
            {
                case Tile_T::Wall:
                   return color("38;5;238;40m#");
                   break;
                
                case Tile_T::Floor:
                   return color("38;5;238;40m.");
                   break;
                
                case Tile_T::Door:
                   return color("38;5;238;40m%");
                   break;
                
                case Tile_T::UpStairs:
                   return color("38;5;238;40m^");
                   break;
                
                case Tile_T::DownStairs:
                   return color("38;5;238;40mv");
                   break;
                
                default:
                   return color("40m ");
                   break;
                
            }
        }
        else
        {    //just a black, all-consuming void...
            return color("40m ");
        }
    }
  //for easy printing
    friend std::ostream& operator<<(std::ostream& os,const Tile& t)
    { return os << t.symbol; }
};
#endif