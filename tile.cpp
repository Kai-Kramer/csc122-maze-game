/* Kai Kramer - CSC 122-001 (Fall 2019)
 */

#include "tile.h"
#include <iostream>
#include <string>

void Tile::beFloor()
{
    tileType = Tile_T::Floor; 
    symbol = color("37;40m.");
    passable = true; 
    opaque = false; 
}

void Tile::beWall()
{ 
    tileType = Tile_T::Wall; 
    symbol = color("37;40m#");
    passable = false; 
    opaque = true;
}

void Tile::beDoor()
{ 
    tileType = Tile_T::Door; 
    symbol = color("38;5;130;40m%");
    passable = false; 
    opaque = true;
}

void Tile::open()
{
    if(this->isDoor() && !passable)
    { symbol = color("38;5;236;40m%"); passable = true; opaque = false; }
    else
    { std::cout << "You can't open that!" << std::endl; }
}

void Tile::close()
{
    if(this->isDoor() && passable)
    { this->beDoor(); }
    else
    { std::cout << "You can't close that!" << std::endl; }
}

void Tile::beUpStair()
{ 
    tileType = Tile_T::UpStairs; 
    symbol = color("1;36;2;40m^");
    passable = true; opaque = false; 
}

void Tile::beDownStair()
{ 
    tileType = Tile_T::DownStairs; 
    symbol = color("1;36;2;40mv"); 
    passable = true; opaque = false;
}

/* std::string Tile::illumiPrint() const;
{
    if(brightness != 0)
    {
       const std::string sym = symbol;
    }
    else if(visited)
    {
        switch(tileType)
        {
            case Tile_T::Wall:
               const std::string sym = color("38;5;238;40m#");
               break;
            
            case Tile_T::Floor:
               const std::string sym = color("38;5;238;40m.");
               break;
            
            case Tile_T::Door:
               const std::string sym = color("38;5;238;40m%");
               break;
            
            case Tile_T::UpStairs:
               const std::string sym = color("38;5;238;40m^");
               break;
               
            case Tile_T::DownStairs:
               const std::string sym = color("38;5;238;40mv");
               break;
               
            default:
               const std::string sym = color("40m ");
               break;
        }
    }
    else
    {    //just a black, all-consuming void...
        const std::string sym = color("40m ");
    }
    
    return sym;
} */