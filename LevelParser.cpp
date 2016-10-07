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
std::string level[] = {"wwwww\nwbbbw\nwbmbw\nsbbbs\nsssss",
                  "wwwww\nwbbbw\nwbmbw\nwbbbw\nsssss",
                  "wwwww\nwsssw\npsmsp\npsssp\nppppp",
                  "wwwww\npwwwp\npwmwp\npbbbp\nppppp",
                  "wwwww\npooop\npooop\npooop\nppppp"};

static void parseLevel(std::vector<Block*> blocks, Ogre::SceneManager *curManager)
{
    int id = 1;
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
                case 'o':
                    // do nothing, but increment the id
                    break;
            }
            id++;
        }
    }
}