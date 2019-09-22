#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>

//  CHECKES IF A STEP IS ALLOWED BY AN ACTOR.
bool thisStepIsAllowed(const int &x, const int &y) {
    if (x >= 0 && x < 61 && y >= 0 && y < 61)
        return true;
    else return false;
}

bool thisStepIsAllowed(const int& x,const int& y, const ActorsBase::Direction& dir) {
    switch (dir) {
    case ActorsBase::up:
        if (y + 1 < 61)
            return true;
        break;
    case ActorsBase::down:
        if (y - 1 >= 0)
            return true;
        break;
    case ActorsBase::right:
        if (x + 1 < 61)
            return true;
        break;
    case ActorsBase::left:
        if (x - 1 >= 0)
            return true;
        break;
    }
    return false;
}

bool ActorsBase::isInRangeOfFour(const int& x, const int& y)
{
    double radius;
    radius = sqrt(pow(x - getX(), 2) + pow(y - getY(), 2));
    if(radius <= 4)
    {
        setVisible(true);
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
//                                    ICEMAN  
//////////////////////////////////////////////////////////////////////////////////////////////
void Iceman::doSomething() {
    // If the player is alive
    if (!isAlive()) {
        return;
    }
    // Then check if currently at a spot that occupies Ice. If does, remove ice and make a sound.
    else {
        int ch;
        if (getWorld()->getKey(ch)) {
            // If the user hit a key this tick, then:
            switch (ch) {
            case KEY_PRESS_ESCAPE:
                    setLife(0);
                break;
            case KEY_PRESS_SPACE:
                if (getWaterCount()) {
                    setWater(-1);
                    getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    int newX = getX(), newY = getY();
                    switch (getDirection()) {
                    case up:
                        newY += 1;
                        break;
                    case down:
                        newY -= 1;
                        break;
                    case right:
                        newX += 1;
                        break;
                    case left:
                        newX -= 1;
                        break;
                    }
                    if (thisStepIsAllowed(newX, newY) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), getDirection())) {
                        getWorld()->squirtCreator(newX, newY, Iceman::getDirection());
                    }
                }
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up)
                    setDirection(up);
                else {
                    if (!getWorld()->isSquareOccupiedByBoulder(getX(), getY(), up) && thisStepIsAllowed(getX(), getY() + 1)) { // If my next step isnt occupied by a boulder AND isn't out of field
                        getWorld()->iceRemover(getX(), getY(), up);
                        moveTo(getX(), getY() + 1);
                    }
                }
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                    setDirection(down);
                else {
                    if (!getWorld()->isSquareOccupiedByBoulder(getX(), getY(), down) && thisStepIsAllowed(getX(), getY() - 1)) { // If my next step isnt occupied by a boulder AND isn't out of field
                        getWorld()->iceRemover(getX(), getY(), down);
                        moveTo(getX(), getY() - 1);
                    }
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                    setDirection(right);
                else {
                    if (!getWorld()->isSquareOccupiedByBoulder(getX(), getY(), right) && thisStepIsAllowed(getX() + 1, getY())) { // If my next step isnt occupied by a boulder AND isn't out of field
                        getWorld()->iceRemover(getX(), getY(), right);
                        moveTo(getX() + 1, getY());
                    }
                }
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                    setDirection(left);
                else {
                    if (!getWorld()->isSquareOccupiedByBoulder(getX(), getY(), left) && thisStepIsAllowed(getX() - 1, getY())) { // If my next step isnt occupied by a boulder AND isn't out of field
                        getWorld()->iceRemover(getX(), getY(), left);
                        moveTo(getX() - 1, getY());
                    }
                }
                break;
            case 'z':
            case 'Z':
                if (getCharges()) {
                    setCharges(-1);
                    getWorld()->setVisibleRadius12(getX(), getY()); // SETS ALL NON MOVING ACTORS WITHIN RADIUS OF 12 TO TRUE.
                }
                break;
            case KEY_PRESS_TAB:
                if (getGold()) {
                    getWorld()->dropNugget(getX(), getY());
                }
                break;
            }
        }
    }
}

void Iceman::getAnnoyed(int damage) {
    Iceman::setLife(getLife() - damage);
    if (getLife() <= 0) {
		setLife(0);
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                    Gold Nugget
//////////////////////////////////////////////////////////////////////////////////////////////
GoldNugget::GoldNugget(int StartX, int StartY, StudentWorld* newWorld, bool flag) : ActorsBase(IID_GOLD, StartX, StartY, right, 1, 2, 1, newWorld)
{   
	setVisible(false);
    //true if pickupable by iceman, false if pickupable by protesters
    setPickupability(flag);
    if(!flag)
        setTForGold(80);
}

void GoldNugget::isInRangeOfThreeForGNOfIceMan(const int&x, const int &y, bool flag)
{
    double radius;
    radius = sqrt(pow(x - getX(), 2) + pow(y - getY(), 2));
            
    if(radius <= 3 && flag)
    {
        std::shared_ptr<Iceman> ptr_to_iceman = getWorld()->getIceman().lock();
        setLife(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(10);
        ptr_to_iceman->setGold(1);
    }
}

void GoldNugget::doSomething()
{
    if(!isAlive())
        return;
    else
    {
        std::shared_ptr<Iceman> ptr_to_Iceman = getWorld()->getIceman().lock();
        if(!isVisible() && isInRangeOfFour(ptr_to_Iceman->getX(), ptr_to_Iceman->getY()))
        {
            return;
        }
        if(!getPickupability())
        {
            decreT();
            if(getTForGold() == 0)
                setLife(0);
        }
        isInRangeOfThreeForGNOfIceMan(ptr_to_Iceman->getX(), ptr_to_Iceman->getY(), getPickupability());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                             Water Pool
//////////////////////////////////////////////////////////////////////////////////////////////
WaterPool::WaterPool(int startX, int startY, StudentWorld* newWorld) : ActorsBase(IID_WATER_POOL, startX, startY, right, 1, 2, 1, newWorld)
{
    setVisible(true);
    int level = getWorld()->getLevel();
    setTForWater(std::max(100, 300 - 10*level));
}

void WaterPool::isInRangeOfThree(const int& x, const int& y)
{
    double radius;
    radius = sqrt(pow(x - getX(), 2) + pow(y - getY(), 2));
    
    if(radius <= 3)
    {
        std::shared_ptr<Iceman> ptr_to_iceman = getWorld()->getIceman().lock();
        setLife(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(100);
        ptr_to_iceman->setWater(5);
    }
}

void WaterPool::doSomething()
{
    if(!isAlive())
        return;
    else
    {
        decreT(); // decrease the time
        std::shared_ptr<Iceman> ptr_to_Iceman = getWorld()->getIceman().lock();
        isInRangeOfThree(ptr_to_Iceman->getX(), ptr_to_Iceman->getY());
        
        if(getTForWater() == 0)
            setLife(0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                             Sonar Kit
//////////////////////////////////////////////////////////////////////////////////////////////
SonarKit::SonarKit(StudentWorld* newWorld) : ActorsBase(IID_SONAR, 0, 60, right, 1, 2, 1, newWorld)
{
    setVisible(true);
    int level = getWorld()->getLevel();
    setTForSonar(std::max(100, 300 - 10*level));
}

void SonarKit::isInRangeOfThree(const int& x, const int& y)
{
    double radius;
    radius = sqrt(pow(x - getX(), 2) + pow(y - getY(), 2));
    
    if(radius <= 3)
    {
        std::shared_ptr<Iceman> ptr_to_iceman = getWorld()->getIceman().lock();
        setLife(0);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(75);
        ptr_to_iceman->setCharges(1);
    }
}

void SonarKit::doSomething()
{
    if(!isAlive())
        return;
    else
    {
        decreT(); // decrease the time
        std::shared_ptr<Iceman> ptr_to_Iceman = getWorld()->getIceman().lock();
        isInRangeOfThree(ptr_to_Iceman->getX(), ptr_to_Iceman->getY());
        
        if(getTForSonar() == 0)
            setLife(0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                    SQUIRT
//////////////////////////////////////////////////////////////////////////////////////////////

void Squirt::doSomething() {
    if (distanceLeft != 0) {
        distanceLeft -= 1;
        switch (getDirection()) {
        case up:
            // The range for isOccupied is 4 x's and one y, which is exactly one step above the squirt.
            if (!getWorld()->isIcy(getX(), getY() + 1, up) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), up))
                Squirt::moveTo(getX(), getY() + 1);
            break;
        case down:
            if (!getWorld()->isIcy(getX(), getY() - 1, down) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), down))
                Squirt::moveTo(getX(), getY() - 1);
            break;
        case right:
            if (!getWorld()->isIcy(getX() + 1, getY(), right) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), right))
                Squirt::moveTo(getX() + 1, getY());
            break;
        case left:
            if (!getWorld()->isIcy(getX() - 1, getY(), left) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), left))
                Squirt::moveTo(getX() - 1, getY());
            break;
        }
        std::shared_ptr<ActorsBase> ptr_to_protester = getWorld()->isSquareOccupiedByProtester(getX(), getY());
        std::shared_ptr<ActorsBase> ptr_to_hardcoreprotester = getWorld()->isSquareOccupiedByHardcoreProtester(getX(), getY());
        if (ptr_to_protester != nullptr) {
            ptr_to_protester->getAnnoyed(2);
            Squirt::setLife(0);
        }
        if (ptr_to_hardcoreprotester != nullptr) {
            ptr_to_hardcoreprotester->getAnnoyed(2);
            Squirt::setLife(0);
        }
    }
    else
    {
        Squirt::setLife(0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                             Barrel of oil
//////////////////////////////////////////////////////////////////////////////////////////////
void oil::isInRangeOfThree(const int& x, const int& y)
{
    double radius;
    radius = sqrt(pow(x - getX(), 2) + pow(y - getY(), 2));
    
    if(radius <= 3)
    {
        std::shared_ptr<Iceman> ptr_to_iceman = getWorld()->getIceman().lock();
        setLife(0);
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->setOil();
    }
}

void oil::doSomething()
{
    if(!isAlive())
        return;
    else
    {
        std::shared_ptr<Iceman> ptr_to_Iceman = getWorld()->getIceman().lock();
        if(!isVisible() && isInRangeOfFour(ptr_to_Iceman->getX(), ptr_to_Iceman->getY()))
        {
            return;
        }
        
        isInRangeOfThree(ptr_to_Iceman->getX(), ptr_to_Iceman->getY());
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
//                                    BOULDER
//////////////////////////////////////////////////////////////////////////////////////////////
void Boulder::AnnoyingBoulder(const int& x, const int& y)
{
    std::shared_ptr<Iceman> ptr_to_Iceman = getWorld()->getIceman().lock();
    std::shared_ptr<Protester> ptr_to_protester = getWorld()->isSquareOccupiedByProtester(x, y);
    std::shared_ptr<HardcoreProtester> ptr_to_hardcoreprotester = getWorld()->isSquareOccupiedByHardcoreProtester(x, y);
    
    if(ptr_to_protester)
    {
        //get the euclidian distance between the boulder and the protesters
        double radius1 = sqrt(pow(static_cast<double>(x) - static_cast<double>(ptr_to_protester->getX()), 2) + pow(static_cast<double>(y) - static_cast<double>(ptr_to_protester->getY()), 2));
        //annoy the protesters if it's within radius of 3
        if(radius1 <= 3)
        {
            ptr_to_protester->getAnnoyed(100);
            getWorld()->increaseScore(500);
        }
    }
    
    if(ptr_to_hardcoreprotester)
    {
        //get the euclidian distance between the boulder and the hardcore protesters
        double radius2 = sqrt(pow(static_cast<double>(x) - static_cast<double>(ptr_to_hardcoreprotester->getX()), 2) + pow(static_cast<double>(y) - static_cast<double>(ptr_to_hardcoreprotester->getY()), 2));
        //annoy the hardcore protesters if it's within radius of 3
        if(radius2 <= 3)
        {
            ptr_to_hardcoreprotester->getAnnoyed(100);
            getWorld()->increaseScore(500);
        }
    }
    
    //get the euclidian distance between the boulder and the actors
    double radius3 = sqrt(pow(static_cast<double>(x) - static_cast<double>(ptr_to_Iceman->getX()), 2) + pow(static_cast<double>(y) - static_cast<double>(ptr_to_Iceman->getY()), 2));
    //annoy the hardcore protesters if it's within radius of 3
    if(radius3 <= 3)
    {
        ptr_to_Iceman->getAnnoyed(100);
        getWorld()->increaseScore(500);
    }
}

bool Boulder::isStable()
{
    bool flag = false;
    
    for(int i = 0; i < 4; i++)
    {
        if(getWorld()->isIcy(Boulder::getX() + i, Boulder::getY() - 1))
            flag = true;
    }
    
    return flag;
}

void Boulder::isFalling()
{
    moveTo(getX(), getY() - 1);
    
    AnnoyingBoulder(getX(), getY());
    
    if(getY() < 0 || isStable() || getWorld()->isSquareOccupiedByBoulder(getX(), getY(), down))
    {
        setLife(0);
        setVisible(false);
    }
    
}

void Boulder::isWaiting()
{
    if(tickstowait != 0)
        tickstowait--;
    else
    {
        isFalling();
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
}

void Boulder::doSomething(){
    if(!isAlive())
    {
        setVisible(false);
        return;
    }
    else
    {
        if(!isStable())
        {
            isWaiting();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//                                    REGULAR PROTESTER
//////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
// image = IID_ Protester, hitPoints = 5;
Protester::Protester(int image, int hitPoints, StudentWorld* newWorld) : ActorsBase(image, 60, 60, left, 1, 0, hitPoints, newWorld)
{
    setVisible(true);
    numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    int maxMoves = 3 - ((int)getWorld()->getLevel() / 4);
    ticksToWaitBetweenMoves = std::max(0, maxMoves);
}

// Protester - doSomething
void Protester::doSomething() {
    // Step 1
    if (!isAlive()) {
        return;
    }
    // If it's alive, even if it has to rest, it hasnt made a perp move, therefore increment ticksSinceLastPerpendicularTurn.
    ticksSinceLastPerpendicularTurn++;
    // Step 2
    if (ticksToWaitBetweenMoves > 0) {
        ticksToWaitBetweenMoves--;
        return;
    }
    // Step 3
    if (leave_the_oil_field_state) {
        if (getX() == 60 && getY() == 60) {
            setLife(0);
            setVisible(false);
        }
        else {
            if (exiting == false) {
                int** mapOfExit = getWorld()->trackingMapCreator();
                shotestPathToDestination(mapOfExit, 60, 60);
                shortestPathToExit = convertShotestPathToStackOfPoints(mapOfExit, 60, 60); // Exit is at 60, 60.
                // Deleting the entire map to prevent memory leak.
                for (int i = 0; i < 64; ++i)
                    delete[] mapOfExit[i];
                delete[] mapOfExit;
                exiting = true;
                walkToGoalUsingFastestPath(shortestPathToExit);
            }
            else {
                walkToGoalUsingFastestPath(shortestPathToExit);
                int maxMoves = 2 - ((int)getWorld()->getLevel() / 4);
                ticksToWaitBetweenMoves = std::max(0, maxMoves);
            }
        }
        return;
    }
    // decrementing the dontshoutmeter
    dontShoutMeter--;
    // Step 4
    if (icemanWithinAnnoyanceDistance()) 
    {
        if (dontShoutMeter <= 0) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getIceman().lock()->getAnnoyed(2); // takes 2 hit point off iceman
            dontShoutMeter = 15;
            ticksToWaitBetweenMoves = 8; // Added by Michael, to give Iceman sufficient time to escape after a hit.
        }
        return;
    }
    int maxMoves = 3 - ((int)getWorld()->getLevel() / 4);
    ticksToWaitBetweenMoves = std::max(0, maxMoves);
    // Step 5
    Direction where_is_iceman;
    if (directLineOfSightToIceman(where_is_iceman)) {
        WalkAtThisDirection(where_is_iceman);
        numSquaresToMoveInCurrentDirection = 0;
        return;
    }
    // If non of the first 5 cases happened, before step 6 we're decrementing the amount of squares to move.
    numSquaresToMoveInCurrentDirection--;
    // Step 6 - otherwise, if not supposed to turn, either move in same direction or chose a new direction randomly
    if (numSquaresToMoveInCurrentDirection <= 0) {
        // Randomize a direction, repeat until was able to move at a certain direction.
        bool step_is_possible = false;
        do {
            Direction randir = static_cast<Direction> (rand() % right + up);
            step_is_possible = WalkAtThisDirection(randir);
        } while (step_is_possible == false);
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    }
    // Step 7 - If its time for the protester to turn, turn
    else if (ticksSinceLastPerpendicularTurn >= 200 && canChangePerpendicularDirection()) {
        // Set a new perpendicular direction
        switch (getDirection()) {
        case right:
        case left:
            if (isViable(down))
                setDirection(down);
            else
                setDirection(up);
            break;
        case up:
        case down:
            if (isViable(left))
                setDirection(left);
            else
                setDirection(right);
            break;
            }
        // After new direction is set, reinitialize the amount of ticks until next turn and set numSqauresInCurrentDirection to random.
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
        ticksSinceLastPerpendicularTurn = 0;
        }
    // step 8 and 9 - try to walk at current direction. If can't - set steps in that direction to zero.
    if (!WalkAtThisDirection(getDirection()))
        numSquaresToMoveInCurrentDirection = 0;
    
    if(getWorld()->isSquareOccupiedByBoulder(getX(), getY(), getDirection()))
        getAnnoyed(100);
    
    tryToPickGoldNugget();
}

void Protester::getAnnoyed(int damage) {
    if (leave_the_oil_field_state)
        return;
    setLife(getLife() - damage);
    if (getLife() > 0) {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        int maxStun = 100 - getWorld()->getLevel() * 10;
        ticksToWaitBetweenMoves = std::max(50, maxStun);
    }
    else if (getLife() <= 0) {
        setLife(100);
        leave_the_oil_field_state = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        ticksToWaitBetweenMoves = 0;
        getWorld()->increaseScore(100);
    }
}

void Protester::tryToPickGoldNugget() {
    std::shared_ptr<GoldNugget> Pp = getWorld()->isSquareOccupiedByGoldenNugget(getX(), getY(), getDirection());
    if (Pp != nullptr && !Pp->getPickupability()) {
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(25);
        Pp->setLife(0);
        leave_the_oil_field_state = true;
    }
}


/////////////////////////////////////////////////////////////
//                Hardcore Protester
/////////////////////////////////////////////////////////////

// Constructor
HardcoreProtester::HardcoreProtester(StudentWorld* newWorld) : Protester(IID_HARD_CORE_PROTESTER, 20, newWorld)
{
    setVisible(true);
    numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    int maxMoves = 3 - ((int)getWorld()->getLevel() / 4);
    ticksToWaitBetweenMoves = std::max(0, maxMoves);
}

// Protester - doSomething
void HardcoreProtester::doSomething() {
    // Step 1
    if (!isAlive()) {
        return;
    }
    ticksSinceLastPerpendicularTurn++;
    // Step 2
    if (ticksToWaitBetweenMoves > 0) {
        ticksToWaitBetweenMoves--;
        return;
    }
    // Step 3
    if (leave_the_oil_field_state) {
        if (getX() == 60 && getY() == 60) {
            setLife(0);
            setVisible(false);
        }
        else {
            if (exiting == false) {
                int** mapOfExit = getWorld()->trackingMapCreator();
                shotestPathToDestination(mapOfExit, 60, 60);
                shortestPathToExit = convertShotestPathToStackOfPoints(mapOfExit, 60, 60); // Exit is at 60, 60.
                                                                                           // Deleting the entire map to prevent memory leak.
                for (int i = 0; i < 64; ++i)
                    delete[] mapOfExit[i];
                delete[] mapOfExit;
                exiting = true;
                walkToGoalUsingFastestPath(shortestPathToExit);
            }
            else {
                walkToGoalUsingFastestPath(shortestPathToExit);
                int maxMoves = 2 - ((int)getWorld()->getLevel() / 4);
                ticksToWaitBetweenMoves = std::max(0, maxMoves);
            }
        }
        return;
    }
    // decrementing the dontshoutmeter
    dontShoutMeter--;
    int maxMoves = 3 - ((int)getWorld()->getLevel() / 4);
    ticksToWaitBetweenMoves = std::max(0, maxMoves);

    // Step 4
    if (icemanWithinAnnoyanceDistance())
    {
        if (dontShoutMeter <= 0) {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->getIceman().lock()->getAnnoyed(2); // takes 2 hit point off iceman
            dontShoutMeter = 15;
            ticksToWaitBetweenMoves = 10;
        }
        return;
    }
//    else {
////             Step 5 - Track iceman's celluar phone signals.
//            int M = (16 + getWorld()->getLevel() * 2);
//
//            //////////////////////////////////////
//            // TODO: Make sure DFS algorithm is only queueing and dequeueing numbers and not pointers.
//            //////////////////////////////////////
//
//            int** mapToFindIceman = getWorld()->trackingMapCreator();
//            shotestPathToDestination(mapToFindIceman, getWorld()->getIceman().lock()->getX(), getWorld()->getIceman().lock()->getY());
//			int numOfSteps = mapToFindIceman[getWorld()->getIceman().lock()->getX()][getWorld()->getIceman().lock()->getY()];
//            // Deleting the entire map to prevent memory leak.
//            for (int i = 0; i < 64; i++)
//                delete[] mapToFindIceman[i];
//            delete[] mapToFindIceman;
//
//        if (numOfSteps <= M) {
//			shortestPathToIceman = convertShotestPathToStackOfPoints(mapToFindIceman, getWorld()->getIceman().lock()->getX(), getWorld()->getIceman().lock()->getY());
//            walkToGoalUsingFastestPath(shortestPathToIceman);
//			HardcoreProtester::tryToPickGoldNugget();
//            return;
//        }
//    }

    // Step 6 - If direct line of sight to Iceman
    Direction where_is_iceman;
    if (directLineOfSightToIceman(where_is_iceman)) {
        WalkAtThisDirection(where_is_iceman);
        numSquaresToMoveInCurrentDirection = 0;
        return;
    }
    // If non of the first 6 cases happened, before step 7 we're decrementing the amount of squares to move.
    numSquaresToMoveInCurrentDirection--;
    // Step 7 - otherwise, if not supposed to turn, either move in same direction or chose a new direction randomly
    if (numSquaresToMoveInCurrentDirection <= 0) {
        // Randomize a direction, repeat until was able to move at a certain direction.
        bool step_is_possible = false;
        do {
            Direction randir = static_cast<Direction> (rand() % right + up);
            step_is_possible = WalkAtThisDirection(randir);
        } while (step_is_possible == false);
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
    }
    // Step 8 - If its time for the protester to turn, turn
    else if (ticksSinceLastPerpendicularTurn >= 200 && canChangePerpendicularDirection()) {
        // Set a new perpendicular direction
        switch (getDirection()) {
        case right:
        case left:
            if (isViable(down))
                setDirection(down);
            else
                setDirection(up);
            break;
        case up:
        case down:
            if (isViable(left))
                setDirection(left);
            else
                setDirection(right);
            break;
        }
        // After new direction is set, reinitialize the amount of ticks until next turn and set numSqauresInCurrentDirection to random.
        numSquaresToMoveInCurrentDirection = rand() % 53 + 8;
        ticksSinceLastPerpendicularTurn = 0;
    }
    // step 8 and 9 - try to walk at current direction. If can't - set steps in that direction to zero.
    if (!WalkAtThisDirection(getDirection()))
        numSquaresToMoveInCurrentDirection = 0;
    
    if(getWorld()->isSquareOccupiedByBoulder(getX(), getY(), getDirection()))
        getAnnoyed(100);
        
	HardcoreProtester::tryToPickGoldNugget();
}
void HardcoreProtester::getAnnoyed(int damage) {
    if (leave_the_oil_field_state)
        return;
    setLife(getLife() - damage);
    if (getLife() > 0) {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        int maxStun = 100 - getWorld()->getLevel() * 10;
        ticksToWaitBetweenMoves = std::max(50, maxStun);
    }
    else if (getLife() <= 0) {
        setLife(100);
        leave_the_oil_field_state = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        ticksToWaitBetweenMoves = 0;
        getWorld()->increaseScore(250);
    }
}

void HardcoreProtester::tryToPickGoldNugget(){
    std::shared_ptr<GoldNugget> Pp = getWorld()->isSquareOccupiedByGoldenNugget(getX(), getY(), getDirection());
    if (Pp != nullptr && !Pp->getPickupability()) {
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(50);
        Pp->setLife(0);
        int maxStun = 100 - getWorld()->getLevel() * 10;
        ticksToWaitBetweenMoves = std::max(50, maxStun);
    }
}


/////////////////////////////////////////////////////////////
//                Protester Helper Functions
/////////////////////////////////////////////////////////////

bool Protester::icemanWithinAnnoyanceDistance() {
    // If Iceman is above or under protester
    if (getWorld()->getIceman().lock()->getX() == getX()) {
        // If Iceman above protester and within radius
        if (getWorld()->getIceman().lock()->getY() - getY() <= 4 && getWorld()->getIceman().lock()->getY() - getY() > 0) {
            setDirection(up);
            return true;
        }
        // If Iceman below protester and within radius
        else if (getWorld()->getIceman().lock()->getY() - getY() >= -4 && getWorld()->getIceman().lock()->getY() - getY() < 0) {
            setDirection(down);
            return true;
        }
    }
    // else, if Iceman is on the right / left of protester
    else if ((getWorld()->getIceman().lock()->getY() == getY())) {
        //If iceman directly to protester's right
        if (getWorld()->getIceman().lock()->getX() - getX() <= 4 && getWorld()->getIceman().lock()->getX() - getX() > 0) {
            setDirection(right);
            return true;
        }
        // If directly to his left
        else if (getWorld()->getIceman().lock()->getX() - getX() >= -4 && getWorld()->getIceman().lock()->getX() - getX() < 0) {
            setDirection(left);
            return true;
        }
    }
    return false;
}


// Checks if the protester is directly above or aside the Iceman, and if there's no ice or boulder in the way.
bool Protester::directLineOfSightToIceman(Direction& to_iceman) {
    //if X coordinate for iceman and protester is the same (means iceman is above or below protester)
    if (getWorld()->getIceman().lock()->getX() == getX()) {
        // If iceman directly above protester, check if going all the way up to iceman is viable.
        if (getWorld()->getIceman().lock()->getY() > getY()) {
            for (int i = getY(); i < getWorld()->getIceman().lock()->getY(); i++) {
                if (!isViable(getX(), i, up))
                    return false;
            }
            to_iceman = up;
            return true;
        }
        else {
            // If iceman directly below protester, chech if going all the way down is possible.
            for (int i = getY(); i > getWorld()->getIceman().lock()->getY(); i--) {
                if (!isViable(getX(), i, down))
                    return false;
            }
            to_iceman = down;
            return true;
        }
    }
    // if Y coordinate for iceman and protester is the same (means iceman is to the right or to the left of protester)
    if (getWorld()->getIceman().lock()->getY() == getY()) {
        // if iceman to protester's right
        if (getWorld()->getIceman().lock()->getX() > getX()) {
            for (int i = getX(); i < getWorld()->getIceman().lock()->getX(); i++) {
                if (!isViable(i, getY(), right))
                    return false;
            }
            to_iceman = right;
            return true;
        }
        // if iceman to protester's left, then
        else {
            for (int i = getX(); i > getWorld()->getIceman().lock()->getX(); i--) {
                if (!isViable(i, getY(), left))
                    return false;
            }
            to_iceman = left;
            return true;
        }
    }
    // If neither x not y coordinates are similar, return false.
    return false;
}

// Move the protester on step in the direction he's pointing to
bool Protester::WalkAtThisDirection(const Direction& dir) {
    // Check if the next step isn't filled with ice.
    if (isViable(dir)) {
        if (getDirection() != dir)
            setDirection(dir);
        switch (dir) {
        case up:
            moveTo(getX(), getY() + 1);
            tryToPickGoldNugget();
            return true;
            break;
        case down:
            moveTo(getX(), getY() - 1);
            tryToPickGoldNugget();
            return true;
            break;
        case right:
            moveTo(getX() + 1, getY());
            tryToPickGoldNugget();
            return true;
            break;
        case left:
            moveTo(getX() - 1, getY());
            tryToPickGoldNugget();
            return true;
            break;
        }
    }
    return false;
}

// Checks if the protester is at a perpendicular intersection, returns true.
bool Protester::canChangePerpendicularDirection() {
    // If protester directs right or left
    if (getDirection() == right || getDirection() == left) {
        // and up or down isn't occupied by ice or boulder
        if (isViable(up) || isViable(down))
            return true;
    }
    else
    {
        if (isViable(right) || isViable(left))
            return true;
    }
    return false;
}

// Return if the protester can move at a certain direction.
bool Protester::isViable(const Direction& dir) {
    switch (dir) {
    case up:
        if (thisStepIsAllowed(getX(), getY(), up) && !getWorld()->isIcy(getX(), getY() + 1, up) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), up))
            return true;
        break;
    case down:
        if (thisStepIsAllowed(getX(), getY(), down) && !getWorld()->isIcy(getX(), getY() - 1, down) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), down))
            return true;
        break;
    case right:
        if (thisStepIsAllowed(getX(), getY(), right) && !getWorld()->isIcy(getX() + 1, getY(), right) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), right))
            return true;
        break;
    case left:
        if (thisStepIsAllowed(getX(), getY(), left) && !getWorld()->isIcy(getX() - 1, getY(), left) && !getWorld()->isSquareOccupiedByBoulder(getX(), getY(), left))
            return true;
        break;
    }
    return false;
}

// Is viable for a range - checks if a certain distance between Iceman and Protester is Viable
bool Protester::isViable(const int& x, const int&y, const Direction& dir) {
    switch (dir) {
    case up:
        // The range for isOccupied is 4 x's and one y, which is exactly one step above the squirt.
        if (thisStepIsAllowed(getX(), getY(), dir) && !getWorld()->isIcy(x, y + 1, up) && !getWorld()->isSquareOccupiedByBoulder(x,y,up))
            return true;
        break;
    case down:
        if (thisStepIsAllowed(getX(), getY(), dir) && !getWorld()->isIcy(x, y - 1, down) && !getWorld()->isSquareOccupiedByBoulder(x, y, down))
            return true;
        break;
    case right:
        if (thisStepIsAllowed(getX(), getY(), dir) && !getWorld()->isIcy(x + 1, y, right) && !getWorld()->isSquareOccupiedByBoulder(x, y, right))
            return true;
        break;
    case left:
        if (thisStepIsAllowed(getX(), getY(), dir) && !getWorld()->isIcy(x - 1, y, left) && !getWorld()->isSquareOccupiedByBoulder(x, y, left))
            return true;
        break;
    }
    return false;
}


//////////////////////////////////////////////////
// Find the shortest path
//////////////////////////////////////////////////

// Function will determine if a step is possible to be done on the field map
bool isSafe(int** M, int x, int y) {
    if (x >= 0 && x < 64 && y >= 0 && y < 64 && M[x][y] == -1)
        return true;
    return false;
}

// TODO: DON'T FORGET TO DELETE FIELD AT THE END OF THE OPERATION.
void Protester::shotestPathToDestination(int** field, const int& desX, const int& desY){
    bool flag = true;
    int position = 0;
    field[getX()][getY()] = position;
    queue<std::shared_ptr<Point>> myQueue;
    std::shared_ptr<Point> currentPos(std::make_shared<Point>());
    std::shared_ptr<Point> guess;
    currentPos->m_x = getX();
    currentPos->m_y = getY();
    myQueue.push(currentPos);
    while (!myQueue.empty() && flag) {
        guess = myQueue.front();
        myQueue.pop();
        position = field[guess->m_x][guess->m_y] + 1;
        if (guess->m_x == desX && guess->m_y == desY) {
            field[guess->m_x][guess->m_y] = position - 1;
            flag = false;
            continue;
        }
        // If a move upwards is possible (all four spots)
        if (isSafe(field, guess->m_x, guess->m_y + 4) && 
            isSafe(field, guess->m_x+1, guess->m_y + 4) && 
            isSafe(field, guess->m_x+2, guess->m_y + 4) &&
            isSafe(field, guess->m_x+3, guess->m_y + 4) &&
            isSafe(field, guess->m_x, guess->m_y + 1)) { // Make sure it doesn't go back to where it started.
            std::shared_ptr<Point> newPos(std::make_shared<Point>());
            newPos->m_x = guess->m_x;
            newPos->m_y = guess->m_y+1;
            myQueue.push(newPos);
            field[guess->m_x][guess->m_y + 1] = position;
        }
        // DOWNWARDS
        if (isSafe(field, guess->m_x, guess->m_y - 1) &&
            isSafe(field, guess->m_x + 1, guess->m_y - 1) &&
            isSafe(field, guess->m_x + 2, guess->m_y - 1) &&
            isSafe(field, guess->m_x + 3, guess->m_y - 1)) {
            std::shared_ptr<Point> newPos(std::make_shared<Point>());
            newPos->m_x = guess->m_x;
            newPos->m_y = guess->m_y-1;
            myQueue.push(newPos);
            field[guess->m_x][guess->m_y - 1] = position;
        }
        // RIGHT
        if (isSafe(field, guess->m_x + 4, guess->m_y) &&
            isSafe(field, guess->m_x + 4, guess->m_y + 1) &&
            isSafe(field, guess->m_x + 4, guess->m_y + 2) &&
            isSafe(field, guess->m_x + 4, guess->m_y + 3) &&
            isSafe(field, guess->m_x + 1, guess->m_y)) { // Make sure it doesn't go back to where it started.
            std::shared_ptr<Point> newPos(std::make_shared<Point>());
            newPos->m_x = guess->m_x+1;
            newPos->m_y = guess->m_y;
            myQueue.push(newPos);
            field[guess->m_x+1][guess->m_y] = position;
        }
        // Left
        if (isSafe(field, guess->m_x - 1, guess->m_y) &&
            isSafe(field, guess->m_x - 1, guess->m_y + 1) &&
            isSafe(field, guess->m_x - 1, guess->m_y + 2) &&
            isSafe(field, guess->m_x - 1, guess->m_y + 3)) { // Make sure it doesn't go back to where it started.
            std::shared_ptr<Point> newPos(std::make_shared<Point>());
            newPos->m_x = guess->m_x - 1;
            newPos->m_y = guess->m_y;
            myQueue.push(newPos);
            field[guess->m_x - 1][guess->m_y] = position;
        }
    }
}
// Converts the field calculated (that contains quickest path) to a stack of shortest steps. Argument list - enter the two destination coordinates.
std::stack<Protester::Point> Protester::convertShotestPathToStackOfPoints(int** field, int indexX, int indexY) {
    stack<Point> resultPath;
    Point currentPoint;
    currentPoint.m_x = indexX;
    currentPoint.m_y = indexY;
    resultPath.push(currentPoint);
    while (field[indexX][indexY] != 1) {
        // If the next step (from destination to starting point) is upwards, put that coordinate in the stack and stop current iteration.
        if (field[indexX][indexY + 1] == field[indexX][indexY] - 1) {
            indexY = indexY + 1;
            Point newPointToPushInStack;
            newPointToPushInStack.m_x = indexX;
            newPointToPushInStack.m_y = indexY;
            resultPath.push(newPointToPushInStack);
        }
        // if moving up did not progress us, try moving downwards. MAKE SURE aren't dereferencing a nullptr by checking y is greater than 0.
        else if (indexY>0 && field[indexX][indexY - 1] == field[indexX][indexY] - 1) {
            indexY = indexY - 1;
            Point newPointToPushInStack;
            newPointToPushInStack.m_x = indexX;
            newPointToPushInStack.m_y = indexY;
            resultPath.push(newPointToPushInStack);
        }
        // if moving up/down didn't work, try moving  to the right.
        else if (field[indexX+1][indexY] == field[indexX][indexY] - 1) {
            indexX = indexX + 1;
            Point newPointToPushInStack;
            newPointToPushInStack.m_x = indexX;
            newPointToPushInStack.m_y = indexY;
            resultPath.push(newPointToPushInStack);
        }
        // try moving to the left. MAKE SURE possible - x>0.
        else if (indexX>0 && field[indexX - 1][indexY] == field[indexX][indexY] - 1) {
            indexX = indexX - 1;
            Point newPointToPushInStack;
            newPointToPushInStack.m_x = indexX;
            newPointToPushInStack.m_y = indexY;
            resultPath.push(newPointToPushInStack);
        }
    }
    return resultPath;
}

void Protester::walkToGoalUsingFastestPath(std::stack<Protester::Point>& path) {
    if (!path.empty()) {
        if (path.top().m_x > getX() && getDirection() != right)
            setDirection(right);
        else if (path.top().m_x < getX() && getDirection() != left)
            setDirection(left);
        else if (path.top().m_y > getY() && getDirection() != up)
            setDirection(up);
        else if (path.top().m_y < getY() && getDirection() != down)
            setDirection(down);
        moveTo(path.top().m_x, path.top().m_y);
        path.pop();
    }
}
