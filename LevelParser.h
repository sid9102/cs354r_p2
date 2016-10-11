//
// Created by sidhants on 10/7/16.
//

#ifndef CS354R_P2_LEVELPARSER_H
#define CS354R_P2_LEVELPARSER_H

#endif //CS354R_P2_LEVELPARSER_H

#include "Block.h"
#include <OgreSceneManager.h>

class LevelParser
{
public:
    static std::vector<Block*> parseLevel(std::vector<Block*> blocks, Ogre::SceneManager *curManager);
};
