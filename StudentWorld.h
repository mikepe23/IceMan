#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <algorithm>
#include <utility>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	virtual int init()
	{
		srand(time(NULL));
		targetNumberOfProtestersAtThisLevel = min(15, (int)(2+getLevel()*1.5)); // Initialize max num of protesters for this level.
		maxNumberOfTicksUntilAProtesterShouldBeCreated = max(25, (int)(200-getLevel()));
		probabilityOfHardcoreProtester = min(90, (int)(getLevel() * 10 + 30));
		probabilityOfEitherSonarKitOrWaterPool = getLevel() * 25 + 300;


		myPlayer = std::make_shared<Iceman>(this);

		for (int x = 0; x < 64; x++)
		{
			for (int y = 0; y < 64; y++) {
				if (((x > 29 && x < 34) && y > 3) || y > 59) {
					iceObjects[x][y] = nullptr;
				}
				else
				{
					std::unique_ptr<Ice> temp(new Ice(x, y, this));
					iceObjects[x][y] = std::move(temp);
				}
			}
		}

		// Create a map of the field. Wherever an item cannot be created, fill in the spot with 999. ONLY spots where the X,Y coordinates of a newly
		// created objects CAN be placed, will be filled with -1 (otherwise, 999). I DONT need to use smart pointers, since NOT dynamically allocated.
		int initMap[64][64];
		for (int i = 0; i < 64; i++) {
			for (int j = 0; j < 64; j++) {
				if ((i > 25 && i < 35) || i > 60 || j > 55)
					initMap[i][j] = 999;
				else 
					initMap[i][j] = -1;
			}
		}
		///////////////////////////////////////////
		// Real Conditions for constructing 
		// an actor
		//////////////////////////////////////////
		int B = std::min((int)(getLevel() / 2 + 2), 9);
		int G = std::max((int)(5-(getLevel() / 2)), 2);
		int L = std::min((int)(2 + getLevel()), 21);
		/////////////////////////////////////////////
		// 1 reg protester at the begginig of stage
		////////////////////////////////////////////
		myPlayers.emplace_back(new Protester(IID_PROTESTER, 5, this));
		////////////////////////////////////
		// Condition to create boulders
		////////////////////////////////////
		while (B) {
			int startX = 0, startY = 0;
			bool cant_be_instantiated = false; // Assume the spot is empty and a boulder can be instantiated
			do {
				startX = rand() % 61;
				startY = rand() % 37 + 20;
				// If the spot is occupied, randomise a different spot.
				if (initMap[startX][startY] != -1)
					cant_be_instantiated = true;
				else 
					cant_be_instantiated = false;
			} while (cant_be_instantiated);
			myPlayers.emplace_back(new Boulder(startX, startY, this));
			boulderIceRemover(startX, startY);
			// Update the initMap for new place that objects cant be instantiated.
			for (int i = -6; i < 7; i++) {
				for (int j = -6; j < 7; j++) {
					if (startX + i > 0 && startY + j > 0 && startX+i <64 && startY+j<64) {
						// All spots within a radius of 6 of the instantiation point will be labeled as occupied (999)
						if (sqrt(pow(i, 2) + pow(j, 2)) <= 6)
							initMap[startX+i][startY+j] = 999;
					}
				}
			}
			B--;
		}
		////////////////////////////////////
		// Condition to create Gold nuggets
		////////////////////////////////////
		while (G) {
			int startX = 0, startY = 0;
			bool cant_be_instantiated = false; // Assume the spot is empty and a boulder can be instantiated
			do {
				startX = rand() % 61;
				startY = rand() % 57;
				// If the spot is occupied, randomise a different spot.
				if (initMap[startX][startY] != -1)
					cant_be_instantiated = true;
				else
					cant_be_instantiated = false;
			} while (cant_be_instantiated);
			myPlayers.emplace_back(new GoldNugget(startX, startY, this, true));
			// Update the initMap for new place that objects cant be instantiated.
			for (int i = -6; i < 7; i++) {
				for (int j = -6; j < 7; j++) {
					if (startX + i > 0 && startY + j > 0 && startX + i <64 && startY + j<64) {
						// All spots within a radius of 6 of the instantiation point will be labeled as occupied (999)
						if (sqrt(pow(i, 2) + pow(j, 2)) <= 6)
							initMap[startX + i][startY + j] = 999;
					}
				}
			}
			G--;
		}
		////////////////////////////////////
		// Condition to create Oil Barrels
		////////////////////////////////////
		while (L) {
			int startX = 0, startY = 0;
			bool cant_be_instantiated = false; // Assume the spot is empty and a boulder can be instantiated
			do {
				startX = rand() % 61;
				startY = rand() % 57;
				// If the spot is occupied, randomise a different spot.
				if (initMap[startX][startY] != -1)
					cant_be_instantiated = true;
				else
					cant_be_instantiated = false;
			} while (cant_be_instantiated);
			myPlayers.emplace_back(new oil(startX, startY, this));
			num_of_oil++;
			// Update the initMap for new place that objects cant be instantiated.
			for (int i = -6; i < 7; i++) {
				for (int j = -6; j < 7; j++) {
					if (startX + i > 0 && startY + j > 0 && startX + i <64 && startY + j<64) {
						// All spots within a radius of 6 of the instantiation point will be labeled as occupied (999)
						if (sqrt(pow(i, 2) + pow(j, 2)) <= 6)
							initMap[startX + i][startY + j] = 999;
					}
				}
			}
			L--;
		}
        
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		// Update Display every tick.
        setDisplayText();

		//////////////////////////////////
		// Add Actors To The Field
		//////////////////////////////////
		// Add ticks to protesters tick count:
		numberOfTicksPassedSinceLastProtesterAdded++;

		//////////////////////////////////
		// Protesters
		/////////////////////////////////
		// Add a protester; If sufficient ticks have passed AND true number of protesters isn't exceeded the max:
		if (numberOfTicksPassedSinceLastProtesterAdded >= maxNumberOfTicksUntilAProtesterShouldBeCreated
			&& trueNumberOfProtestersAtField<targetNumberOfProtestersAtThisLevel) {
			// Decide if hardcore or regular:
			int regularOrHardcore = rand() % 100 + 1;
			if (regularOrHardcore < probabilityOfHardcoreProtester) {
				myPlayers.emplace_back(new HardcoreProtester(this));
			}
			else {
				myPlayers.emplace_back(new Protester(IID_PROTESTER,5,this));
			}
			numberOfTicksPassedSinceLastProtesterAdded = 0;
			trueNumberOfProtestersAtField++;
		}

		////////////////////////////////////
		// Gold Nuggets and Sonar Kits
		///////////////////////////////////
		int shouldAGoodieBeCreated = rand() % probabilityOfEitherSonarKitOrWaterPool;
		if (shouldAGoodieBeCreated == 1) {
			int isASonarKitShouldBeCreated = rand() % 5 + 1;
			if (isASonarKitShouldBeCreated == 1) {
				myPlayers.emplace_back(new SonarKit(this));
			}
			else {
				int newXforWaterPool;
				int newYforWaterPool;
				do {
					newXforWaterPool = rand() % 61;
					newYforWaterPool = rand() % 61;
					// Repeat if the square is filled with ice, until finds a suitable coordinate.
				} while (isIcy(newXforWaterPool, newYforWaterPool, ActorsBase::up));
				myPlayers.emplace_back(new WaterPool(newXforWaterPool, newYforWaterPool, this));
			}
		}

		myPlayer->doSomething();
		// Running doSomething every tick for every player.
		// Set to invisible every object that reaches 0 life;
		for (auto dummy : myPlayers) {
			if (dummy->getLife() > 0)
				dummy->doSomething();
			else
				dummy->setVisible(false);
		}
        garbageCollector();
        
		if (myPlayer->getLife() <= 0) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
        
        if(getOil() <= 0)
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
        
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		for (auto it = myPlayers.begin(); it != myPlayers.end();){
			it = myPlayers.erase(it);
		}
		num_of_oil = 0;
		numberOfTicksPassedSinceLastProtesterAdded = 0;
		maxNumberOfTicksUntilAProtesterShouldBeCreated = 0;
		targetNumberOfProtestersAtThisLevel = 0;
		trueNumberOfProtestersAtField = 1;
		probabilityOfHardcoreProtester = 0;
		probabilityOfEitherSonarKitOrWaterPool = 0;
	}

	void setVisibleRadius12(int x, int y);

	// Checks if there is an actor occupies the field within a certain range of a coordiante, Returns a pointer to the actor.
	//std::shared_ptr<ActorsBase> isOccupied(int const &x, int const &y, int const &rangeX, int const &rangeY);

	// Removes ice as the Iceman moves in the field.
	bool iceRemover(int const &x, int const &y, int const &Direction);
    bool isIcemanInPosition(const int& x, const int& y);
    bool isIcy(const int& x, const int& y);

	// Removes ice for boulder
	void boulderIceRemover(int const &x, int const &y);
    
	// Creates an actor "Squirt" when Player presses space bar, emplaces it in actors vector.  
	void squirtCreator(const int &posX, const int &posY, ActorsBase::Direction direction);

    bool isIcy(const int& x, const int& y, const ActorsBase::Direction& dir);
    
    // Checks if a golden nugget occupies a square (of which to protester might occupy)
    std::shared_ptr<GoldNugget> isSquareOccupiedByGoldenNugget(const int& x, const int& y, const ActorsBase::Direction& dir);
    // Checks if occupied by protester
    std::shared_ptr<Protester> isSquareOccupiedByProtester(const int& x, const int& y);
	std::shared_ptr<HardcoreProtester> isSquareOccupiedByHardcoreProtester(const int& x, const int& y);
    // Checks if occupied by protester
    bool isSquareOccupiedByBoulder(const int& x, const int& y, const ActorsBase::Direction& dir);
        
    void setOil() {num_of_oil--;}
    int getOil() { return num_of_oil;}
    
    
    //Let Iceman drop the nugget
    void dropNugget(const int& x, const int& y);
    
    //Clean up all the actors that are "dead"
    void garbageCollector();
    
    std::weak_ptr<Iceman> getIceman();
    
	// Stats presentation.
	string StatFormat(int,int,int,int,int,int,int,int);
    void setDisplayText();

	// map creator to find the nearest exit.
	int** trackingMapCreator();

private:
	std::shared_ptr<Iceman> myPlayer;
	std::vector<std::shared_ptr<ActorsBase>> myPlayers;
	std::unique_ptr<Ice> iceObjects[64][64];
    int num_of_oil = 0;
	int numberOfTicksPassedSinceLastProtesterAdded = 0;
	int maxNumberOfTicksUntilAProtesterShouldBeCreated = 0;
	int targetNumberOfProtestersAtThisLevel = 0;
	int trueNumberOfProtestersAtField = 1;
	int probabilityOfHardcoreProtester = 0;
	int probabilityOfEitherSonarKitOrWaterPool = 0;

};

#endif // STUDENTWORLD_H_


