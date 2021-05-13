/* Kai Kramer -- CSC 121-001
 * First attempt at roguelike...
 * This will be the implementation for entities. The more code here, the more
 * points this will be worth.
 */
#include "tileEntity.h"
#include "mazeGen.h"
#include <iostream>
#include <vector>
#include <random> //this will be removed very soon. I hope.

std::random_device seedll; //remove these, too!!
std::mt19937 genll(seedll());

        //creates a von-neumann neighborhood of specified range in some level d.
void T_Player::calcSight(Dungeon& d, short range, point p)
{
  //recursive version of while???
    if(range != 0) 
    {//I also don't want it to go backwards, 'cuz that be stack intensive, yo!
        if(d.at(p).light() < range) //we haven't checked this tile, yet.
        {
            if(!d.at(p).isOpaque())
            {
            d.at(p).setBrightness(range);
            
            d.at(p).visit();
            
            
            T_Player::calcSight(d, sc(range - 1), p + point{0, -1} ); //UP
            T_Player::calcSight(d, sc(range - 1), p + point{0,  1} ); //DOWN
            T_Player::calcSight(d, sc(range - 1), p + point{-1, 0} ); //LEFT
            T_Player::calcSight(d, sc(range - 1), p + point{1,  0} ); //RIGHT
            }
            else
            {   //we still need to see walls...
                d.at(p).setBrightness(1); 
                d.at(p).visit();
            }
        }
    }
    else
    {
        if(range == 0) //and so ends the recursion.
        { d.at(p).setBrightness(range); }
        
        std::cout << std::endl;
    }
    return;
}

void T_Player::move(Dir direction, Dungeon& d)
{
    point cur = location;
    switch(direction)
    {
        case Dir::UP: 
        
            cur += point{0 , -1}; 
            //The door's gotta open before we walk through it! (QOL)
            if(d.at(cur).isDoor()) 
            {  
                d.at(cur).open(); 
                std::cout << "The door swings open.";
            }
            if(d.at(cur).isPassable())
            {   
            
                location = cur; //the actual movement
                cur += point{0 , 1};
                
                if(this->look(Dir::DOWN, d).isDoor())
                {
                    d.at(cur).close();
                    std::cout << "You shut the door behind you.";
                }
            }
            break;
            
        case Dir::LEFT:
        
            cur += point{-1 , 0};
            
            if(d.at(cur).isDoor())
            {  
                d.at(cur).open(); 
                std::cout << "The door swings open.";
            }
            if(d.at(cur).isPassable())
            {
                
            
                location = cur;
                cur += point{1 , 0};
                
                if(this->look(Dir::RIGHT, d).isDoor())
                {
                    d.at(cur).close();
                    std::cout << "You shut the door behind you.";
                }
            }
            break;
            
        case Dir::DOWN:
        
            cur += point{0 , 1};
            
            if(d.at(cur).isDoor())
            {
                d.at(cur).open(); 
                std::cout << "The door swings open.";
            }
            if(d.at(cur).isPassable())
            {
                location = cur;
                cur += point{0 , -1};
                
                if(this->look(Dir::UP, d).isDoor())
                {
                    d.at(cur).close();
                    std::cout << "You shut the door behind you.";
                }
            }
            break;
            
        case Dir::RIGHT:
        
            cur += point{1 , 0};
            
            if(d.at(cur).isDoor())
            {
                d.at(cur).open(); 
                std::cout << "The door swings open.";
            }                    
            if(d.at(cur).isPassable())
            {
                location = cur;
                cur += point{-1 , 0};
                
                if(this->look(Dir::LEFT, d).isDoor())
                {
                    d.at(cur).close();
                    std::cout << "You shut the door behind you.";
                }
            }
            break;
            
        default:
            break;
    }   //after all is said and done, take a quit look around!
    this->see(d);
    return;
}

//this will be very quickly deprecated. Really. I swear.
void Skeleton::see(Dungeon& d)
{
    std::vector<Dir> validDirections;
    short size = 0;
    
    //look to see if a direction is passable or a door
    
    if(this->look(Dir::UP, d).isPassable() 
      || this->look(Dir::UP, d).isDoor())
    { validDirections.push_back(Dir::UP); size++; }

    if(this->look(Dir::DOWN, d).isPassable() 
      || this->look(Dir::DOWN, d).isDoor())
    { validDirections.push_back(Dir::DOWN); size++; }

    if(this->look(Dir::RIGHT, d).isPassable() 
      || this->look(Dir::RIGHT, d).isDoor())
    { validDirections.push_back(Dir::RIGHT); size++; }

    if(this->look(Dir::LEFT, d).isPassable() 
      || this->look(Dir::LEFT, d).isDoor())
    { validDirections.push_back(Dir::LEFT); size++; }
    
    /* This will be replaced. So messy. */
    if(!validDirections.empty())
    {
        short s = shortRNG{0, size}(genll);
        nextMove = validDirections[s];
    }
}
