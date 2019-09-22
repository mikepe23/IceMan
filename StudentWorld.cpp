#include "StudentWorld.h"
#include <string>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

string StudentWorld:: StatFormat(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score)
{
    string s, twoSpace, LvlString, LivesString, HlthString, WtrString, GldString, OilString, SonarString, ScrString;
    
    twoSpace = "  ";
    
    LvlString = "Lvl: ";
    string lvl = to_string(level);
    switch (lvl.size()) {
        case 1:
            lvl.insert(0, " ");
            break;
        default:
            break;
    }
    LvlString.append(lvl);
    LvlString.append(twoSpace);
    
    LivesString = "Lives: ";
    string lvs = to_string(lives);
    LivesString.append(lvs);
    LivesString.append(twoSpace);
    
    HlthString = "Hlth: ";
    string hlth = to_string(health);
    switch (hlth.size()) {
        case 2:
            hlth.insert(0, " ");
            hlth.insert(hlth.size(), "%");
            break;
        case 3:
            hlth.insert(hlth.size(), "%");
            break;
        default:
            break;
    }
    HlthString.append(hlth);
    HlthString.append(twoSpace);
    
    WtrString = "Wtr: " ;
    string wtr = to_string(squirts);
    switch (wtr.size()) {
        case 1:
            wtr.insert(0, " ");
            break;
        default:
            break;
    }
    WtrString.append(wtr);
    WtrString.append(twoSpace);
    
    GldString = "Gld: ";
    string gld = to_string(gold);
    switch (gld.size()) {
        case 1:
            gld.insert(0, " ");
            break;
        default:
            break;
    }
    GldString.append(gld);
    GldString.append(twoSpace);
    
    OilString = "Oil Left: ";
    string oil = to_string(barrelsLeft);
    switch (oil.size()) {
        case 1:
            oil.insert(0, " ");
            break;
        default:
            break;
    }
    OilString.append(oil);
    OilString.append(twoSpace);
    
    SonarString = "Sonar: ";
    string snr = to_string(sonar);
    switch (snr.size()) {
        case 1:
            snr.insert(0, " ");
            break;
        default:
            break;
    }
    SonarString.append(snr);
    SonarString.append(twoSpace);
    
    ScrString = "Scr: ";
    string scr = to_string(score);
    //To check if the score needs to be added leading zeros
    switch (scr.length()) {
        case 6:
            break;
        case 5:
            scr.insert(0, "0");
            break;
        case 4:
            scr.insert(0, "0");
            scr.insert(1, "0");
            break;
        case 3:
            scr.insert(0, "0");
            scr.insert(1, "0");
            scr.insert(2, "0");
            break;
        case 2:
            scr.insert(0, "0");
            scr.insert(1, "0");
            scr.insert(2, "0");
            scr.insert(3, "0");
            break;
        case 1:
            scr.insert(0, "0");
            scr.insert(1, "0");
            scr.insert(2, "0");
            scr.insert(3, "0");
            scr.insert(4, "0");
            break;
        default:
            scr = "000000";
            break;
    }
    ScrString.append(scr);
    ScrString.append(twoSpace);
    
    s.append(twoSpace);
    s.append(LvlString);
    s.append(LivesString);
    s.append(HlthString);
    s.append(WtrString);
    s.append(GldString);
    s.append(OilString);
    s.append(SonarString);
    s.append(ScrString);
    
    return s;
}

void StudentWorld:: setDisplayText()
{
    int level = getLevel();
    int lives = GameWorld::getLives();
    int health = this->myPlayer->getLife() * 10;
    int squirts = this->myPlayer->getWaterCount();
    int gold = this->myPlayer->getGold();
    int barrelsLeft = getOil();
    int sonar = this->myPlayer->getCharges();
    int score = getScore();
    
    string s = StatFormat(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
    
    setGameStatText(s);
}

 //Detecting AND removing ice at row/column according to x, y and direction OF ICEMAN.
bool StudentWorld::iceRemover(int const &x, int const &y, int const &Direction) {
	bool flag = false;
	switch (Direction) {
	case ActorsBase::up:
		for (int i = 0; i < 4; i++) {
			if (iceObjects[x + i][y + 4] != nullptr) {
				iceObjects[x + i][y + 4]->setVisible(false);
				iceObjects[x + i][y + 4].reset();
				iceObjects[x + i][y + 4] = nullptr;
				flag = true;
                playSound(SOUND_DIG);
			}
		}
		break;
	case ActorsBase::down:
		for (int i = 0; i < 4; i++) {
			if (iceObjects[x + i][y - 1] != nullptr) {
				iceObjects[x + i][y - 1]->setVisible(false);
				iceObjects[x + i][y - 1].reset();
				iceObjects[x + i][y - 1] = nullptr;

				flag = true;
				playSound(SOUND_DIG);
			}
		}
		break;
	case ActorsBase::right:
		for (int i = 0; i < 4; i++) {
			if (iceObjects[x + 4][y + i] != nullptr) {
				iceObjects[x + 4][y + i]->setVisible(false);
				iceObjects[x + 4][y + i].reset();
				iceObjects[x + 4][y + i] = nullptr;
				flag = true;
				playSound(SOUND_DIG);
			}
		}
		break;
	case ActorsBase::left:
		for (int i = 0; i < 4; i++) {
			if (iceObjects[x - 1][y + i] != nullptr) {
				iceObjects[x - 1][y + i]->setVisible(false);
				iceObjects[x - 1][y + i].reset();
				iceObjects[x - 1][y + i] = nullptr;
				flag = true;
				playSound(SOUND_DIG);
			}
		}
		break;
	}
	return flag;
}

void StudentWorld::boulderIceRemover(int const &x, int const &y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			iceObjects[x + i][y + j].reset();
			iceObjects[x + i][y + j] = nullptr;
		}
	}
}

void StudentWorld::squirtCreator(const int &posX, const int &posY, ActorsBase::Direction direction) {
	if (!isIcy(posX, posY, direction))
		myPlayers.emplace_back(new Squirt(posX, posY, direction, this));
}

// Checks a given X and Y of an object (and the whole 4 cubes of an actor's image rubric) if contains ice.
bool StudentWorld::isIcy(const int& x, const int& y, const ActorsBase::Direction& dir) {
	if (x < 0 || x>60 || y < 0 || y>60)
		return true;
	switch (dir) {
	case ActorsBase::up:
	case ActorsBase::right:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (iceObjects[x + i][y+j] != nullptr)
					return true;
			}
		}
		break;
	case ActorsBase::down:
		for (int i = 0; i < 4; i++) {
			if (iceObjects[x + i][y] != nullptr)
				return true;
		}
		break;
	case ActorsBase::left:
		for (int j = 0; j < 4; j++) {
			if (iceObjects[x][y + j] != nullptr)
				return true;
		}
		break;
	}
	return false;
}

// Returns true is a certain point on the field HAS ice in it.
bool StudentWorld::isIcy(const int& x, const int& y) {
	if (iceObjects[x][y] != nullptr)
		return true;
	return false;
}

// Protester checks if iceman is within range
bool StudentWorld::isIcemanInPosition(const int& x, const int& y) {
	if (myPlayer->getX() == x && myPlayer->getY() == y)
		return true;
	return false;
}

// Returns a weak_ptr to Iceman in order to manipulate by other actors in game.
std::weak_ptr<Iceman> StudentWorld::getIceman() {
	std::weak_ptr<Iceman> wp1 = myPlayer;
	return wp1;
}

// Sets all actors within r = sqrt
void StudentWorld::setVisibleRadius12(int x, int y) {
	for (auto ptr_to_actor:myPlayers) {
		// Distance formula. If the true distance between Iceman and the object is 12 or less (d = sqrt ((x1-x2)^2 + (y1-y2)^2) <= 12)
		if (sqrt(pow(ptr_to_actor->getX() - x, 2) + pow(ptr_to_actor->getY() - y, 2) <= 12)) {
			ptr_to_actor->setVisible(true);
		}
	}
}


// checks if a point has a golden nugget around it (4X4) and returns a pointer to it
std::shared_ptr<GoldNugget> StudentWorld::isSquareOccupiedByGoldenNugget(const int& x, const int& y, const ActorsBase::Direction& dir) {
    for (auto ptr_to_actor : myPlayers) {
		if (ptr_to_actor->isGoldNugget()) {
			switch (dir) {
			case ActorsBase::up:
				for (int i = -3; i < 4; i++) {
					if ((ptr_to_actor->getX() + i) == x && (ptr_to_actor->getY() + 4) == y)
                    {
                        std::shared_ptr<GoldNugget> ptr_to_GoldNugget = dynamic_pointer_cast<GoldNugget>(ptr_to_actor);
						return ptr_to_GoldNugget;
                    }
				}
				break;
			case ActorsBase::down:
				for (int i = -3; i < 4; i++) {
					if ((ptr_to_actor->getX() + i) == x && (ptr_to_actor->getY() - 1) == y)
                    {
                        std::shared_ptr<GoldNugget> ptr_to_GoldNugget = dynamic_pointer_cast<GoldNugget>(ptr_to_actor);
                        return ptr_to_GoldNugget;
                    }
				}
				break;
			case ActorsBase::right:
				for (int i = -3; i < 4; i++) {
					if ((ptr_to_actor->getX() + 4) == x && (ptr_to_actor->getY() + i) == y)
                    {
                        std::shared_ptr<GoldNugget> ptr_to_GoldNugget = dynamic_pointer_cast<GoldNugget>(ptr_to_actor);
                        return ptr_to_GoldNugget;
                    }
				}
				break;
			case ActorsBase::left:
				for (int i = -3; i < 4; i++) {
					if ((ptr_to_actor->getX() - 1) == x && (ptr_to_actor->getY() + i) == y)
                    {
                        std::shared_ptr<GoldNugget> ptr_to_GoldNugget = dynamic_pointer_cast<GoldNugget>(ptr_to_actor);
                        return ptr_to_GoldNugget;
                    }
				}
				break;
			}
		}
	}
	return nullptr;
}


// checks if a point has a Protester around it (4X4) and returns a pointer to it
std::shared_ptr<Protester> StudentWorld::isSquareOccupiedByProtester(const int& x, const int& y) {
    for (auto ptr_to_actor : myPlayers) {
        if (ptr_to_actor->isProtester()) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if ((ptr_to_actor->getX() == x + i) && (ptr_to_actor->getY() == y + j))
                    {
                        std::shared_ptr<Protester> ptr_to_protester = dynamic_pointer_cast<Protester>(ptr_to_actor);
                        return ptr_to_protester;
                    }
                }
            }
        }
    }
    return nullptr;
}

std::shared_ptr<HardcoreProtester> StudentWorld::isSquareOccupiedByHardcoreProtester(const int& x, const int& y) {
	for (auto ptr_to_actor : myPlayers) {
		if (ptr_to_actor->isHardcoreProtester()) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if ((ptr_to_actor->getX() == x + i) && (ptr_to_actor->getY() == y + j))
					{
                        std::shared_ptr<HardcoreProtester> ptr_to_hardcoreprotester = dynamic_pointer_cast<HardcoreProtester>(ptr_to_actor);
						return ptr_to_hardcoreprotester;
					}
				}
			}
		}
	}
	return nullptr;
}



bool StudentWorld::isSquareOccupiedByBoulder(const int& x, const int& y, const ActorsBase::Direction& dir) {
    for (auto ptr_to_actor : myPlayers) {
        if (ptr_to_actor->isBoulder()) {
            switch (dir) {
                case ActorsBase::down:
                    for (int i = -3; i < 4; i++) {
                        if ((ptr_to_actor->getX() + i) == x && (ptr_to_actor->getY() + 4) == y)
                            return true;
                    }
                    break;
                case ActorsBase::up:
                    for (int i = -3; i < 4; i++) {
                        if ((ptr_to_actor->getX() + i) == x && (ptr_to_actor->getY() - 4) == y)
                            return true;
                    }
                    break;
                case ActorsBase::left:
                    for (int i = -3; i < 4; i++) {
                        if ((ptr_to_actor->getX() + 4) == x && (ptr_to_actor->getY() + i) == y)
                            return true;
                    }
                    break;
                case ActorsBase::right:
                    for (int i = -3; i < 4; i++) {
                        if ((ptr_to_actor->getX() - 4) == x && (ptr_to_actor->getY() + i) == y)
                            return true;
                    }
                    break;
            }
        }
    }
    return false;
}

//Clean up all the actors that are "dead"
void StudentWorld::garbageCollector()
{
    for(auto it = myPlayers.begin(); it != myPlayers.end();)
    {
		if ((*it)->getLife() <= 0) {
			if ((*it)->isProtester() || (*it)->isHardcoreProtester())
				trueNumberOfProtestersAtField--;
			it = myPlayers.erase(it);
		}
        else
            ++it;
    }
}

void StudentWorld::dropNugget(const int& x, const int& y)
{
    std::shared_ptr<Iceman> ptr_to_Iceman = getIceman().lock();
    
    myPlayers.emplace_back(new GoldNugget(x, y, this, false));
    ptr_to_Iceman->setGold(-1);
    
}

//////////////////////////////////////////////////
// Find the shortest path
//////////////////////////////////////////////////

// This function will create a 2D - map of the field. Every empty spot will be denoted with -1, and a taken one with 9999.
int** StudentWorld::trackingMapCreator() {
    // Create a dynamically allocated 2D array called fieldmap.
    int** fieldMap = new int*[64];
    for (int i = 0; i < 64; ++i)
        fieldMap[i] = new int[64];
    
    // Iterate through iceObjects, and for every spot occupied with ice, fill in the fieldMap with a value of 99999 (unworthy of walking through).
    // Otherwise, the spot will get a value of -1: an empty spot you can walk through.
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 64; y++) {
            if (iceObjects[x][y] != nullptr) {
                fieldMap[x][y] = 99999;
            }
            else
                fieldMap[x][y] = -1;
        }
    }
    
    // Boulders are another object that players aren't allowed to walk through. Iterate theough myPlayers vector to see where there are boulders.
    // For the entire boulder block (4X4) - fill in with 99999 (unwalkable).
    
    for (auto actor : myPlayers) {
        // If actor is a boulder
        if (actor->isBoulder()) {
            // Get its x,y coordinates and input fieldMap with unwalkable ints (99999) the entire square.
            for (int x = actor->getX(); x < actor->getX() + 4; x++) {
                for (int y = actor->getY(); y < actor->getY() + 4; y++) {
                    fieldMap[x][y] = 99999;
                }
            }
        }
    }
    return fieldMap;
}
