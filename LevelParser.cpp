//
// Created by sidhants on 10/7/16.
//

#include "LevelParser.h"
#include "Block.h"
#include <string>
#include <vector>

/* Level Design:
 * p = paper, w = wood, s = stone, b = brick, m = metal, o = empty
 * layer 1:
 * wwwww
 * wbbbw
 * wbmbw
 * sbbbs
 * sssss
 * layer 2:
 * wwwww
 * wbbbw
 * wbmbw
 * wbbbw
 * sssss
 * layer 3:
 * wwwww
 * wsssw
 * psmsp
 * psssp
 * ppppp
 * layer 4:
 * wwwww
 * pwwwp
 * pwmwp
 * pbbbp
 * ppppp
 * layer 5:
 * wwwww
 * pooop
 * pooop
 * pooop
 * ppppp
 */
std::string level[] = {"pwsbm",
                       "psmbw",
                       "psmbw",
                       "pwww"};

std::vector<Block*> LevelParser::parseLevel(std::vector<Block*> blocks, Ogre::SceneManager *curManager)
{
    int id = 1;
    double y = 100;
    double x = 400;
    double z = -200;
    double width = 50;
    double height = 20;
    double length = 20;
    bool gap = false;
    for(std::string layer : level)
    {
        for(char& c : layer)
        {
            switch(c)
            {
                case 'p':
                    blocks.push_back(new Block(curManager, Block::paper, id));
                    break;
                case 'w':
                    blocks.push_back(new Block(curManager, Block::wood, id));
                    break;
                case 's':
                    blocks.push_back(new Block(curManager, Block::stone, id));
                    break;
                case 'b':
                    blocks.push_back(new Block(curManager, Block::brick, id));
                    break;
                case 'm':
                    blocks.push_back(new Block(curManager, Block::metal, id));
                    break;
                default:
                    gap = true;
                    break;
            }
            printf("Placing block with id %i at (%f, %f, %f)\n", id, x, y, z);
            blocks.at(id - 1)->setSize(width, height, length);
            blocks.at(id - 1)->setPosition(x, y, z);

            z += width + 5;
            if(z >= 200)
            {
                z = -200;
                x -= length + 5;
            }
            if(!gap)
            {
                id++;
            }
            else
            {
                gap = false;
            }
        }
        x = 400;

        y += height + 20;
    }
    return blocks;
}