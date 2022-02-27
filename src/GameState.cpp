/*
        STARFLIGHT - THE LOST COLONY
        GameState.cpp - ?
        Author: ?
        Date: ?
*/

#include <cmath>
#include <map>
#include <stdexcept>
#include <utility>

#include "Archive.h"
#include "DataMgr.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "Point2D.h"
#include "QuestMgr.h"
#include "Util.h"

using namespace std;

ALLEGRO_DEBUG_CHANNEL("GameState")

// SHIP CLASS
std::string
convertClassTypeToString(int num) {
    switch (num) {
    case 0:
        return "None";
    case 1:
        return "Class 1";
    case 2:
        return "Class 2";
    case 3:
        return "Class 3";
    case 4:
        return "Class 4";
    case 5:
        return "Class 5";
    case 6:
        return "Class 6";
    case 7:
        return "Class 7";
    case 8:
        return "Class 8";
    case 9:
        return "Class 9";
    }
    return "Error in convertClassTypeToString()";
}

Ship::Ship()
    : maxEngineClass(0), maxShieldClass(0), maxArmorClass(0),
      maxMissileLauncherClass(0), maxLaserClass(0) {
    Reset();
}

Ship::~Ship() {}

void
Ship::initializeRepair() {
    ALLEGRO_DEBUG("Calling Ship::initializeRepair()\n");

    // roll the minerals that will be used for repair
    for (int i = 0; i < NUM_REPAIR_PARTS; i++) {
        switch (sfrand() % 5) {
        case 0:
            repairMinerals[i] = ITEM_COBALT;
            break;
        case 1:
            repairMinerals[i] = ITEM_MOLYBDENUM;
            break;
        case 2:
            repairMinerals[i] = ITEM_ALUMINUM;
            break;
        case 3:
            repairMinerals[i] = ITEM_TITANIUM;
            break;
        case 4:
            repairMinerals[i] = ITEM_SILICA;
            break;
        default:
            ALLEGRO_ASSERT(0);
        }
    }

    // set the repair counters so that the player will need to pay the next time
    // he start repairs
    for (int i = 0; i < NUM_REPAIR_PARTS; i++) {
        repairCounters[i] = MAX_REPAIR_COUNT;
    }

    // stop all repair, if needed
    partInRepair = PART_NONE;
}

// accessors
std::string
Ship::getName() const {
    return name;
}
int
Ship::getCargoPodCount() const {
    return cargoPodCount;
}
int
Ship::getEngineClass() const {
    return engineClass;
}
int
Ship::getShieldClass() const {
    return shieldClass;
}
int
Ship::getArmorClass() const {
    return armorClass;
}
int
Ship::getMissileLauncherClass() const {
    return missileLauncherClass;
}

float
Ship::get_maximum_velocity() const {
    double topspeed = 1.0;

    int engine = getEngineClass();
    if (engine < 1 || engine > 6) {
        engine = 1;
        ALLEGRO_DEBUG("*** Error in PlayerShipSprite::getMaximumVelocity: "
                      "Engine class is invalid");
    }

    switch (engine) {
    case 1:
        topspeed = g_game->getGlobalNumber("ENGINE1_TOPSPEED");
        break;
    case 2:
        topspeed = g_game->getGlobalNumber("ENGINE2_TOPSPEED");
        break;
    case 3:
        topspeed = g_game->getGlobalNumber("ENGINE3_TOPSPEED");
        break;
    case 4:
        topspeed = g_game->getGlobalNumber("ENGINE4_TOPSPEED");
        break;
    case 5:
        topspeed = g_game->getGlobalNumber("ENGINE5_TOPSPEED");
        break;
    case 6:
        topspeed = g_game->getGlobalNumber("ENGINE6_TOPSPEED");
        break;
    }

    return topspeed;
}

float
Ship::get_fuel_usage(float distance) const {
    float max_vel = get_maximum_velocity();

    distance = fabs(distance);

    return distance * max_vel / 100 / getEngineClass();
}

int
Ship::getTotalSpace() {
    return cargoPodCount * POD_CAPACITY;
}

int
Ship::getOccupiedSpace() {
    Item item;
    int numItems, occupiedSpace = 0;

    // loop over the inventory to get items count
    int numstacks = g_game->gameState->m_items.GetNumStacks();
    for (int i = 0; i < numstacks; i++) {
        g_game->gameState->m_items.GetStack(i, item, numItems);

        // artifacts do not take any space
        if (!item.IsArtifact())
            occupiedSpace += numItems;
    }

    return occupiedSpace;
}

int
Ship::getAvailableSpace() {
    int freeSpace = getTotalSpace() - getOccupiedSpace();

    return freeSpace;
}

int
Ship::getMissileLauncherDamage() {
    // return missile damage based on class
    switch (missileLauncherClass) {
    case 1:
        return g_game->getGlobalNumber("MISSILE1_DAMAGE");
        break;
    case 2:
        return g_game->getGlobalNumber("MISSILE2_DAMAGE");
        break;
    case 3:
        return g_game->getGlobalNumber("MISSILE3_DAMAGE");
        break;
    case 4:
        return g_game->getGlobalNumber("MISSILE4_DAMAGE");
        break;
    case 5:
        return g_game->getGlobalNumber("MISSILE5_DAMAGE");
        break;
    case 6:
        return g_game->getGlobalNumber("MISSILE6_DAMAGE");
        break;
    case 7:
        return g_game->getGlobalNumber("MISSILE7_DAMAGE");
        break;
    case 8:
        return g_game->getGlobalNumber("MISSILE8_DAMAGE");
        break;
    case 9:
        return g_game->getGlobalNumber("MISSILE9_DAMAGE");
        break;
    default:
        return 0;
    }
}

int
Ship::getMissileLauncherFiringRate() {
    // return missile firing rate based on class
    switch (missileLauncherClass) {
    case 1:
        return g_game->getGlobalNumber("MISSILE1_FIRERATE");
        break;
    case 2:
        return g_game->getGlobalNumber("MISSILE2_FIRERATE");
        break;
    case 3:
        return g_game->getGlobalNumber("MISSILE3_FIRERATE");
        break;
    case 4:
        return g_game->getGlobalNumber("MISSILE4_FIRERATE");
        break;
    case 5:
        return g_game->getGlobalNumber("MISSILE5_FIRERATE");
        break;
    case 6:
        return g_game->getGlobalNumber("MISSILE6_FIRERATE");
        break;
    case 7:
        return g_game->getGlobalNumber("MISSILE7_FIRERATE");
        break;
    case 8:
        return g_game->getGlobalNumber("MISSILE8_FIRERATE");
        break;
    case 9:
        return g_game->getGlobalNumber("MISSILE9_FIRERATE");
        break;
    default:
        return 0;
    }
}
int
Ship::getLaserClass() const {
    return laserClass;
}
int
Ship::getLaserDamage() {
    // return laser damage based on class
    switch (laserClass) {
    case 1:
        return g_game->getGlobalNumber("LASER1_DAMAGE");
        break;
    case 2:
        return g_game->getGlobalNumber("LASER2_DAMAGE");
        break;
    case 3:
        return g_game->getGlobalNumber("LASER3_DAMAGE");
        break;
    case 4:
        return g_game->getGlobalNumber("LASER4_DAMAGE");
        break;
    case 5:
        return g_game->getGlobalNumber("LASER5_DAMAGE");
        break;
    case 6:
        return g_game->getGlobalNumber("LASER6_DAMAGE");
        break;
    case 7:
        return g_game->getGlobalNumber("LASER7_DAMAGE");
        break;
    case 8:
        return g_game->getGlobalNumber("LASER8_DAMAGE");
        break;
    case 9:
        return g_game->getGlobalNumber("LASER9_DAMAGE");
        break;
    default:
        return 0;
    }
}

int
Ship::getLaserFiringRate() {
    // return laser firing rate based on class
    switch (laserClass) {
    case 1:
        return g_game->getGlobalNumber("LASER1_FIRERATE");
        break;
    case 2:
        return g_game->getGlobalNumber("LASER2_FIRERATE");
        break;
    case 3:
        return g_game->getGlobalNumber("LASER3_FIRERATE");
        break;
    case 4:
        return g_game->getGlobalNumber("LASER4_FIRERATE");
        break;
    case 5:
        return g_game->getGlobalNumber("LASER5_FIRERATE");
        break;
    case 6:
        return g_game->getGlobalNumber("LASER6_FIRERATE");
        break;
    case 7:
        return g_game->getGlobalNumber("LASER7_FIRERATE");
        break;
    case 8:
        return g_game->getGlobalNumber("LASER8_FIRERATE");
        break;
    case 9:
        return g_game->getGlobalNumber("LASER9_FIRERATE");
        break;
    default:
        return 0;
    }
}

float
Ship::getMaxArmorIntegrity() {
    switch (armorClass) {
    case 1:
        return g_game->getGlobalNumber("ARMOR1_STRENGTH");
        break;
    case 2:
        return g_game->getGlobalNumber("ARMOR2_STRENGTH");
        break;
    case 3:
        return g_game->getGlobalNumber("ARMOR3_STRENGTH");
        break;
    case 4:
        return g_game->getGlobalNumber("ARMOR4_STRENGTH");
        break;
    case 5:
        return g_game->getGlobalNumber("ARMOR5_STRENGTH");
        break;
    case 6:
        return g_game->getGlobalNumber("ARMOR6_STRENGTH");
        break;
    default:
        return 0;
    }
}

// shield capacity is the absorbing capacity of the shield, it is different from
// the shield generator integrity itself. it will slowly regenerate itself up to
// a maximum determined by the shield generator class and current integrity and
// only decrease as a result of taking damage during combat.
float
Ship::getMaxShieldCapacity() {
    switch (shieldClass) {
    case 1:
        return g_game->getGlobalNumber("SHIELD1_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 2:
        return g_game->getGlobalNumber("SHIELD2_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 3:
        return g_game->getGlobalNumber("SHIELD3_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 4:
        return g_game->getGlobalNumber("SHIELD4_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 5:
        return g_game->getGlobalNumber("SHIELD5_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 6:
        return g_game->getGlobalNumber("SHIELD6_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 7:
        return g_game->getGlobalNumber("SHIELD7_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    case 8:
        return g_game->getGlobalNumber("SHIELD8_STRENGTH") * shieldIntegrity
               / 100.0f;
        break;
    default:
        return 0;
    }
}

float
Ship::getHullIntegrity() const {
    return hullIntegrity;
}
float
Ship::getArmorIntegrity() const {
    return armorIntegrity;
}
float
Ship::getShieldIntegrity() const {
    return shieldIntegrity;
}
float
Ship::getShieldCapacity() const {
    return shieldCapacity;
}

float
Ship::getEngineIntegrity() const {
    return engineIntegrity;
}
float
Ship::getMissileLauncherIntegrity() const {
    return missileLauncherIntegrity;
}
float
Ship::getLaserIntegrity() const {
    return laserIntegrity;
}

std::string
Ship::getEngineClassString() const {
    return convertClassTypeToString(engineClass);
}
std::string
Ship::getShieldClassString() const {
    return convertClassTypeToString(shieldClass);
}
std::string
Ship::getArmorClassString() const {
    return convertClassTypeToString(armorClass);
}
std::string
Ship::getMissileLauncherClassString() const {
    return convertClassTypeToString(missileLauncherClass);
}
std::string
Ship::getLaserClassString() const {
    return convertClassTypeToString(laserClass);
}
bool
Ship::HaveEngines() const {
    return engineClass != NotInstalledType;
}

float
Ship::getFuel() {
    return fuelPercentage;
}

/**
    Standard consumption occurs in interplanetary space (iterations=1).
Interstellar should consume 4x this amount. Planet landing/takeoff should each
consume 10x. Remember, 1 cu-m Endurium will fill the fuel tank.
**/
void
Ship::ConsumeFuel(int iterations) {
    for (int n = 0; n < iterations; n++) {
        // consume fuel 0.1% / engine_class (higher class uses less fuel)
        float percent_amount =
            0.001f / g_game->gameState->m_ship.getEngineClass();

        g_game->gameState->m_ship.augFuel(-percent_amount);
    }
}

void
Ship::augFuel(float percentage) {
    fuelPercentage += percentage;
    capFuel();
}

void
Ship::capFuel() {
    if (fuelPercentage > 1.0f)
        fuelPercentage = 1.0f;
    else if (fuelPercentage < 0.0f)
        fuelPercentage = 0.0f;
}

int
Ship::getEnduriumOnBoard() {
    // get amount of endurium in cargo
    Item endurium;
    const int ITEM_ENDURIUM = 54;
    int amount = 0;
    g_game->gameState->m_items.Get_Item_By_ID(ITEM_ENDURIUM, endurium, amount);
    return amount;
}

void
Ship::injectEndurium() {
    // check endurium amount
    int number_of_endurium = getEnduriumOnBoard();
    if (number_of_endurium >= 1) {
        // reduce endurium
        number_of_endurium--;
        g_game->gameState->m_items.RemoveItems(54, 1);
        g_game->printout(
            "Consuming Endurium crystal... We have "
                + Util::ToString(number_of_endurium) + " left.",
            ORANGE,
            5000);

        // use it to fill the fuel tank
        g_game->gameState->m_ship.augFuel(1.0f);

        // notify CargoHold to update itself
        ALLEGRO_EVENT e = {
            .type = static_cast<unsigned int>(EVENT_CARGO_UPDATE)};
        g_game->broadcast_event(&e);
    } else
        g_game->printout("We have no Endurium!", ORANGE, 5000);
}

// mutators
void
Ship::setName(const string &initName) {
    name = initName;
}
void
Ship::setCargoPodCount(int initCargoPodCount) {
    cargoPodCount = initCargoPodCount;
}
void
Ship::cargoPodPlusPlus() {
    cargoPodCount++;
}
void
Ship::cargoPodMinusMinus() {
    cargoPodCount--;
}
void
Ship::setEngineClass(int initEngineClass) {
    engineClass = initEngineClass;
}
void
Ship::setShieldClass(int initShieldClass) {
    shieldClass = initShieldClass;
}
void
Ship::setArmorClass(int initArmorClass) {
    armorClass = initArmorClass;
}
void
Ship::setMissileLauncherClass(int initMissileLauncherClass) {
    missileLauncherClass = initMissileLauncherClass;
}
void
Ship::setLaserClass(int initLaserClass) {
    laserClass = initLaserClass;
}

void
Ship::setHullIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > 100.0f)
        value = 100.0f;
    hullIntegrity = value;
}
void
Ship::augHullIntegrity(float amount) {
    if (hullIntegrity + amount < 100)
        setHullIntegrity(hullIntegrity + amount);
    else
        setHullIntegrity(100);
}

void
Ship::setArmorIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > getMaxArmorIntegrity())
        value = getMaxArmorIntegrity();
    armorIntegrity = value;
}

void
Ship::setShieldIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > 100.0f)
        value = 100.0f;
    shieldIntegrity = value;
}
void
Ship::augShieldIntegrity(float amount) {
    if (shieldIntegrity + amount < 100)
        setShieldIntegrity(shieldIntegrity + amount);
    else
        setShieldIntegrity(100);
}

void
Ship::setShieldCapacity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > getMaxShieldCapacity())
        value = getMaxShieldCapacity();
    shieldCapacity = value;
}

void
Ship::setEngineIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > 100.0f)
        value = 100.0f;
    engineIntegrity = value;
}
void
Ship::augEngineIntegrity(float amount) {
    if (engineIntegrity + amount < 100)
        setEngineIntegrity(engineIntegrity + amount);
    else
        setEngineIntegrity(100);
}

void
Ship::setMissileLauncherIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > 100.0f)
        value = 100.0f;
    missileLauncherIntegrity = value;
}
void
Ship::augMissileLauncherIntegrity(float amount) {
    if (missileLauncherIntegrity + amount < 100)
        setMissileLauncherIntegrity(missileLauncherIntegrity + amount);
    else
        setMissileLauncherIntegrity(100);
}

void
Ship::setLaserIntegrity(float value) {
    if (value < 0.0f)
        value = 0.0f;
    if (value > 100.0f)
        value = 100.0f;
    laserIntegrity = value;
}
void
Ship::augLaserIntegrity(float amount) {
    if (laserIntegrity + amount < 100)
        setLaserIntegrity(laserIntegrity + amount);
    else
        setLaserIntegrity(100);
}

void
Ship::setMaxEngineClass(int engineClass) {
    ALLEGRO_ASSERT(engineClass >= 1 && engineClass <= 6);
    maxEngineClass = engineClass;
}
void
Ship::setMaxArmorClass(int armorClass) {
    ALLEGRO_ASSERT(armorClass >= 1 && armorClass <= 6);
    maxArmorClass = armorClass;
}
void
Ship::setMaxShieldClass(int shieldClass) {
    ALLEGRO_ASSERT(shieldClass >= 1 && shieldClass <= 8);
    maxShieldClass = shieldClass;
}
void
Ship::setMaxLaserClass(int laserClass) {
    ALLEGRO_ASSERT(laserClass >= 1 && laserClass <= 9);
    maxLaserClass = laserClass;
}
void
Ship::setMaxMissileLauncherClass(int missileLauncherClass) {
    ALLEGRO_ASSERT(missileLauncherClass >= 1 && missileLauncherClass <= 9);
    maxMissileLauncherClass = missileLauncherClass;
}

void
Ship::SendDistressSignal() {
    // calculate cost of rescue
    int starport_x = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_X") / 128;
    int starport_y = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_Y") / 128;
    Point2D starport_pos(starport_x, starport_y);
    double distance = Point2D::Distance(
        g_game->gameState->getHyperspaceCoordinates(), starport_pos);
    double cost = (5000 + (distance * 50.0));

    /* string message = */
    /*     "Myrrdan Port Authority has dispatched a tow ship to bring us in. ";
     */
    /* message += "The cost of the rescue is " + Util::ToString(cost) + " MU.";
     */
    /* g_game->ShowMessageBoxWindow( */
    /*     "", */
    /*     message, */
    /*     500, */
    /*     300, */
    /*     BLUE, */
    /*     SCREEN_WIDTH / 2, */
    /*     SCREEN_HEIGHT / 2, */
    /*     true, */
    /*     false); */

    // charge player's account for the tow
    g_game->gameState->m_credits -= cost;

    // return to starport
    g_game->set_vibration(0);
    g_game->LoadModule(MODULE_PORT);
}

// specials
void
Ship::Reset() {
    /*
    These properties are set in ModuleCaptainCreation based on profession
    NOTE: Actually ModuleCaptainCreation will call this function at the end of
    the creation process so the values set here are the definitive ones.
    Presumably this was not the original purpose of this function but it was
    thereafter hijacked to override ModuleCaptainCreation.
    */
    cargoPodCount = 0;
    engineClass = 0; // this will be upgraded with a tutorial mission
    shieldClass = 0;
    armorClass = 0;
    missileLauncherClass = 0;
    laserClass = 0;
    hullIntegrity = 100;
    armorIntegrity = 0;
    shieldIntegrity = 0;
    engineIntegrity = 100;
    missileLauncherIntegrity = 0;
    laserIntegrity = 0;
    hasTV = true;
    fuelPercentage = 1.0f;
}

InputArchive &
operator>>(InputArchive &ar, Ship &ship) {
    string class_name = string(Ship::class_name);
    int schema = 0;

    ship.Reset();

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: " + class_name + " version "
                + std::to_string(load_schema));
    }

    ar >> ship.name;
    ar >> ship.cargoPodCount;
    ar >> ship.engineClass;
    ar >> ship.shieldClass;
    ar >> ship.armorClass;
    ar >> ship.missileLauncherClass;
    ar >> ship.laserClass;
    ar >> ship.hullIntegrity;
    ar >> ship.armorIntegrity;
    ar >> ship.shieldIntegrity;
    ar >> ship.engineIntegrity;
    ar >> ship.missileLauncherIntegrity;
    ar >> ship.laserIntegrity;
    ar >> ship.hasTV;
    ar >> ship.fuelPercentage;

    // we used to allow shieldIntegrity > 100.0f, but now that we distinguish
    // between shield integrity and capacity, integrity can't be greater than
    // 100 anymore.
    if (ship.shieldIntegrity > 100.0f)
        ship.shieldIntegrity = 100.0f;

    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const Ship &ship) {
    string class_name = string(Ship::class_name);
    int schema = 0;

    ar << class_name;
    ar << schema;

    ar << ship.name;
    ar << ship.cargoPodCount;
    ar << ship.engineClass;
    ar << ship.shieldClass;
    ar << ship.armorClass;
    ar << ship.missileLauncherClass;
    ar << ship.laserClass;
    ar << ship.hullIntegrity;
    ar << ship.armorIntegrity;
    ar << ship.shieldIntegrity;
    ar << ship.engineIntegrity;
    ar << ship.missileLauncherIntegrity;
    ar << ship.laserIntegrity;
    ar << ship.hasTV;
    ar << ship.fuelPercentage;
    return ar;
}

Ship &
Ship::operator=(const Ship &rhs) {
    if (this == &rhs)
        return *this;

    name = rhs.name;
    cargoPodCount = rhs.cargoPodCount;

    engineClass = rhs.engineClass;
    shieldClass = rhs.shieldClass;
    armorClass = rhs.armorClass;
    laserClass = rhs.laserClass;
    missileLauncherClass = rhs.missileLauncherClass;

    maxEngineClass = rhs.maxEngineClass;
    maxShieldClass = rhs.maxShieldClass;
    maxArmorClass = rhs.maxArmorClass;
    maxLaserClass = rhs.maxLaserClass;
    maxMissileLauncherClass = rhs.maxMissileLauncherClass;

    hullIntegrity = rhs.hullIntegrity;
    engineIntegrity = rhs.engineIntegrity;
    shieldIntegrity = rhs.shieldIntegrity;
    shieldCapacity = rhs.shieldCapacity;
    armorIntegrity = rhs.armorIntegrity;
    laserIntegrity = rhs.laserIntegrity;
    missileLauncherIntegrity = rhs.missileLauncherIntegrity;

    hasTV = rhs.hasTV;
    fuelPercentage = rhs.fuelPercentage;

    partInRepair = rhs.partInRepair;

    for (int i = 0; i < NUM_REPAIR_PARTS; i++) {
        repairMinerals[i] = rhs.repairMinerals[i];
        repairCounters[i] = rhs.repairCounters[i];
    }

    return *this;
}

void
Ship::damageRandomSystemOrCrew(int odds, int mindamage, int maxdamage) {
    if (Util::Random(1, 100) > odds)
        return;

    float amount;
    int damage = Util::Random(mindamage, maxdamage);
    int system =
        Util::Random(0, 4); // 0=hull,1=laser,2=missile,3=shield,4=engine,5=crew

    switch (system) {
    case 0:
        // damage the hull
        amount = getHullIntegrity();
        if (amount > 0) {
            amount -= damage;
            if (amount < 0) {
                amount = 0;
                g_game->printout("Ship's Hull has been destroyed.", RED, 1000);
            }
        } else
            g_game->printout("Ship's Hull has been breached!", YELLOW, 1000);
        setHullIntegrity(amount);
        break;
    case 1:
        // damage the laser
        amount = getLaserIntegrity();
        if (amount > 1) {
            amount -= damage;
            if (amount < 1) {
                amount = 1;
                g_game->printout(
                    "Your laser has been heavily damaged!", RED, 1000);
            } else
                g_game->printout("Laser is sustaining damage.", YELLOW, 1000);
            setLaserIntegrity(amount);
        }
        break;
    case 2:
        // damage missile launcher
        amount = getMissileLauncherIntegrity();
        if (amount > 1) {
            amount -= damage;
            if (amount < 1) {
                amount = 1;
                g_game->printout(
                    "The missile launcher has been heavily damaged!",
                    RED,
                    1000);
            } else
                g_game->printout(
                    "Missile launcher is sustaining damage.", YELLOW, 1000);
            setMissileLauncherIntegrity(amount);
        }
        break;
    case 3:
        // damage shield generator
        amount = getShieldIntegrity();
        if (amount > 1) {
            amount -= damage;
            if (amount < 1) {
                amount = 1;
                g_game->printout(
                    "The shield generator has been heavily damaged!",
                    RED,
                    1000);
            } else
                g_game->printout(
                    "Shield generator is sustaining damage.", YELLOW, 1000);
            setShieldIntegrity(amount);
        }
        break;
    case 4:
        // damage engine
        amount = getEngineIntegrity();
        if (amount > 1) {
            amount -= damage;
            if (amount < 1) {
                amount = 1;
                g_game->printout(
                    "The engine has been heavily damaged!", RED, 1000);
            } else
                g_game->printout("Engine is sustaining damage.", YELLOW, 1000);
            setEngineIntegrity(amount);
        }
        break;
    }
}

// GAMESTATE CLASS
const map<Skill, OfficerType> GameState::skill_map = {
    {SKILL_SCIENCE, OFFICER_SCIENCE},
    {SKILL_NAVIGATION, OFFICER_NAVIGATION},
    {SKILL_TACTICAL, OFFICER_TACTICAL},
    {SKILL_ENGINEERING, OFFICER_ENGINEER},
    {SKILL_COMMUNICATION, OFFICER_COMMUNICATION},
    {SKILL_MEDICAL, OFFICER_MEDICAL}};

GameState::GameState() : m_items(*new Items) { Reset(); }

ALLEGRO_PATH *
GameState::get_save_file_path(GameSaveSlot slot) {
    ALLEGRO_ASSERT(slot != GAME_SAVE_SLOT_UNKNOWN);
    string slotpathname;
    if (slot == GAME_SAVE_SLOT_NEW) {
        slotpathname = "saves/newcaptain.dat";
    } else {
        slotpathname =
            string("saves/savegame-") + to_string(slot) + string(".dat");
    }
    ALLEGRO_PATH *save_file_path = al_create_path(slotpathname.c_str());

    return save_file_path;
}

GameState::~GameState() {}

void
GameState::operator=(const GameState &other) {
    for (int i = 0; i < NUM_ALIEN_RACES; i++) {
        alienAttitudes[i] = other.alienAttitudes[i];
    }
    flux_info = other.flux_info;
    alienAttitudeUpdate = other.alienAttitudeUpdate;
    playerPosture = other.playerPosture;
    m_baseGameTimeSecs = other.m_baseGameTimeSecs;
    m_gameTimeSecs = other.m_gameTimeSecs;
    stardate = other.stardate;
    m_captainSelected = other.m_captainSelected;
    m_profession = other.m_profession;
    m_credits = other.m_credits;
    m_items = other.m_items;
    player = other.player;
    m_ship = other.m_ship;
    m_currentSelectedOfficer = other.m_currentSelectedOfficer;
    navigateStatus = other.navigateStatus;
    shieldStatus = other.shieldStatus;
    weaponStatus = other.weaponStatus;
    plotStage = other.plotStage;
    currentModule = other.currentModule;
    firstTimeVisitor = other.firstTimeVisitor;
    activeQuest = other.activeQuest;
    storedValue = other.storedValue;
    questCompleted = other.questCompleted;
    currentModeWhenGameSaved = other.currentModeWhenGameSaved;
    for (auto &i : other.m_unemployed_officers) {
        m_unemployed_officers.insert(new Officer(*i));
    }
    for (auto &i : other.m_unassigned_officers) {
        m_unassigned_officers.insert(new Officer(*i));
    }
    for (auto &i : other.m_crew) {
        m_crew[i.first] = new Officer(*i.second);
    }
}

void
PlayerInfo::Reset() {
    m_scanner = false;
    m_previous_scanner_state = false;
    m_bHasHyperspacePermit = true;

    currentStar = 2;
    currentPlanet = 450;
    controlPanelMode = 0; // ?????  NOT SURE WHAT TO SET THIS TO

    posHyperspace.x = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_X");
    posHyperspace.y = g_game->getGlobalNumber("PLAYER_HYPERSPACE_START_Y");

    posSystem.x = g_game->getGlobalNumber("PLAYER_SYSTEM_START_X");
    posSystem.y = g_game->getGlobalNumber("PLAYER_SYSTEM_START_Y");

    posPlanet.x = 0; // randomized in PlanetSurface module
    posPlanet.y = 0;

    posStarport.x = g_game->getGlobalNumber("PLAYER_STARPORT_START_X");
    posStarport.y = g_game->getGlobalNumber("PLAYER_STARPORT_START_Y");

    posCombat.x = 0;
    posCombat.y = 0;

    m_is_lost = false;
    alive = true;
}

bool
PlayerInfo::pay_loan(int amount) {
    if (m_loan) {
        if (m_loan->pay(amount)) {
            m_loan = nullopt;
            return true;
        }
        return false;
    }
    return true;
}

bool
PlayerInfo::take_loan(int amount) {
    if (m_loan) {
        return false;
    }
    m_loan = Loan(amount);
    return true;
}

void
PlayerInfo::update_loan() {
    if (m_loan && m_loan->is_paid_off()) {
        m_loan = nullopt;
    }
    if (m_loan) {
        m_loan->compute_interest();
    }
}

std::string
PlayerInfo::getAlienRaceName(int race) {
    switch (race) {
    case 1:
        return "Pirate";
        break;
    case 2:
        return "Elowan";
        break;
    case 3:
        return "Spemin";
        break;
    case 4:
        return "Thrynn";
        break;
    case 5:
        return "Barzhon";
        break;
    case 6:
        return "Nyssian";
        break;
    case 7:
        return "Tafel";
        break;
    case 8:
        return "Minex";
        break;
    case 9:
        return "Coalition";
        break;
    default:
        return "None";
        break;
    }
    return "";
}

std::string
PlayerInfo::getAlienRaceName(AlienRaces race) {
    return getAlienRaceName((int)race);
}

std::string
PlayerInfo::getAlienRaceNamePlural(AlienRaces race) {
    switch (race) {
    case ALIEN_ELOWAN:
        return "Elowan";
        break;
    case ALIEN_SPEMIN:
        return "Spemin";
        break;
    case ALIEN_THRYNN:
        return "Thrynn";
        break;
    case ALIEN_BARZHON:
        return "Barzhon";
        break;
    case ALIEN_NYSSIAN:
        return "Nyssian";
        break;
    case ALIEN_TAFEL:
        return "Tafel";
        break;
    case ALIEN_MINEX:
        return "Minex";
        break;
    case ALIEN_COALITION:
        return "Coalition";
        break;
    case ALIEN_PIRATE:
        return "Pirates";
        break;
    default:
        return "None";
        break;
    }
    return "";
}

InputArchive &
operator>>(InputArchive &ar, PlayerInfo &info) {
    string class_name = string(PlayerInfo::class_name);
    int schema = 1;
    // schema 1: remove hasOverdueLoan, replace with actual loan data

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: " + class_name + " version "
                + std::to_string(load_schema));
    }

    ar >> info.m_scanner;
    ar >> info.m_previous_scanner_state;
    ar >> info.m_bHasHyperspacePermit;
    if (schema == 0) {
        bool has_overdue_loan;
        ar >> has_overdue_loan;
    } else {
        bool has_loan;
        ar >> has_loan;

        if (has_loan) {
            info.m_loan = Loan(ar);
        } else {
            info.m_loan = nullopt;
        }
    }

    ar >> info.currentStar;
    ar >> info.currentPlanet;
    ar >> info.controlPanelMode;

    ar >> info.posHyperspace;
    ar >> info.posSystem;
    ar >> info.posPlanet;
    ar >> info.posStarport;
    ar >> info.posCombat;

    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const PlayerInfo &info) {
    string class_name = string(PlayerInfo::class_name);
    int schema = 1;

    ar << class_name;
    ar << schema;

    ar << info.m_scanner;
    ar << info.m_previous_scanner_state;
    ar << info.m_bHasHyperspacePermit;

    ar << (info.m_loan != nullopt);

    if (info.m_loan) {
        ar << *info.m_loan;
    }

    ar << info.currentStar;
    ar << info.currentPlanet;
    ar << info.controlPanelMode;

    ar << info.posHyperspace;
    ar << info.posSystem;
    ar << info.posPlanet;
    ar << info.posStarport;
    ar << info.posCombat;

    return ar;
}

PlayerInfo &
PlayerInfo::operator=(const PlayerInfo &rhs) {
    if (this == &rhs)
        return *this;

    m_scanner = rhs.m_scanner;
    m_previous_scanner_state = rhs.m_previous_scanner_state;
    m_bHasHyperspacePermit = rhs.m_bHasHyperspacePermit;

    currentStar = rhs.currentStar;
    currentPlanet = rhs.currentPlanet;
    controlPanelMode = rhs.controlPanelMode;

    posHyperspace = rhs.posHyperspace;
    posSystem = rhs.posSystem;
    posPlanet = rhs.posPlanet;
    posStarport = rhs.posStarport;
    posCombat = rhs.posCombat;
    if (rhs.m_loan) {
        m_loan = Loan(*rhs.m_loan);
    }

    return *this;
}

/*
 * This resets the GameState to the default values
 */
void
GameState::Reset() {
    // initialize alien race attitudes
    alienAttitudes[ALIEN_ELOWAN] = 40;
    alienAttitudes[ALIEN_SPEMIN] = 70;
    alienAttitudes[ALIEN_THRYNN] = 50;
    alienAttitudes[ALIEN_BARZHON] = 40;
    alienAttitudes[ALIEN_NYSSIAN] = 60;
    alienAttitudes[ALIEN_TAFEL] = 10;
    alienAttitudes[ALIEN_MINEX] = 50;
    alienAttitudes[ALIEN_COALITION] = 40;
    alienAttitudes[ALIEN_PIRATE] = 10;

    // start alien attitude update time
    alienAttitudeUpdate = g_game->globalTimer.getTimer();

    // initialize player's posture
    playerPosture = "friendly";

    m_gameTimeSecs = 0;
    m_baseGameTimeSecs = 0;

    stardate.Reset(); // altered to use current object.

    setCaptainSelected(false);

    setProfession(PROFESSION_MILITARY);

    // player starts with nothing and given things via missions
    setCredits(0);

    m_items.Reset();

    // initialize player data
    player.Reset();

    // This returns the ship's values to the defaults
    m_ship.Reset();

    m_currentSelectedOfficer = OFFICER_CAPTAIN;

    m_unemployed_officers.clear();
    m_unassigned_officers.clear();
    m_crew.clear();

    // initial tactical properties
    shieldStatus = false;
    weaponStatus = false;

    currentModeWhenGameSaved = "";

    // current stage of the game, INITIAL=1,VIRUS=2,WAR=3,ANCIENTS=4
    plotStage = 1;

    // quest related state variables
    activeQuest = 1; // -1;  bug fix
    storedValue = -1;
    questCompleted = false;
    firstTimeVisitor = true;
    for (int i = 0; i < MAX_FLUX; i++) {
        flux_info[i].endpoint_1_visible = false;
        flux_info[i].endpoint_2_visible = false;
        flux_info[i].path_visible = false;
    }
}

InputArchive &
operator>>(InputArchive &ar, GameState &game_state) {
    string class_name = string(GameState::class_name);
    int schema = 5;
    // schema 1 - added currentModeWhenGameSaved
    // schema 2 - removed fluxSeed, added flux_info
    // schema 3 - rework flux_info to include ID, data
    // schema 4 - remove unused cargo stacks
    // schema 5 - replace m_unemployed_officers and officerXXX with
    //            m_unemployed_officers, m_unassigned_officers, m_crew
    game_state.Reset();

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: unknown schema: " + class_name + " version "
                + std::to_string(load_schema));
    }

    int load_num_alien_races;
    ar >> load_num_alien_races;
    for (int i = 0; (i < load_num_alien_races) && (i < NUM_ALIEN_RACES); i++)
        ar >> game_state.alienAttitudes[i];

    ar >> game_state.playerPosture;

    // the base game time is added to the current ms timer to get an
    // accurate date (from gameTimeSecs), so we save gameTimeSecs, but read
    // back into baseGameTimeSecs:
    ar >> game_state.m_baseGameTimeSecs;
    ar >> game_state.stardate;
    ar >> game_state.m_captainSelected;

    int tmpi;
    ar >> tmpi;
    game_state.m_profession = (ProfessionType)tmpi;

    ar >> game_state.m_credits;

    ar >> game_state.m_items;
    ar >> game_state.player;
    ar >> game_state.m_ship;

    if (load_schema < 2) {
        int flux_seed;
        ar >> flux_seed;
    }

    int load_currently_selected_officer;
    ar >> load_currently_selected_officer;
    game_state.m_currentSelectedOfficer =
        static_cast<OfficerType>(load_currently_selected_officer);

    if (load_schema < 5) {
        int load_num_unemployed_officers;
        ar >> load_num_unemployed_officers;

        game_state.m_unemployed_officers.clear();
        for (int i = 0; i < load_num_unemployed_officers; i++) {
            Officer *officer = new Officer(ar);
            game_state.m_unemployed_officers.insert(officer);
        }
        game_state.m_unassigned_officers.clear();

        int load_num_officers;
        ar >> load_num_officers;

        for (int i = 0; i < load_num_officers; i++) {
            Officer *crew_member = new Officer(ar);
            game_state.m_crew[crew_member->get_officer_type()] = crew_member;
        }
    } else {
        int loaded_unemployed;
        ar >> loaded_unemployed;
        for (int i = 0; i < loaded_unemployed; i++) {
            Officer *unemployed_officer = new Officer(ar);
            game_state.m_unemployed_officers.insert(unemployed_officer);
        }

        int loaded_unassigned;
        ar >> loaded_unassigned;
        for (int i = 0; i < loaded_unassigned; i++) {
            Officer *unassigned_officer = new Officer(ar);
            game_state.m_unassigned_officers.insert(unassigned_officer);
        }

        int loaded_crew;
        ar >> loaded_crew;
        for (int i = 0; i < loaded_crew; i++) {
            Officer *crew = new Officer(ar);
            game_state.m_crew[crew->get_officer_type()] = crew;
        }
    }

    if (load_schema < 4) {
        int TotalCargoStacks;
        int defaultShipCargoSize;
        ar >> TotalCargoStacks;
        ar >> defaultShipCargoSize;
    }

    if (load_schema >= 1)
        ar >> game_state.currentModeWhenGameSaved;

    // load quest data
    ar >> game_state.activeQuest;
    ar >> game_state.storedValue;

    if (load_schema == 2) {
        int max_flux;
        ar >> max_flux;

        ALLEGRO_ASSERT(max_flux == MAX_FLUX);
        game_state.flux_info.clear();
        for (int i = 0; i < MAX_FLUX; i++) {
            FluxInfo fi;

            ar >> fi.endpoint_1_visible;
            ar >> fi.endpoint_2_visible;
            ar >> fi.path_visible;

            if (fi.endpoint_1_visible || fi.endpoint_2_visible
                || fi.path_visible) {
                game_state.flux_info[i] = fi;
            }
        }
    } else if (load_schema >= 3) {
        int num_flux;
        ar >> num_flux;

        game_state.flux_info.clear();
        for (int i = 0; i < num_flux; i++) {
            FluxInfo fi;
            ID flux_id;

            ar >> flux_id;
            ar >> fi.endpoint_1_visible;
            ar >> fi.endpoint_2_visible;
            ar >> fi.path_visible;

            if (fi.endpoint_1_visible || fi.endpoint_2_visible
                || fi.path_visible) {
                game_state.flux_info[i] = fi;
            }
        }
    }

    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const GameState &game_state) {
    string class_name = string(GameState::class_name);
    int schema = 5;
    // schema 1 - added currentModeWhenGameSaved
    // schema 2 - removed fluxSeed, added flux_info
    // schema 3 - rework flux_info to include ID, data
    // schema 4 - remove unused cargo stacks
    // schema 5 - replace m_unemployed_officers and officerXXX with
    //            m_unemployed_officers, m_unassigned_officers, m_crew

    ar << class_name;
    ar << schema;

    ar << static_cast<int>(NUM_ALIEN_RACES);
    for (int i = 0; i < NUM_ALIEN_RACES; i++)
        ar << game_state.alienAttitudes[i];

    ar << game_state.playerPosture;

    // the base game time is added to the current ms timer to get an
    // accurate date (from gameTimeSecs), so we save gameTimeSecs, but read
    // back into
    ar << static_cast<double>(game_state.m_gameTimeSecs);

    ar << game_state.stardate;

    ar << game_state.m_captainSelected;
    ar << static_cast<int>(game_state.m_profession);

    ar << game_state.m_credits;

    ar << game_state.m_items;

    ar << game_state.player;
    ar << game_state.m_ship;
    ar << static_cast<int>(game_state.m_currentSelectedOfficer);

    ar << static_cast<int>(game_state.m_unemployed_officers.size());
    for (auto &i : game_state.m_unemployed_officers) {
        ar << *i;
    }
    ar << static_cast<int>(game_state.m_unassigned_officers.size());
    for (auto &i : game_state.m_unassigned_officers) {
        ar << *i;
    }
    ar << static_cast<int>(game_state.m_crew.size());
    for (auto &i : game_state.m_crew) {
        ar << *(i.second);
    }

    ar << g_game->modeMgr->GetCurrentModuleName();

    // save quest data
    ar << game_state.activeQuest;
    // storedValue keeps track of current quest objective
    ar << game_state.storedValue;

    ar << static_cast<int>(game_state.flux_info.size());
    for (auto &i : game_state.flux_info) {
        ar << i.first;
        ar << i.second.endpoint_1_visible;
        ar << i.second.endpoint_2_visible;
        ar << i.second.path_visible;
    }
    return ar;
}

GameState::GameSaveSlot GameState::currentSaveGameSlot =
    GameState::GAME_SAVE_SLOT_UNKNOWN;

#define GAME_MAGIC 0xAAFFAAFF
#define GAME_STRING "StarflightTLC-SaveGame"
#define GAME_SCHEMA 0

bool
GameState::SaveGame(GameSaveSlot slot) {
    currentSaveGameSlot = slot;
    ALLEGRO_PATH *slot_path = get_save_file_path(slot);
    ensure_save_dir();

    try {
        OutputArchive ar(slot_path);
        al_destroy_path(slot_path);

        int GameMagic = GAME_MAGIC;
        ar << GameMagic;

        string GameString = GAME_STRING;
        ar << GameString;

        int GameSchema = GAME_SCHEMA;
        ar << GameSchema;

        ar << *this;
        ar << GameMagic;

        ar.close();
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}

GameState *
GameState::ReadGame(GameSaveSlot slot) {
    currentSaveGameSlot = slot;
    ALLEGRO_PATH *slot_path = get_save_file_path(slot);
    GameState *g = nullptr;

    try {
        InputArchive ar(slot_path);
        al_destroy_path(slot_path);

        // numeric code uniquely identifying this game's file
        int LoadedGameMagic;
        ar >> LoadedGameMagic;
        if ((unsigned int)LoadedGameMagic != GAME_MAGIC) {
            g_game->message("Invalid save game file");
            ALLEGRO_DEBUG("*** GameState: Invalid save game file");
            return nullptr;
        }

        // unique string for this savegame file
        string LoadedGameString;
        ar >> LoadedGameString;
        if (LoadedGameString != GAME_STRING) {
            g_game->message("Invalid save game file");
            ALLEGRO_DEBUG("*** GameState: Invalid save game file");
            return nullptr;
        }

        // schema number--not really needed
        int LoadedGameSchema = 0;
        ar >> LoadedGameSchema;
        if (LoadedGameSchema > GAME_SCHEMA) {
            ALLEGRO_DEBUG("*** GameState: Incorrect schema in save game file");
            return nullptr;
        }

        g = new GameState();
        ar >> *g;

        LoadedGameMagic = 0;
        ar >> LoadedGameMagic;
        if ((unsigned int)LoadedGameMagic != GAME_MAGIC) {
            ALLEGRO_DEBUG("*** GameState: Error loading save game file");
            delete g;
            return nullptr;
        }
    } catch (const std::exception &e) {
        delete g;
        return nullptr;
    }
    return g;
}

GameState *
GameState::LoadGame(GameSaveSlot slot) {
    ALLEGRO_DEBUG("\n");
    GameState *gs = ReadGame(slot);
    if (gs == NULL) {
        ALLEGRO_DEBUG(" in GameState::LoadGame\n");
        return gs;
    } // message picks up where ReadGame left off.

    gs->m_captainSelected = true;
    *g_game->gameState = *gs; // assign to game state.
    delete gs;                // return reference pointer only (to game state).

    g_game->globalTimer
        .reset(); // reset the global timer (logic from CaptainsLounge module)
    // restart the quest mgr
    if (!g_game->questMgr->Initialize())
        g_game->fatalerror(
            "GameState::LoadGame(): Error initializing quest manager");

    /* Copy baseGameTimeSecs into the gameTimeSecs (note this could be done on
       assignment; in order to keep code functions compartmentalized, it is done
       here along with all other manipulations related to GameState
       initialization). This assignment is done to avoid issues related to
       reloading a saved game starting in a time-paused module such as the
       Captain's Lounge, from another module with unpaused time. (=>
       baseGameTimeSecs is set using gameTimeSecs, which is assumed to have been
       previously updated itself from baseGameTimeSecs and the timer ) */
    g_game->gameState->setGameTimeSecs(
        g_game->gameState->getBaseGameTimeSecs());

    // Update the quest completion status:
    g_game->questMgr->getActiveQuest();
    int reqCode = g_game->questMgr->questReqCode;
    int reqType = g_game->questMgr->questReqType;
    int reqAmt = g_game->questMgr->questReqAmt;
    g_game->questMgr->VerifyRequirements(reqCode, reqType, reqAmt);

    /* when loading a saved game, the plot stage was being set incorrectly. It
    was a switch statement in GameState::LoadGame() function. The switch
    statement is not needed, because the plot stage is set with the call to
    QuestMgr::getActiveQuest(), which is being called from GameState::LoadGame()
    function. */

    // Set the corresponding plot stage
    /*	switch ( g_game->gameState->getActiveQuest() ) {
                    case 36: g_game->gameState->setPlotStage(2); break;
                    case 37: g_game->gameState->setPlotStage(3); break;
                    case 38: case 39: g_game->gameState->setPlotStage(4); break;

                    default: g_game->gameState->setPlotStage(1); break;
            }
    */

    // Set maximum ship upgrades available given captain profession
    int maxEngineClass = 0, maxShieldClass = 0, maxArmorClass = 0,
        maxMissileLauncherClass = 0, maxLaserClass = 0;
    switch (g_game->gameState->getProfession()) {
    case PROFESSION_FREELANCE:
        maxEngineClass = g_game->getGlobalNumber("PROF_FREELANCE_ENGINE_MAX");
        maxShieldClass = g_game->getGlobalNumber("PROF_FREELANCE_SHIELD_MAX");
        maxArmorClass = g_game->getGlobalNumber("PROF_FREELANCE_ARMOR_MAX");
        maxMissileLauncherClass =
            g_game->getGlobalNumber("PROF_FREELANCE_MISSILE_MAX");
        maxLaserClass = g_game->getGlobalNumber("PROF_FREELANCE_LASER_MAX");
        break;
    case PROFESSION_MILITARY:
        maxEngineClass = g_game->getGlobalNumber("PROF_MILITARY_ENGINE_MAX");
        maxShieldClass = g_game->getGlobalNumber("PROF_MILITARY_SHIELD_MAX");
        maxArmorClass = g_game->getGlobalNumber("PROF_MILITARY_ARMOR_MAX");
        maxMissileLauncherClass =
            g_game->getGlobalNumber("PROF_MILITARY_MISSILE_MAX");
        maxLaserClass = g_game->getGlobalNumber("PROF_MILITARY_LASER_MAX");
        break;
    case PROFESSION_SCIENTIFIC:
        maxEngineClass = g_game->getGlobalNumber("PROF_SCIENCE_ENGINE_MAX");
        maxShieldClass = g_game->getGlobalNumber("PROF_SCIENCE_SHIELD_MAX");
        maxArmorClass = g_game->getGlobalNumber("PROF_SCIENCE_ARMOR_MAX");
        maxMissileLauncherClass =
            g_game->getGlobalNumber("PROF_SCIENCE_MISSILE_MAX");
        maxLaserClass = g_game->getGlobalNumber("PROF_SCIENCE_LASER_MAX");
        break;
    default:
        // cant happen
        ALLEGRO_ASSERT(0);
    }

    g_game->gameState->m_ship.setMaxEngineClass(maxEngineClass);
    g_game->gameState->m_ship.setMaxShieldClass(maxShieldClass);
    g_game->gameState->m_ship.setMaxArmorClass(maxArmorClass);
    g_game->gameState->m_ship.setMaxMissileLauncherClass(
        maxMissileLauncherClass);
    g_game->gameState->m_ship.setMaxLaserClass(maxLaserClass);

    g_game->gameState->m_ship.setShieldCapacity(
        g_game->gameState->m_ship.getMaxShieldCapacity());

    // Roll random repair minerals and set the repair counters
    g_game->gameState->m_ship.initializeRepair();

    ALLEGRO_DEBUG("Game state loaded successfully\n");
    return g_game->gameState; // return gs & leave deletion to caller?
}

void
GameState::DeleteGame(GameSaveSlot slot) {
    ALLEGRO_PATH *slot_path = get_save_file_path(slot);
    const char *slot_string = al_path_cstr(slot_path, ALLEGRO_NATIVE_PATH_SEP);
    if (al_filename_exists(slot_string)) {
        bool rc = al_remove_filename(slot_string);

        if (!rc) {
            ALLEGRO_ERROR("Unable to delete %s\n", slot_string);
        }
    }
}

bool
GameState::AutoSave() {
    if (currentSaveGameSlot == GAME_SAVE_SLOT_UNKNOWN) {
        return false;
    }

    return SaveGame(currentSaveGameSlot);
}

bool
GameState::AutoLoad() {
    if (currentSaveGameSlot == GAME_SAVE_SLOT_UNKNOWN) {
        return false;
    }

    GameState *lgs = LoadGame(currentSaveGameSlot);
    if (lgs == nullptr) {
        return false;
    }

    if (currentModeWhenGameSaved.size() > 0) {
        g_game->modeMgr->LoadModule(currentModeWhenGameSaved);
    }
    return true;
}

// accessors

int
GameState::getCredits() const {
    return m_credits;
}

string
GameState::getProfessionString() const {
    switch (m_profession) {
    case PROFESSION_FREELANCE:
        return "freelance";
        break;
    case PROFESSION_MILITARY:
        return "military";
        break;
    case PROFESSION_SCIENTIFIC:
        return "scientific";
        break;
    }
    return "";
}

const Officer *
GameState::set_officer(OfficerType type, const Officer *officer) {
    auto old_officer_it = m_crew.find(type);
    Officer *old_officer = nullptr;

    ALLEGRO_ASSERT(type != OFFICER_NONE);
    ALLEGRO_ASSERT(type != OFFICER_CAPTAIN);
    ALLEGRO_ASSERT(officer != nullptr);

    if (old_officer_it != m_crew.end()) {
        old_officer = old_officer_it->second;
        if (old_officer == officer) {
            return nullptr;
        }
        old_officer->set_officer_type(OFFICER_NONE);
        m_unassigned_officers.insert(old_officer);
    }

    if (officer->get_officer_type() == OFFICER_NONE) {
        auto officer_it =
            m_unassigned_officers.find(const_cast<Officer *>(officer));

        ALLEGRO_ASSERT(officer_it != m_unassigned_officers.end());
        auto unconst = *officer_it;
        unconst->set_officer_type(type);
        m_unassigned_officers.erase(officer_it);
        m_crew[type] = unconst;
    } else {
        auto officer_it =
            find_if(m_crew.begin(), m_crew.end(), [officer](auto i) {
                return i.second == officer;
            });
        ALLEGRO_ASSERT(officer_it != m_crew.end());
        ALLEGRO_ASSERT(
            officer_it->second->get_officer_type() != OFFICER_CAPTAIN);
        auto unconst = officer_it->second;
        unconst->set_officer_type(type);
        m_crew.erase(officer_it);
        m_crew[type] = unconst;
    }

    return old_officer;
}

void
GameState::hire_officer(const Officer *officer) {
    auto type = officer->get_officer_type();
    ALLEGRO_ASSERT(type == OFFICER_NONE);

    auto o = m_unemployed_officers.find(const_cast<Officer *>(officer));
    ALLEGRO_ASSERT(o != m_unemployed_officers.end());
    m_unassigned_officers.insert(*o);
    m_unemployed_officers.erase(o);
}

void
GameState::unassign_officer(const Officer *officer) {
    auto type = officer->get_officer_type();
    ALLEGRO_ASSERT(type != OFFICER_NONE);
    auto o = m_crew.at(type);
    m_crew.erase(type);
    m_unassigned_officers.insert(o);
}

void
GameState::fire_officer(const Officer *officer) {
    ALLEGRO_ASSERT(officer != nullptr);
    auto type = officer->get_officer_type();

    if (type != OFFICER_NONE) {
        auto o = m_crew.at(type);
        m_crew.erase(type);
        o->set_officer_type(OFFICER_NONE);
        m_unemployed_officers.insert(o);
    } else {
        auto o = m_unassigned_officers.find(const_cast<Officer *>(officer));
        ALLEGRO_ASSERT(o != m_unassigned_officers.end());
        (*o)->set_officer_type(OFFICER_NONE);
        m_unemployed_officers.insert(*o);
        m_unassigned_officers.erase(o);
    }
}

void
GameState::create_captain(
    const string &name,
    const map<Skill, int> &attributes) {
    auto old_officer = m_crew.find(OFFICER_CAPTAIN);
    auto captain = new Officer(name, attributes);
    if (old_officer != m_crew.end()) {
        old_officer->second = captain;
    } else {
        m_crew[OFFICER_CAPTAIN] = captain;
    }
}

// calculate effective skill level taking into account vitality and captain
// modifier
int
GameState::CalcEffectiveSkill(Skill skill) const {

    ALLEGRO_ASSERT(skill != SKILL_DURABILITY);
    ALLEGRO_ASSERT(skill != SKILL_LEARN_RATE);

    auto captain = get_officer(OFFICER_CAPTAIN);
    float cap_vitality = captain ? captain->get_vitality() : 0;
    float cap_skill = captain ? captain->get_skill(skill) : 0;

    auto officer = get_officer(skill_map.at(skill));
    float off_vitality = officer ? officer->get_vitality() : 0;
    float off_skill = officer ? officer->get_skill(skill) : 0;

    float res = (off_vitality > 0) ? off_skill * off_vitality / 100
                                         + cap_skill / 10 * cap_vitality / 100
                                   : cap_skill * cap_vitality / 100;

    return res;
}

/**
  this does the same thing as Officer::SkillCheck() except that it does take
into account that officers can be replaced by the captain when they are dead, it
also does take into account vitality and captain modifier.
**/
bool
GameState::SkillCheck(Skill skill) {
    auto tempOfficer = m_crew.find(skill_map.at(skill));
    int skill_value = CalcEffectiveSkill(skill);
    int chance = 0;

    if (tempOfficer != m_crew.end()) {
        tempOfficer->second->add_experience(skill, 0);
    }

    // 250+ is guaranteed pass
    if (skill_value > 250) {
        return true;
    }
    // any below 200 is % chance to pass skill check
    else if (skill_value > 200) {
        chance = 90;
    } else if (skill_value > 150) {
        chance = 80;
    } else if (skill_value > 100) {
        chance = 70;
    } else if (skill_value > 75) {
        chance = 60;
    } else if (skill_value > 50) {
        chance = 50;
    } else
        chance = 25;

    int roll = sfrand() % 100;
    return roll < chance;
}

bool
GameState::add_experience(Skill skill, int amount) {
    auto officer = effective_officer(skill_map.at(skill));

    return officer->add_experience(skill, amount);
}

bool
GameState::SkillUp(Skill skill, int amount) {
    auto off = effective_officer(GameState::skill_map.at(skill));

    if (off != nullptr) {
        return off->SkillUp(skill, amount);
    } else {
        return false;
    }
}

bool
GameState::CanSkillCheck(Skill skill) const {
    auto off = effective_officer(GameState::skill_map.at(skill));

    if (off != nullptr) {
        return off->CanSkillCheck();
    } else {
        return false;
    }
}

Ship
GameState::getShip() const {
    return m_ship;
}

bool
GameState::HaveFullCrew() const {
    return m_crew.size() == CREW_SIZE;
}

bool
GameState::PreparedToLaunch() const {
    return HaveFullCrew() && m_ship.HaveEngines();
}

AlienRaces
GameState::getCurrentAlien() {
    return player.getGalacticRegion();
}
string
GameState::getCurrentAlienName() {
    return player.getAlienRaceNamePlural(getCurrentAlien());
}
int
GameState::getAlienAttitude() {
    AlienRaces region = getCurrentAlien();
    return alienAttitudes[region];
}

// mutators
void
GameState::setCaptainSelected(bool initCaptainSelected) {
    m_captainSelected = initCaptainSelected;
}

void
GameState::setProfession(ProfessionType initProfession) {
    m_profession = initProfession;
}
void
GameState::setCredits(int initCredits) {
    m_credits = initCredits;
}
void
GameState::augCredits(int amount) {
    m_credits += amount;
}
void
GameState::setShip(const Ship &initShip) {
    m_ship = initShip;
}

void
GameState::setAlienAttitude(int value) {
    if (value < 0)
        value = 0;
    if (value > 100)
        value = 100;
    AlienRaces region = getCurrentAlien();
    alienAttitudes[region] = value;
}

void
GameState::update_unemployed_officers() {
    while (m_unemployed_officers.size() <= 18) {
        m_unemployed_officers.insert(new Officer());
    }
}

std::string
GameState::get_saved_module_name() const {
    static map<string_view, string_view> name_map = {
        {MODULE_CAPTAINCREATION, "Captain Creation"},
        {MODULE_CAPTAINSLOUNGE, "Captain's Lounge"},
        {MODULE_HYPERSPACE, "Hyperspace"},
        {MODULE_INTERPLANETARY, "Interplanetary space"},
        {MODULE_ORBIT, "Planet Orbit"},
        {MODULE_SURFACE, "Planet Surface"},
        {MODULE_PORT, "Starport"},
        {MODULE_STARPORT, "Starport"}};

    auto val = name_map.find(currentModeWhenGameSaved);

    if (val != name_map.end()) {
        return string(val->second);
    } else {
        return "";
    }
}

bool
GameState::ensure_save_dir() {
    ALLEGRO_PATH *saves_subdir = al_create_path("saves");
    bool res = true;

    if (!al_filename_exists(
            al_path_cstr(saves_subdir, ALLEGRO_NATIVE_PATH_SEP))) {
        res = al_make_directory(
            al_path_cstr(saves_subdir, ALLEGRO_NATIVE_PATH_SEP));
    }
    al_destroy_path(saves_subdir);
    return res;
}

static const map<Skill, int> BASEATT_SCIENTIFIC = {
    {SKILL_DURABILITY, 5},
    {SKILL_LEARN_RATE, 5},
    {SKILL_SCIENCE, 15},
    {SKILL_NAVIGATION, 5},
    {SKILL_TACTICAL, 0},
    {SKILL_ENGINEERING, 5},
    {SKILL_COMMUNICATION, 15},
    {SKILL_MEDICAL, 10},
};

static const map<Skill, int> MAXATT_SCIENTIFIC = {
    {SKILL_DURABILITY, 10},
    {SKILL_LEARN_RATE, 10},
    {SKILL_SCIENCE, 250},
    {SKILL_NAVIGATION, 95},
    {SKILL_TACTICAL, 65},
    {SKILL_ENGINEERING, 95},
    {SKILL_COMMUNICATION, 250},
    {SKILL_MEDICAL, 125}};

static const map<Skill, int> BASEATT_FREELANCE = {
    {SKILL_DURABILITY, 5},
    {SKILL_LEARN_RATE, 5},
    {SKILL_SCIENCE, 5},
    {SKILL_NAVIGATION, 15},
    {SKILL_TACTICAL, 5},
    {SKILL_ENGINEERING, 10},
    {SKILL_COMMUNICATION, 15},
    {SKILL_MEDICAL, 0},
};

static const map<Skill, int> MAXATT_FREELANCE = {
    {SKILL_DURABILITY, 10},
    {SKILL_LEARN_RATE, 10},
    {SKILL_SCIENCE, 95},
    {SKILL_NAVIGATION, 250},
    {SKILL_TACTICAL, 95},
    {SKILL_ENGINEERING, 125},
    {SKILL_COMMUNICATION, 250},
    {SKILL_MEDICAL, 65},
};

static const map<Skill, int> BASEATT_MILITARY = {
    {SKILL_DURABILITY, 5},
    {SKILL_LEARN_RATE, 5},
    {SKILL_SCIENCE, 0},
    {SKILL_NAVIGATION, 10},
    {SKILL_TACTICAL, 15},
    {SKILL_ENGINEERING, 10},
    {SKILL_COMMUNICATION, 15},
    {SKILL_MEDICAL, 0},
};

static const map<Skill, int> MAXATT_MILITARY = {
    {SKILL_DURABILITY, 10},
    {SKILL_LEARN_RATE, 10},
    {SKILL_SCIENCE, 65},
    {SKILL_NAVIGATION, 125},
    {SKILL_TACTICAL, 250},
    {SKILL_ENGINEERING, 125},
    {SKILL_COMMUNICATION, 250},
    {SKILL_MEDICAL, 65},
};

const std::map<Skill, int> &
GameState::base_skills(ProfessionType type) {
    ALLEGRO_ASSERT(
        type == PROFESSION_FREELANCE || type == PROFESSION_MILITARY
        || type == PROFESSION_SCIENTIFIC);

    switch (type) {
    case PROFESSION_MILITARY:
        return BASEATT_MILITARY;
    case PROFESSION_FREELANCE:
        return BASEATT_FREELANCE;
    case PROFESSION_SCIENTIFIC:
        return BASEATT_SCIENTIFIC;
    default:
        throw std::domain_error("Invalid ProfessionType value");
    }
}

const std::map<Skill, int> &
GameState::base_skills() {
    return base_skills(getProfession());
}

const std::map<Skill, int> &
GameState::max_skills(ProfessionType type) {
    ALLEGRO_ASSERT(
        type == PROFESSION_FREELANCE || type == PROFESSION_MILITARY
        || type == PROFESSION_SCIENTIFIC);

    switch (type) {
    case PROFESSION_MILITARY:
        return MAXATT_MILITARY;
    case PROFESSION_FREELANCE:
        return MAXATT_FREELANCE;
    case PROFESSION_SCIENTIFIC:
        return MAXATT_SCIENTIFIC;
    default:
        throw std::domain_error("Invalid ProfessionType value");
    }
}

const std::map<Skill, int> &
GameState::max_skills() {
    return max_skills(getProfession());
}
