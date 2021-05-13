/* Kai Kramer - CSC 122-001 (Fall 2019)
 * I am going to utilize a binary tree to build a dungeon.
 */
#include "mazeGen.h"
#include <random> //better than cstdlib + ctime.
#include <iostream>//std::cerr, print().
#include <vector> //again, arrays suck.
#include <cmath>   //abs

                 //gives me a random seed so I can avoid <ctime>
std::random_device seed; 

                 //"mersenne twister" engine rather than cstdlib 'rand()'
std::mt19937 gen(seed());
std::uniform_real_distribution<> randPctage(0.0, 1.0);


std::vector<Rectangle> Sector::corridors;
std::vector<Rectangle> Sector::allRooms;

//allows for exponential growth. Like organic tissue. Only this isn't .
bool Sector::split()
{
    

  //if we haven't already split
    if (!leftKid && !rightKid)
    {

        //(intelligently) determine which way we want to split
        //initial random choice--we will change this depending on Sector dim.
        bool verticalSplit = (randPctage(gen) < 0.33); //33%
        
        //if width is larger by 25% or more, split vertically.
        //if talth is larger by 25% or more, split horizontally.
        if(width > talth && static_cast<double>(width) / talth >= 1.75)
            { verticalSplit = true; }
        
        else if(talth > width && static_cast<double>(talth) / width >= 1.35)
            { verticalSplit = false; }

        short max = sc((verticalSplit ? width : talth) - ROOM_MIN_SIZE);
  
        if(max > ROOM_MIN_SIZE)
        {
            short split = shortRNG{ROOM_MIN_SIZE, max}(gen);

            if(verticalSplit)
            {
                leftKid = new Sector(root, talth, split);
                rightKid = new Sector(point{sc(root.x + split), root.y},
                                    talth, sc(width - split));
            }
            else
            {
                leftKid = new Sector(root, split, width);
                rightKid = new Sector(point{root.x, sc(root.y + split)},
                                    sc(talth - split), width);
            }
        }
        else
        {/*std::cerr << "cannot split further, room too small.\n";*/}
    }
    else
    {std::cerr << "ceased split branch.\n";}
return (leftKid||rightKid);
}

void Sector::createRooms()
{
    if(leftKid||rightKid)
    {
        if(leftKid)
        {
            leftKid->createRooms();
        }
        if(rightKid)
        {
            rightKid->createRooms();
        }           
        if(leftKid && rightKid)
        {
            Sector::createHall(leftKid->getRoom(), rightKid->getRoom());
        }
    }
    else
    {
      //if I wasn't limited to 80 chars, ths would be 1 statement.
      // (...to, uh, save on, uh, like, uh, variable... allocation?)
        short roomWidth,
              roomTalth,
                  roomX,
                  roomY;
                  
      //lhs of `op,()` sets RNG to proper range. rhs assigns random number.
        roomWidth = shortRNG{SUBROOM_MIN_SIZE, sc(width - 2)}(gen);
        roomTalth = shortRNG{SUBROOM_MIN_SIZE, sc(talth - 2)}(gen);
                    
            roomX = shortRNG{1, sc(width - roomWidth - 1)}(gen);
            roomY = shortRNG{1, sc(talth - roomTalth - 1)}(gen);
                    
        room = Rectangle(point{sc(roomX+root.x), sc(roomY+root.y)},
                         roomWidth, roomTalth);
        allRooms.push_back(room);
        
        //std::cerr << "Room|";
    }
    return;
}

Rectangle Sector::getRoom()
{
    Rectangle theRoom;
    if (!!room) //I don't think that this is redundant...
    {  theRoom = room; } 
    else
    {
        Rectangle leftRoom,
                 rightRoom;
        if(leftKid)
        {   leftRoom = leftKid->getRoom(); }
        if(rightKid)
        {   rightRoom = rightKid->getRoom(); }
        
        //if either leftRoom or rightRoom are not exist, return the other
        //if both exist, flip a coin!
        if(!leftRoom&&!rightRoom)
            //this is a dummy rectangle, effectively NULL.
        {   theRoom = Rectangle(point{0,0},0,0);}
        else if(!rightRoom)
        {   theRoom = leftRoom; }
        else if(!leftRoom)
        {   theRoom = rightRoom; }
        else if(randPctage(gen) < 0.5)
        {   theRoom = leftRoom; }
        else
        {   theRoom = rightRoom; }
    }
    return theRoom;
}


void Dungeon::populate(Sector& r)
{
    rooms.push_back(&r);
    
    bool splitted = true;
    
    while(splitted)
    {
        splitted = false;
        for(const auto rm : rooms)
        {           //if current node has no kids,
            if(!rm->leftKid && !rm->rightKid)
            {           //and is larger than it should be
                if(rm->width > ROOM_MAX_SIZE || rm->talth > ROOM_MAX_SIZE
                    || randPctage(gen) < 0.75) //(75% to extra-split a room.)
                {       //split the room into two smaller rooms
                    if(rm->split())
                    {
                        rooms.push_back(rm->leftKid);
                        rooms.push_back(rm->rightKid);
                        splitted = true;
                    }//!~^ `````````
                }//e~^ ```````......
            }//e~^ `````......------
        }//h~^ ```......------======
    }//w~^ `......------======{{{{{{
    return;
}

void Sector::createHall(Rectangle lRec, Rectangle rRec)
{   
            //lRec.x + 1 is 1 unit to the right of the top-left corner,
            //lRex.tRX -2 is 2 units to the left of the top-right corner.
            //essentially, range is ~ the top line of the Rectangle.
    short x = shortRNG{sc(lRec.x + 1), sc(lRec.tRX() - 2)}(gen);

    short y = shortRNG{sc(lRec.y + 1), sc(lRec.bLY() - 2)}(gen);

            
    point p1{x, y}; 
    
          x = shortRNG{sc(rRec.x + 1), sc(rRec.tRX() - 2)}(gen);

          y = shortRNG{sc(rRec.y + 1), sc(rRec.bLY() - 2)}(gen);

          
    point p2{x, y};
    
        //w = width, h = talth
    R2Vec delta(p1, p2);
    //p1.x = sc(p1.x + root.x); p1.y = sc(p1.y + root.y);
    //p2.x = sc(p2.x + root.x); p2.y = sc(p2.x + root.y);
    
    switch(delta.rotation)
    {
        case R2Vec::north: //push_back temp Rectangles => they die out of scope
            hallA = Rectangle{p2, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
            corridors.push_back(hallA); 
//std::cerr << "corridor|" << corridors.size() << " " << corridors.size() << "\n";
            break;
            
        case R2Vec::neast: //push_back each branch => no empty Rectangles
            if (randPctage(gen) < 0.5)
            {/*   P2
              * h |
              *   v
              *   +-->P1
              *     w     
              */
                hallA = 
                      Rectangle{point{p2.x, p1.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p2.x, p2.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            }
            else
            {/*    w
              * P2-->+
              *      | h
              *      v
              *      P1
              */
                hallA = 
                      Rectangle{point{p2.x, p2.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB =
                      Rectangle{point{p1.x, p2.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " " << " ";
            } break;
            
        case R2Vec::eeast:
        
            hallA = Rectangle{p2, sc(std::abs(delta.vectorDiff.x) + 1), 1};
            corridors.push_back(hallA);
//std::cerr << "corridor|" << corridors.size() << " ";
            break;
            
        case R2Vec::seast:
            if (randPctage(gen) < 0.5)
            {/*      P1
              *      | h
              *      v
              * P2-->+
              *    w
              */
                hallA = 
                      Rectangle{point{p2.x, p2.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                
                hallB = 
                      Rectangle{point{p1.x, p1.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                      
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            }
            else
            {/*     w
              *   +-->P1
              * h |
              *   v
              *   P2
              */
                hallA = 
                      Rectangle{point{p2.x, p1.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p2.x, p1.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            } break;
            
        case R2Vec::south:
            hallA = Rectangle{p1, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
            corridors.push_back(hallA);
//std::cerr << "corridor|" << corridors.size() << " ";
            break;
            
        case R2Vec::swest:
            if (randPctage(gen) < 0.5)
            {/*   P1
              * h |
              *   v
              *   +-->P2
              *     w     
              */
                hallA = 
                      Rectangle{point{p1.x, p1.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p2.x, p1.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            }
            else
            {/*    w
              * P1-->+
              *      | h
              *      v 
              *      P2
              */
                hallA = 
                      Rectangle{point{p1.x, p2.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p1.x, p1.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            } break;
        
        case R2Vec::wwest:
            hallA = Rectangle{p1, sc(std::abs(delta.vectorDiff.x) + 1), 1};
            corridors.push_back(hallA);
//std::cerr << "corridor|" << corridors.size() << " ";
            break;
            
        case R2Vec::nwest:
            if (randPctage(gen) < 0.5)
            {/*     w
              *   +-->P2
              * h |
              *   v
              *   P1
              */
                hallA = 
                      Rectangle{point{p1.x, p2.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p1.x, p2.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            }
            else
            {/*      P2
              *      | h
              *      v
              * P1-->+
              *    w
              */
                hallA = 
                      Rectangle{point{p1.x, p1.y}, sc(std::abs(delta.vectorDiff.x) + 1), 1};
                      
                hallB = 
                      Rectangle{point{p2.x, p2.y}, 1, sc(std::abs(delta.vectorDiff.y) + 1)};
                corridors.push_back(hallA); corridors.push_back(hallB);
//std::cerr << "corridor|" << corridors.size() << " ";
            } break;
            
        default:
            break;
    }
    return;
}


void Dungeon::inscribe()
{
    // I need to, first, bind all Tiles to their position. I think it's easier
    // to do this linearly first
    for(short y = 0; y < LEVEL_HEIGHT; y++)
    {   for(short x = 0; x < LEVEL_LENGTH; x++)
        {
            level[y][x].setPosition(point{x, y});
        }
    }        
    
    
    for(Rectangle r : rooms[0]->allRooms)
    {   
      // a room is not a hole, it is enclosed by walls!
      // I do, still, want to ensure the permanence of its walls.
      // also, curse you, 80-char width!!
//std::cout << "\nr:(" << r.x << ',' << r.y << ")w:" << r.width << " t:" << r.talth << std::endl;
        for(
            curPos.y = r.y; 
            curPos.y < sc(r.y + r.talth); 
            curPos.y = sc(curPos.y + 1)
            )
        {
            for(
                curPos.x = r.x; 
                curPos.x < sc(r.x + r.width); 
                curPos.x = sc(curPos.x + 1)
                )
            {
              // if we are blarf with the top-right corner
             // or the bottom-left corner, then that means we are on the "shell"
                if(curPos.y == r.y || curPos.y == r.bLY() ||
                   curPos.x == r.x || curPos.x == r.tRX())
                { 
                
                    if(curPos == r.base() || curPos == r.topRight() ||
                       curPos == r.bottomLeft() || curPos == r.opposite())
                    {/* I don't want corners to be borders. No corner doors!*/}
                   else
                   {
                        level[curPos.x][curPos.y].assertBorderness(); 
                   }
                }
              // What is the word for this property? Is there a word for it??
              // E.G. (1,1) and (1,50) are BLARF, as are (1,1) and (50,1),
              // while (1,50) and (50,1) are not. (collinear by axis??)
                
                else
                {
                    level[curPos.x][curPos.y].beFloor();
//std::cerr << curPos;
                }
            }
        }
//std::cerr<<"R";
    }

/* all rooms have been placed. This should be after Rooms to get good doors*/
    bool isVertical = false;
    short length;

    //for(const auto r : rooms)
    //{
    for(Rectangle h : rooms[0]->corridors)
    {
//std::cout << "\nc:(" << h.x << ',' << h.y << ")w:" << h.width << " t:" << h.talth << std::endl;
        length = max(h.width, h.talth); // width == talth returns 0;
        isVertical = (length == h.talth);
        curPos = point{h.x, h.y};
        
        for(int i = 0; i < length; i++) // dun bother w/ degen corridors
        {           
            if(isVertical)
            {   curPos.y = h.y; curPos.y = sc(curPos.y + i); }
            else
            {   curPos.x = h.x; curPos.x = sc(curPos.x + i); }
        
        
          // if it's already been excavated, we don't need to mess with it!
            if(level[curPos.x][curPos.y].isWall()) 
            {
              // being anal about borders allows for corridors and proper
              // door placement. win-win!
                if(level[curPos.x][curPos.y].affirmBorderness() && !h.doorMade)
                {
                    level[curPos.x][curPos.y].beDoor();
                    h.doorMade = true;
                }
                else
                {
                    level[curPos.x][curPos.y].beFloor();
                }
            }
        }
        
//std::cerr << "C";
    }
//std::cerr << "\n\nFinished Base Geometry Generation.\n";
    //}
/* all corridors have been dug. */
/* sprinkling the xtras on top... */

/* If adding more stuff determined in BSP-Tree, here's the adding spot. */

/* Lastly, the start and the exit. */

    startTile += rooms[0]->allRooms.front().base(); //add randomness?
//std::cerr << "[startTile: ";
    this->at(startTile).beUpStair();
    //add in actual logic for this, if desired
//std::cerr << startTile << " ]\n";
    endTile += rooms[0]->allRooms.back().base();
//std::cerr << "[endTile: " << endTile << " ]\n";
    this->at(endTile).beDownStair();
//std::cerr << "[D]";
    //it might work just fine, who knows!!
}


void Dungeon::print()
{
    for(auto & rows : level)
    {
        for(auto & cols : rows)
        {
            std::cout << cols.illumiPrint();
        }
        std::cout << std::endl;
    }
}