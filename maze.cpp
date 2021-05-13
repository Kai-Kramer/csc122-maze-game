/*?
$%      Kai Kramer       \\
(((| Roguelike--Attempt I  ^
###########################\
#CSC 122 - 001, [Main file]#
p[ *///####################/


    /*!
      TODO:  (a * indicates that requirements are mixed with options)
~~~~~[L 4.0] * Procedurally generate a bare level~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~*[L 1.5] * Implement player--movement and interaction~~~~~~~~~~~~~~~~~~~~~
                -> MUST MAKE USER INPUT BETTER
~~~~~[L 7.0] * Implement a lighting system (limited FOV)~~~~~~~~~~~~~~~~~~~~~~
~~~~~[req]   * Finalize room layout (have a spawn point and end of maze)~~~~~~
    *[L 0.5] * Implement immobile treasures, enemies, and traps
     [req]   * Make treasures worth value
     [req]   * Make enemies move around, blocking the player's movement
     [req]   * Add a menu and game persistence
             * Assignment worth (Level 23.0) if all of the above are complete.

             Bounties:
     [L 1.5] * Vampires avoid garlic
     [L 4.0] * Implement a Combat system w/ tiered equipment
~~~~~[L 3.0] * Allow for large levels, where the map stays centered~~~~~~~~~~~
     [L 2.0] * Add stairs and multiple depths
     [L 2.0] T Rudimentary pathfinding
             I_[L+1.0] Variable intelligence
             I_[L+4.0] No dumb-ness

          If all bounties are completed, Assignment worth (Level 41.5) */
            

#include "mazeGen.h"// me work long 'n hard 4 this 1 :^)
#include "tileEntity.h"
#include <iostream> // cin/cout will be used for gameplay
#include <fstream>  // for saving/loading a game. 
#include <vector>   // stoor floorage. Floor storage? Floorage!
#include <forward_list> // just a singly-linked list.
#include <memory>   // std::unique_ptr, so I don't have to worry about memory.
#include <string>   //for string::getline

constexpr short RENDER_DISTANCE = 17;

std::string userInput;
Dir direction;
bool properResponse;
point curPos;

typedef std::vector<point>::size_type p_v_Sz;

int main()
{
    Dungeon depth1;
    
    Sector map1(point{0,0}, LEVEL_LENGTH, LEVEL_HEIGHT);
    
    depth1.populate(map1);
  
    map1.createRooms();
    
    depth1.inscribe();
    
    std::cout << "\nMaze Generation Complete..." << std::endl;
    
    std::vector<std::unique_ptr<T_Entity> > entities;
    std::vector<point> filledPoints; //so I don't turn O(n) into O(n^2)
    
    entities.push_back(std::make_unique<T_Player>(depth1.startTile));
    
 // just random enemies. They don't do anything.
    entities.push_back(std::make_unique<Skeleton>(depth1.startTile + point{1,1}));
    entities.push_back(std::make_unique<Skeleton>(depth1.startTile + point{0,1}));
    entities.push_back(std::make_unique<Skeleton>(depth1.startTile + point{1,0}));
    
 // getting an initial lay of the land
    entities[0]->calcSight(depth1, RENDER_DISTANCE, depth1.startTile);
    depth1.print();
    
    while (entities[0]->echolocate() != depth1.endTile)
    {
        filledPoints.clear(); // reset this
        
        //print commands,
        properResponse = false;
        char c;
        
        while(!properResponse)
        {
            std::cout << "> ";
            getline(std::cin, userInput);
            std::cout << std::endl;
            
            std::string::size_type s_t = userInput.find_first_of("1234wasdWASDmM");
            c = (s_t == std::string::npos)? '\0' : userInput.at(s_t);
            //process input,
            switch(c)
            {
                case 'w':
                case 'W': // rudimentary
                case '1':
                    direction = Dir::UP;
                    properResponse = true;
                    break;
                    
                case 'a':
                case 'A': // input
                case '2':
                    direction = Dir::LEFT;
                    properResponse = true;
                    break;
                    
                case 's':
                case 'S': // validation,
                case '3':
                    direction = Dir::DOWN;
                    properResponse = true;
                    break;
                    
                case 'd':
                case 'D': // probably
                case '4':
                    direction = Dir::RIGHT;
                    properResponse = true;
                    break;
                    
                case 'm':
                case 'M':
                    depth1.print();
                    break;
                    
                default:
                    break;
            }
        }

//std::cout << "OK " << std::endl;

        //process player (inter)action,
        
        entities[0]->move(direction, depth1);
        filledPoints.push_back(entities[0]->echolocate()); //REMOVE THIS
        
//std::cout << "Check one... ";
        
        //loop thru entities, will be refined.
        for(size_t i = 1; i < 4; i++)
        {
            entities[i]->see(depth1);
            entities[i]->move(entities[i]->nextMove, depth1);
            filledPoints.push_back(entities[i]->echolocate()); 
            std::cout << entities[i]->echolocate() << filledPoints.size() << std::endl;
        }
        
//std::cout << "Check two... "; 
        
        
      //first, I will construct a rectangle centered about the user 
      //player should be at the center of this box. AKA RENDER_DISTANCE/2
      // --> I want integer division. Casting because short gets promoted.
        point renderBase{sc(RENDER_DISTANCE) , sc(RENDER_DISTANCE / 2)};
        renderBase = entities[0]->echolocate() - renderBase; //true center
//std::cout << renderBase;        
      // coords < 0 is OOB
        if(renderBase.x < 0) {renderBase.x = 0;}
        if(renderBase.y < 0) {renderBase.y = 0;}
//std::cout << " -> " << renderBase;
      // now we can safely construct the rectangle...        
        Rectangle renderBox(renderBase, sc(RENDER_DISTANCE * 2), RENDER_DISTANCE);
        
      // but wait, coords > LEVEL_CONSTS is also OOB...
        if(renderBox.opposite().x > LEVEL_LENGTH) 
        { renderBase.x = sc(LEVEL_LENGTH - sc(RENDER_DISTANCE*2)); }
        
        if(renderBox.opposite().y > LEVEL_HEIGHT)
        { renderBase.y = sc(LEVEL_HEIGHT - RENDER_DISTANCE); }
//std::cout << " -> " << renderBase << std::endl;        
      // and NOW it's safe!
        renderBox.set(renderBase);
        
            //faster than v1 by a factor of 
            //(LEVEL_HEIGHT * LEVEL_LENGTH)/(RENDER_DISTANCE^2)
            // so like, 12.5x faster using default values...
        for(short y = renderBox.y; y < renderBox.opposite().y; y = sc(y + 1) )
        { for(short x = renderBox.x; x < renderBox.opposite().x; x = sc(x + 1) )
            {
                curPos = point{x, y};
                p_v_Sz match = filledPoints.size(); // basically string::npos?
                p_v_Sz i = 0;
                
                for(point p : filledPoints)
                { if(depth1.at(p).light() != 0 && p == curPos)
                    { match = i; }
                    i++;
                }
                        //this gives entities precedence over tiles.
                if(match != filledPoints.size())
                {
                    std::cout << *entities[match];
                }
                else
                {
                    std::cout << depth1.at(x, y).illumiPrint();
                }
            }
            std::cout << std::endl;
        }
    }
    return 0;
}