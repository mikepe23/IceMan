#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
class StudentWorld;


// Virtual abstract class for all actors, including IceMan and Ice.
class ActorsBase: public GraphObject {
public:
	ActorsBase(int imageID, int startX, int startY, Direction startDirection, float pictureSize, unsigned int depth, int life, StudentWorld* newWorld) : GraphObject(imageID, startX, startY, startDirection, pictureSize, depth), m_life (life), myWorld(newWorld) {};
	virtual ~ActorsBase(){}
	virtual void doSomething() = 0;		// Iceman movement dervied from keys, ice doesn't do anything, all other actors partially randomised.
	virtual void getAnnoyed(int damage)=0;		// will cause a decrease in life.
	bool isAlive() const { if (m_life > 0) return true; else return false; }
	int getLife() const { return m_life; }
	void setLife(int new_life) { m_life = new_life; }
	StudentWorld* getWorld() { return myWorld; }

    bool isInRangeOfFour(const int& x, const int& y);
    
	// Getter functions of the type of actor:
	virtual bool isProtester() { return false; };
	virtual bool isHardcoreProtester() { return false; };
	virtual bool isBoulder() { return false; };
	virtual bool isGoldNugget() { return false; };
    virtual bool isOil() { return false; };
    virtual bool isSonarKit() { return false; };
    virtual bool isWaterPool() { return false; };

	/*GraphObject has a moveTo(int x, int y) that needs to be called when one of the actors needs to move. (moveTo (getX()+1, get y) will move right one column, needs to 
	called when UP is pressed for the Iceman. - X = Column, Y = Row.*/
	//setDirection - If a direction is pressed != getDirection(), then set direction and dont moveTo.

private:
	int m_life;
	StudentWorld* myWorld;
};

class Iceman : public ActorsBase {
public:
	Iceman(StudentWorld* newWorld) : ActorsBase(IID_PLAYER, 30, 60, right, 1, 0, 10, newWorld) { setVisible(true);  }
	~Iceman() {};
	virtual void doSomething() override;
	virtual void getAnnoyed(int damage) override;
	int getWaterCount() const { return water_gun_squirts; }
	int getCharges() const { return sonar_charges; }
	int getGold() const { return gold_nuggets; }
	void setWater(int waterAmount) { water_gun_squirts += waterAmount; }
	void setCharges(int sonarAmount) { sonar_charges += sonarAmount; }
	void setGold(int goldAmount) { gold_nuggets += goldAmount; }
    void setOil(int oilAmount) { }

private: 
	int water_gun_squirts = 5;
	int sonar_charges = 1;
	int gold_nuggets = 0;

};

class Squirt : public ActorsBase {
public:
	Squirt(int startX, int startY, Direction startDirection, StudentWorld *newWorld) : ActorsBase(IID_WATER_SPURT, startX, startY, startDirection, 1, 1, 1, newWorld) { setVisible(true); };
	virtual void doSomething() override;
	virtual void getAnnoyed(int damage) {}
private:
	int distanceLeft = 4;
};

class Protester : public ActorsBase {
public:
	Protester(int image, int hitPoints, StudentWorld* newWorld);
	virtual void doSomething() override;
	virtual void getAnnoyed(int damage) override;
	virtual bool icemanWithinAnnoyanceDistance();
	virtual bool directLineOfSightToIceman(Direction& to_iceman);
    virtual bool WalkAtThisDirection(const Direction& dir);
	virtual bool canChangePerpendicularDirection();
	virtual bool isViable(const Direction& dir);
	virtual bool isViable(const int& x, const int&y, const Direction& dir);
    virtual void tryToPickGoldNugget();
	virtual bool isProtester() { return true; };
    int setTicksToWaitBetweenMoves(int a) {return ticksToWaitBetweenMoves = a;};

    struct Point{
        int m_x;
        int m_y;
    };
    
    virtual void shotestPathToDestination(int** field, const int& desX, const int& dexY);
    std::stack<Protester::Point> convertShotestPathToStackOfPoints(int** field, int indexX, int indexY);
    void walkToGoalUsingFastestPath(std::stack<Protester::Point>& path);
protected:
	int numSquaresToMoveInCurrentDirection = 0;
	int ticksToWaitBetweenMoves = 0;
	bool leave_the_oil_field_state = false;
	int dontShoutMeter = 0;
	int ticksSinceLastPerpendicularTurn = 0;

	bool exiting = false;
	std::stack<Point> shortestPathToExit;
};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* newWorld);
	virtual void doSomething() override;
	virtual void getAnnoyed(int damage) override;

	virtual void tryToPickGoldNugget() override;
	virtual bool isProtester() override { return false; };
	virtual bool isHardcoreProtester() override { return true; };

private:
	std::stack<Point> shortestPathToIceman;

};

class Boulder : public ActorsBase {
public:
    Boulder(int startX, int startY, StudentWorld* newWorld) : ActorsBase(IID_BOULDER, startX, startY, down, 1, 1, 1, newWorld) { setVisible(true);}
    virtual void doSomething();
	virtual void getAnnoyed(int damage) override {}
    bool isStable();
    void isWaiting();
    void isFalling();
    virtual bool isBoulder() { return true; };
    void AnnoyingBoulder(const int& x, const int& y);
    
private:
    int tickstowait = 30;
};

class Ice : public ActorsBase {
public:
	Ice(int startX, int startY, StudentWorld* newWorld) : ActorsBase(IID_ICE, startX, startY, right, .25, 3, 1, newWorld) { setVisible(true); }
	virtual void doSomething() {}
	virtual void getAnnoyed(int damage) override {}

private:

};

class oil : public ActorsBase{
public:
    oil(int startX, int startY, StudentWorld* newWorld) : ActorsBase(IID_BARREL, startX, startY, right, 1, 2, 1, newWorld) { setVisible(false);}
    virtual void doSomething();
    virtual void getAnnoyed(int damage) override {}
    virtual bool isOil() { return true; };
    void isInRangeOfThree(const int& x, const int& y);
    
};

class GoldNugget : public ActorsBase {
public:
    GoldNugget(int startX, int startY, StudentWorld* newWorld, bool flag);
    virtual void doSomething();
	virtual void getAnnoyed(int damage) override {}
	virtual bool isGoldNugget() { return true; };
    void isInRangeOfThreeForGNOfIceMan(const int& x, const int& y, bool flag);
    
    void setTForGold(int a) {TForGold = a;}
    void decreT() {TForGold--;};
    int getTForGold() {return TForGold;}
    //Set the status if gold nugget is pickup-able by iceman or protesters
    void setPickupability (bool flag) { Pickupability = flag;};
    bool getPickupability(){ return Pickupability;};
private:
    bool Pickupability; //True if pickup-able by Iceman, false if pickup-able by protesters
    int TForGold; // Remaining time for the Sonar Kit
};

class SonarKit : public ActorsBase {
public:
    SonarKit(StudentWorld* newWorld);
    virtual void doSomething();
    virtual void getAnnoyed(int damage) override {}
    virtual bool isSonarKit() { return true; };
    void isInRangeOfThree(const int& x, const int& y);
    
    void setTForSonar(int a) {TForSonar = a;}
    void decreT() {TForSonar--;};
    int getTForSonar() {return TForSonar;}
private:
    int TForSonar; // Remaining time for the Sonar Kit
};

class WaterPool : public ActorsBase {
public:
    WaterPool(int startX, int startY, StudentWorld* newWorld);
    virtual void doSomething();
    virtual void getAnnoyed(int damage) override {}
    virtual bool isWaterPool() { return true; };
    void isInRangeOfThree(const int& x, const int& y);
    
    void setTForWater(int a) {TForWater = a;}
    void decreT() {TForWater--;}
    int getTForWater() {return TForWater;}
private:
    int TForWater; // REmaining time for the Water Pool
};

#endif // ACTOR_H_
