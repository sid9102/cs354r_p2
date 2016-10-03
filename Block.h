#ifndef __Block_h_
#define __Block_h_

class Block
{
public:
	Block();
	Block(SceneManager *newManager, Type t, num);
	void destroy();
	void collision();
	int getID(); 
private:
	int id;
	enum Type {paper, wood, stone, brick, metal};
	Type t;
	int durability;
	int points;
	int damage;
	enum Powerup {spare, sticky, explosion, penetrate, multi, widen, slow, gun};
	Powerup powerup;

	Ogre::SceneManager *blockManager;
	Ogre::SceneNode* blockNode;
	Ogre::Entity* blockEntity;
	void buildBlock();
};

#endif // #ifndef __Block_h_
