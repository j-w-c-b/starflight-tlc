/*
        STARFLIGHT - THE LOST COLONY
        DataMgr.cpp - ?
        Author: Dave "coder1024" Calkins
        Date: 01/22/2007
*/

#include "DataMgr.h"
#include "Archive.h"
#include "Game.h"
#include "GameState.h"
#include "QuestMgr.h"
#include "Util.h"
#include "tinyxml/tinyxml.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <cstdarg>

ALLEGRO_DEBUG_CHANNEL("DataMgr")

using namespace std;

#define ITEMS_FILE "data/strfltitems.xml"
#define GALAXY_FILE "data/galaxy.xml"
#define HUMANNAMES_FILE "data/human.xml"

Item::Item() { Reset(); }

Item::Item(const Item &rhs) { *this = rhs; }

Item::~Item() {}

Item &
Item::operator=(const Item &rhs) {
    id = rhs.id;
    itemType = rhs.itemType;
    name = rhs.name;
    value = rhs.value;
    size = rhs.size;
    speed = rhs.speed;
    danger = rhs.danger;
    damage = rhs.damage;
    itemAge = rhs.itemAge;
    shipRepairMetal = rhs.shipRepairMetal;
    blackMarketItem = rhs.blackMarketItem;
    portrait = rhs.portrait;
    planetid = rhs.planetid;
    x = rhs.x;
    y = rhs.y;
    description = rhs.description;

    return *this;
}

void
Item::Reset() {
    id = 0;
    itemType = IT_INVALID;
    name = "";
    value = 0;
    size = 0;
    speed = 0;
    danger = 0;
    damage = 0;
    itemAge = IA_INVALID;
    shipRepairMetal = false;
    blackMarketItem = false;
    portrait = "";
    planetid = 0;
    x = 0;
    y = 0;
}

ItemType
Item::ItemTypeFromString(const string &s) {
    ItemType result = IT_INVALID;

    if (s == "Artifact") {
        result = IT_ARTIFACT;
    } else if (s == "Ruin") {
        result = IT_RUIN;
    } else if (s == "Mineral") {
        result = IT_MINERAL;
    } else if (s == "Life Form") {
        result = IT_LIFEFORM;
    } else if (s == "Trade Item") {
        result = IT_TRADEITEM;
    }

    return result;
}

ItemAge
Item::ItemAgeFromString(const string &s) {
    ItemAge result = IA_INVALID;

    if (s == "Stone") {
        result = IA_STONE;
    } else if (s == "Metal") {
        result = IA_METAL;
    } else if (s == "Industrial") {
        result = IA_INDUSTRIAL;
    } else if (s == "Space Faring") {
        result = IA_SPACEFARING;
    }

    return result;
}

Star::Star()
    : id(-1), name(""), x(0), y(0), spectralClass(SC_INVALID), color(""),
      temperature(0), mass(1), radius(1), luminosity(1) {}

Star::Star(const Star &rhs) { *this = rhs; }

Star &
Star::operator=(const Star &rhs) {
    id = rhs.id;
    name = rhs.name;
    x = rhs.x;
    y = rhs.y;
    spectralClass = rhs.spectralClass;
    color = rhs.color;
    temperature = rhs.temperature;
    mass = rhs.mass;
    radius = rhs.radius;
    luminosity = rhs.luminosity;

    return *this;
}

Star::~Star() {
    for (vector<Planet *>::iterator i = planets.begin(); i != planets.end();
         ++i) {
        delete (*i);
    }
}

int
Star::GetNumPlanets() {
    return (int)planets.size();
}

Planet *
Star::GetPlanet(int idx) {
    Planet *result = NULL;

    if ((idx >= 0) && (idx < (int)planets.size())) {
        result = planets[idx];
    }

    return result;
}

Planet *
Star::GetPlanetByID(ID id) {
    Planet *result = NULL;

    map<ID, Planet *>::iterator i = planetsByID.find(id);
    if (i != planetsByID.end()) {
        result = i->second;
    }

    return result;
}

SpectralClass
Star::SpectralClassFromString(const string &s) {
    SpectralClass result = SC_INVALID;

    if (s == "M") {
        result = SC_M;
    } else if (s == "K") {
        result = SC_K;
    } else if (s == "G") {
        result = SC_G;
    } else if (s == "F") {
        result = SC_F;
    } else if (s == "A") {
        result = SC_A;
    } else if (s == "B") {
        result = SC_B;
    } else if (s == "O") {
        result = SC_O;
    }

    return result;
}

Planet::Planet()
    : id(-1), hostStarID(-1), name(""), size(PS_INVALID), type(PT_INVALID),
      color(""), temperature(PTMP_INVALID), gravity(PG_INVALID),
      atmosphere(PA_INVALID), weather(PW_INVALID) {}

Planet::Planet(const Planet &rhs) { *this = rhs; }

Planet::~Planet() {}

Planet &
Planet::operator=(const Planet &rhs) {
    id = rhs.id;
    hostStarID = rhs.hostStarID;
    name = rhs.name;
    size = rhs.size;
    type = rhs.type;
    color = rhs.color;
    temperature = rhs.temperature;
    gravity = rhs.gravity;
    atmosphere = rhs.atmosphere;
    weather = rhs.weather;
    landable = rhs.landable;

    return *this;
}

PlanetSize
Planet::PlanetSizeFromString(const string &size) {
    PlanetSize result = PS_INVALID;

    if (size == "SMALL") {
        result = PS_SMALL;
    } else if (size == "MEDIUM") {
        result = PS_MEDIUM;
    } else if (size == "LARGE") {
        result = PS_LARGE;
    } else if (size == "HUGE") {
        result = PS_HUGE;
    }

    return result;
}

string
Planet::PlanetSizeToString(PlanetSize size) {
    string result;

    switch (size) {
    case PS_SMALL:
        result = "SMALL";
        break;

    case PS_MEDIUM:
        result = "MEDIUM";
        break;

    case PS_LARGE:
        result = "LARGE";
        break;

    case PS_HUGE:
        result = "HUGE";
        break;

    default:
        result = "INVALID";
    }

    return result;
}

PlanetType
Planet::PlanetTypeFromString(const string &type) {
    PlanetType result = PT_INVALID;

    if (type == "ASTEROID") {
        result = PT_ASTEROID;
    } else if (type == "ROCKY") {
        result = PT_ROCKY;
    } else if (type == "FROZEN") {
        result = PT_FROZEN;
    } else if (type == "OCEANIC") {
        result = PT_OCEANIC;
    } else if (type == "MOLTEN") {
        result = PT_MOLTEN;
    } else if (type == "GAS GIANT") {
        result = PT_GASGIANT;
    } else if (type == "ACIDIC") {
        result = PT_ACIDIC;
    }

    return result;
}

string
Planet::PlanetTypeToString(PlanetType type) {
    string result;

    switch (type) {
    case PT_ASTEROID:
        result = "ASTEROID";
        break;

    case PT_ROCKY:
        result = "ROCKY";
        break;

    case PT_FROZEN:
        result = "FROZEN";
        break;

    case PT_OCEANIC:
        result = "OCEANIC";
        break;

    case PT_MOLTEN:
        result = "MOLTEN";
        break;

    case PT_GASGIANT:
        result = "GAS GIANT";
        break;

    case PT_ACIDIC:
        result = "ACIDIC";
        break;

    default:
        result = "INVALID";
        break;
    }

    return result;
}

PlanetTemperature
Planet::PlanetTemperatureFromString(const string &temperature) {
    PlanetTemperature result = PTMP_INVALID;

    if (temperature == "SUBARCTIC" || temperature == "SUB-ARCTIC") {
        result = PTMP_SUBARCTIC;
    } else if (temperature == "ARCTIC") {
        result = PTMP_ARCTIC;
    } else if (temperature == "TEMPERATE") {
        result = PTMP_TEMPERATE;
    } else if (temperature == "TROPICAL") {
        result = PTMP_TROPICAL;
    } else if (temperature == "SEARING") {
        result = PTMP_SEARING;
    } else if (temperature == "INFERNO") {
        result = PTMP_INFERNO;
    }

    return result;
}

string
Planet::PlanetTemperatureToString(PlanetTemperature temperature) {
    string result;

    switch (temperature) {
    case PTMP_SUBARCTIC:
        result = "SUBARCTIC";
        break;

    case PTMP_ARCTIC:
        result = "ARCTIC";
        break;

    case PTMP_TEMPERATE:
        result = "TEMPERATE";
        break;

    case PTMP_TROPICAL:
        result = "TROPICAL";
        break;

    case PTMP_SEARING:
        result = "SEARING";
        break;

    case PTMP_INFERNO:
        result = "INFERNO";
        break;

    default:
        result = "INVALID";
        break;
    }

    return result;
}

PlanetGravity
Planet::PlanetGravityFromString(const string &gravity) {
    PlanetGravity result = PG_INVALID;

    if (gravity == "NEGLIGIBLE") {
        result = PG_NEGLIGIBLE;
    } else if (gravity == "VERY LOW") {
        result = PG_VERYLOW;
    } else if (gravity == "LOW") {
        result = PG_LOW;
    } else if (gravity == "OPTIMAL") {
        result = PG_OPTIMAL;
    } else if (gravity == "VERY HEAVY") {
        result = PG_VERYHEAVY;
    } else if (gravity == "CRUSHING") {
        result = PG_CRUSHING;
    }

    return result;
}

string
Planet::PlanetGravityToString(PlanetGravity gravity) {
    string result;

    switch (gravity) {
    case PG_NEGLIGIBLE:
        result = "NEGLIGIBLE";
        break;

    case PG_VERYLOW:
        result = "VERY LOW";
        break;

    case PG_LOW:
        result = "LOW";
        break;

    case PG_OPTIMAL:
        result = "OPTIMAL";
        break;

    case PG_VERYHEAVY:
        result = "VERY HEAVY";
        break;

    case PG_CRUSHING:
        result = "CRUSHING";
        break;

    default:
        result = "INVALID";
        break;
    }

    return result;
}

PlanetAtmosphere
Planet::PlanetAtmosphereFromString(const string &atmosphere) {
    PlanetAtmosphere result = PA_INVALID;

    if (atmosphere == "NONE") {
        result = PA_NONE;
    } else if (atmosphere == "TRACEGASES" || atmosphere == "TRACE GASES") {
        result = PA_TRACEGASES;
    } else if (atmosphere == "BREATHABLE") {
        result = PA_BREATHABLE;
    } else if (atmosphere == "ACIDIC") {
        result = PA_ACIDIC;
    } else if (atmosphere == "TOXIC") {
        result = PA_TOXIC;
    } else if (atmosphere == "FIRESTORM") {
        result = PA_FIRESTORM;
    }

    return result;
}

string
Planet::PlanetAtmosphereToString(PlanetAtmosphere atmosphere) {
    string result;

    switch (atmosphere) {
    case PA_NONE:
        result = "NONE";
        break;

    case PA_TRACEGASES:
        result = "TRACEGASES";
        break;

    case PA_BREATHABLE:
        result = "BREATHABLE";
        break;

    case PA_ACIDIC:
        result = "ACIDIC";
        break;

    case PA_TOXIC:
        result = "TOXIC";
        break;

    case PA_FIRESTORM:
        result = "FIRESTORM";
        break;

    default:
        result = "INVALID";
        break;
    }

    return result;
}

PlanetWeather
Planet::PlanetWeatherFromString(const string &weather) {
    PlanetWeather result = PW_INVALID;

    if (weather == "NONE") {
        result = PW_NONE;
    } else if (weather == "CALM") {
        result = PW_CALM;
    } else if (weather == "MODERATE") {
        result = PW_MODERATE;
    } else if (weather == "VIOLENT") {
        result = PW_VIOLENT;
    } else if (weather == "VERYVIOLENT") {
        result = PW_VERYVIOLENT;
    }

    return result;
}

DataMgr::DataMgr() : m_initialized(false) {}

DataMgr::~DataMgr() {
    for (vector<Star *>::iterator i = stars.begin(); i != stars.end(); ++i) {
        delete (*i);
    }

    for (auto &i : humanNames) {
        delete (i->second);
    }
    for (auto &i : g_game->dataMgr->flux) {
        delete i;
    }
    flux.clear();
}

int
DataMgr::GetNumItems() {
    return (int)items.size();
}

Item *
DataMgr::GetItem(int idx) {
    Item *result = NULL;

    if ((idx >= 0) && (idx < (int)items.size())) {
        result = items[idx];
    }

    return result;
}

Item *
DataMgr::GetItemByID(ID id) {
    Item *result = NULL;

    map<ID, Item *>::iterator i = itemsByID.find(id);
    if (i != itemsByID.end()) {
        result = i->second;
    }

    return result;
}

Item *
DataMgr::GetItem(const string &name) {
    Item *result = NULL;
    for (vector<Item *>::iterator i = items.begin(); i != items.end(); ++i) {
        if ((*i)->name == name)
            result = *i;
    }
    return result;
}

int
DataMgr::GetNumStars() {
    return (int)stars.size();
}

Star *
DataMgr::GetStar(int idx) {
    Star *result = NULL;

    if ((idx >= 0) && (idx < (int)stars.size())) {
        result = stars[idx];
    }

    return result;
}

Star *
DataMgr::GetStarByID(ID id) {
    Star *result = NULL;

    map<ID, Star *>::iterator i = starsByID.find(id);
    if (i != starsByID.end()) {
        result = i->second;
    }

    return result;
}

Star *
DataMgr::GetStarByLocation(CoordValue x, CoordValue y) {
    Star *result = NULL;

    map<pair<CoordValue, CoordValue>, Star *>::iterator i =
        starsByLocation.find(make_pair(x, y));
    if (i != starsByLocation.end()) {
        result = i->second;
    }

    return result;
}

int
DataMgr::GetNumFlux() {
    return (int)flux.size();
}

const Flux *
DataMgr::GetFlux(int idx) {
    Flux *result = nullptr;

    if ((idx >= 0) && (idx < (int)flux.size())) {
        result = flux[idx];
    }

    return result;
}

const Flux *
DataMgr::GetFluxByLocation(CoordValue x, CoordValue y) {
    auto i = fluxByLocation.find(make_pair(x, y));
    if (i != fluxByLocation.end()) {
        return i->second;
    }
    return nullptr;
}

Planet *
DataMgr::GetPlanetByID(ID id) {
    Planet *result = NULL;

    map<ID, Planet *>::iterator i = allPlanetsByID.find(id);
    if (i != allPlanetsByID.end()) {
        result = i->second;
    }

    return result;
}

string
DataMgr::GetRandMixedName() {
    if (humanNames.size() == 0) {
        g_game->message("ERROR: The human names data has not been loaded.");
        return "<Error>";
    }

    int randomID = Util::Random(0, (int)humanNames.size() - 1);
    int randomID2 = Util::Random(0, (int)humanNames.size() - 1);

    return *humanNames[randomID]->first + " " + *humanNames[randomID2]->second;
}

bool
DataMgr::Initialize() {
    if (m_initialized)
        return true;

    m_initialized = true;

    if (!LoadItems())
        return false;

    if (!LoadGalaxy())
        return false;

    if (!LoadHumanNames())
        return false;

    return true;
}

bool
DataMgr::LoadItems() {
    string xml_file = Util::resource_path(ITEMS_FILE);

    // open the strfltitems.xml file
    TiXmlDocument doc(xml_file);

    if (!doc.LoadFile())
        return false;

    // load root of xml hierarchy
    TiXmlElement *itemSet = doc.FirstChildElement("items");
    if (itemSet == NULL)
        return false;

    // load all items
    TiXmlElement *item = itemSet->FirstChildElement("item");
    while (item != NULL) {
        Item newItem;
        TiXmlHandle itemHandle(item);

        TiXmlText *text;

        text = itemHandle.FirstChild("ID").FirstChild().Text();
        if (text != NULL) {
            newItem.id = atoi(text->Value());
        }

        text = itemHandle.FirstChild("Type").FirstChild().Text();
        if (text != NULL) {
            newItem.itemType = Item::ItemTypeFromString(text->Value());
        }

        text = itemHandle.FirstChild("Name").FirstChild().Text();
        if (text != NULL) {
            newItem.name = text->Value();
        }

        text = itemHandle.FirstChild("Value").FirstChild().Text();
        if (text != NULL) {
            newItem.value = atof(text->Value());
        }

        text = itemHandle.FirstChild("Size").FirstChild().Text();
        if (text != NULL) {
            newItem.size = atof(text->Value());
        }

        text = itemHandle.FirstChild("Speed").FirstChild().Text();
        if (text != NULL) {
            newItem.speed = atof(text->Value());
        }

        text = itemHandle.FirstChild("Danger").FirstChild().Text();
        if (text != NULL) {
            newItem.danger = atof(text->Value());
        }

        text = itemHandle.FirstChild("Damage").FirstChild().Text();
        if (text != NULL) {
            newItem.damage = atof(text->Value());
        }

        text = itemHandle.FirstChild("Age").FirstChild().Text();
        if (text != NULL) {
            newItem.itemAge = Item::ItemAgeFromString(text->Value());
        }

        text = itemHandle.FirstChild("ShipRepairMetal").FirstChild().Text();
        if (text != NULL) {
            string v(text->Value());
            newItem.shipRepairMetal = v == "true";
        }

        text = itemHandle.FirstChild("BlackMarket").FirstChild().Text();
        if (text != NULL) {
            string v(text->Value());
            newItem.blackMarketItem = v == "true";
        }

        text = itemHandle.FirstChild("Portrait").FirstChild().Text();
        if (text != NULL) {
            newItem.portrait = text->Value();
        }

        // new property for Artifacts
        text = itemHandle.FirstChild("planetid").FirstChild().Text();
        if (text != NULL) {
            newItem.planetid = atof(text->Value());
        }
        // new property for Artifacts
        text = itemHandle.FirstChild("x").FirstChild().Text();
        if (text != NULL) {
            newItem.x = atof(text->Value());
        }
        // new property for Artifacts
        text = itemHandle.FirstChild("y").FirstChild().Text();
        if (text != NULL) {
            newItem.y = atof(text->Value());
        }

        // new property for Ruins (and now for lifeForms too).
        text = itemHandle.FirstChild("Description").FirstChild().Text();
        if (text != NULL) {
            newItem.description = text->Value();
        }

        // make sure an item with this ID doesn't already exist
        Item *existingItem = GetItemByID(newItem.id);
        if (existingItem == NULL) {
            // add the item
            Item *toAdd = new Item(newItem);
            items.push_back(toAdd);
            itemsByID[newItem.id] = toAdd;
        }

        item = item->NextSiblingElement("item");
    }

    return true;
}

bool
DataMgr::LoadGalaxy() {
    string xml_file = Util::resource_path(GALAXY_FILE);
    TiXmlDocument doc(xml_file);

    if (!doc.LoadFile())
        return false;

    TiXmlElement *galaxy = doc.FirstChildElement("galaxy");
    if (galaxy == NULL)
        return false;

    // load all stars first, since the planets reference them
    TiXmlElement *star = galaxy->FirstChildElement("star");
    while (star != NULL) {
        Star newStar;
        TiXmlHandle starHandle(star);

        TiXmlText *text;

        text = starHandle.FirstChild("id").FirstChild().Text();
        if (text != NULL) {
            newStar.id = atoi(text->Value());
        }

        text = starHandle.FirstChild("name").FirstChild().Text();
        if (text == NULL)
            newStar.name = "";
        else
            newStar.name = text->Value();

        text = starHandle.FirstChild("x").FirstChild().Text();
        if (text != NULL) {
            newStar.x = atoi(text->Value());
        }

        text = starHandle.FirstChild("y").FirstChild().Text();
        if (text != NULL) {
            newStar.y = atoi(text->Value());
        }

        text = starHandle.FirstChild("spectralclass").FirstChild().Text();
        if (text != NULL) {
            newStar.spectralClass =
                Star::SpectralClassFromString(text->Value());
        }

        text = starHandle.FirstChild("color").FirstChild().Text();
        if (text != NULL) {
            newStar.color = text->Value();
        }

        text = starHandle.FirstChild("temperature").FirstChild().Text();
        if (text != NULL) {
            newStar.temperature = atol(text->Value());
        }

        text = starHandle.FirstChild("mass").FirstChild().Text();
        if (text != NULL) {
            newStar.mass = atol(text->Value());
        }

        text = starHandle.FirstChild("radius").FirstChild().Text();
        if (text != NULL) {
            newStar.radius = atol(text->Value());
        }

        text = starHandle.FirstChild("luminosity").FirstChild().Text();
        if (text != NULL) {
            newStar.luminosity = atol(text->Value());
        }

        // make sure a star with this ID doesn't already exist
        Star *existingStar = GetStarByID(newStar.id);
        if (existingStar == NULL) {
            // add the star
            Star *toAdd = new Star(newStar);
            stars.push_back(toAdd);
            starsByID[newStar.id] = toAdd;
            starsByLocation[make_pair(newStar.x, newStar.y)] = toAdd;
        }

        star = star->NextSiblingElement("star");
    }

    // now load all planets
    TiXmlElement *planet = galaxy->FirstChildElement("planet");
    while (planet != NULL) {
        Planet newPlanet;
        TiXmlHandle planetHandle(planet);

        TiXmlText *text;

        text = planetHandle.FirstChild("id").FirstChild().Text();
        if (text != NULL) {
            newPlanet.id = atoi(text->Value());
        }

        text = planetHandle.FirstChild("hoststar").FirstChild().Text();
        if (text != NULL) {
            newPlanet.hostStarID = atoi(text->Value());
        }

        text = planetHandle.FirstChild("name").FirstChild().Text();
        if (text != NULL) {
            newPlanet.name = text->Value();
        }

        text = planetHandle.FirstChild("size").FirstChild().Text();
        if (text != NULL) {
            newPlanet.size = Planet::PlanetSizeFromString(text->Value());
        }

        text = planetHandle.FirstChild("type").FirstChild().Text();
        if (text != NULL) {
            newPlanet.type = Planet::PlanetTypeFromString(text->Value());
        }

        text = planetHandle.FirstChild("color").FirstChild().Text();
        if (text != NULL) {
            newPlanet.color = text->Value();
        }

        text = planetHandle.FirstChild("temperature").FirstChild().Text();
        if (text != NULL) {
            newPlanet.temperature =
                Planet::PlanetTemperatureFromString(text->Value());
        }

        text = planetHandle.FirstChild("gravity").FirstChild().Text();
        if (text != NULL) {
            newPlanet.gravity = Planet::PlanetGravityFromString(text->Value());
        }

        text = planetHandle.FirstChild("atmosphere").FirstChild().Text();
        if (text != NULL) {
            newPlanet.atmosphere =
                Planet::PlanetAtmosphereFromString(text->Value());
        }

        text = planetHandle.FirstChild("weather").FirstChild().Text();
        if (text != NULL) {
            newPlanet.weather = Planet::PlanetWeatherFromString(text->Value());
        }

        // added to prevent landing on homeworlds
        newPlanet.landable = true;
        text = planetHandle.FirstChild("landable").FirstChild().Text();
        if (text != NULL) {
            string str = Util::ToString(text->Value());
            // all planets are landable unless otherwise specified in galaxy
            // data
            if (str == "false")
                newPlanet.landable = false;
        }

        // sanity checks
        if (newPlanet.size == PS_INVALID || newPlanet.type == PT_INVALID
            || newPlanet.temperature == PTMP_INVALID
            || newPlanet.gravity == PG_INVALID
            || newPlanet.atmosphere == PA_INVALID) {
            std::string msg =
                "loadGalaxy: error loading planet #" + to_string(newPlanet.id);
            msg += " , name -- " + newPlanet.name + " --";
            msg += " , size " + Planet::PlanetSizeToString(newPlanet.size);
            msg += " , type " + Planet::PlanetTypeToString(newPlanet.type);
            msg += " , temperature "
                   + Planet::PlanetTemperatureToString(newPlanet.temperature);
            msg += " , gravity "
                   + Planet::PlanetGravityToString(newPlanet.gravity);
            msg += " , atmosphere "
                   + Planet::PlanetAtmosphereToString(newPlanet.atmosphere);
            msg += "\n";
            ALLEGRO_DEBUG("%s\n", msg.c_str());
            ALLEGRO_ASSERT(0);
        }

        // make sure the host star does exist
        Star *hostStar = GetStarByID(newPlanet.hostStarID);
        if (hostStar != NULL) {
            // make sure a planet with this ID doesn't already exist in the host
            // star
            Planet *existingPlanet = hostStar->GetPlanetByID(newPlanet.id);
            if (existingPlanet == NULL) {
                // add the planet to the host star
                Planet *toAdd = new Planet(newPlanet);
                hostStar->planets.push_back(toAdd);
                hostStar->planetsByID[newPlanet.id] = toAdd;
            }
        }

        // add to the large list of all planets (not bound by host star)
        Planet *toAdd = new Planet(newPlanet);
        allPlanets.push_back(toAdd);
        allPlanetsByID[newPlanet.id] = toAdd;

        planet = planet->NextSiblingElement("planet");
    }

    // now load all flux
    TiXmlElement *flux = galaxy->FirstChildElement("flux");
    while (flux != NULL) {
        ID flux_id = -1;
        Point2D endpoint_1 = {-1, -1};
        Point2D endpoint_2 = {-1, -1};
        TiXmlHandle fluxHandle(flux);

        TiXmlText *text;

        text = fluxHandle.FirstChild("id").FirstChild().Text();
        if (text != NULL) {
            flux_id = atoi(text->Value());
        }

        TiXmlHandle endpoint = fluxHandle.FirstChildElement("endpoint");
        text = endpoint.FirstChild("x").FirstChild().Text();
        if (text != nullptr) {
            endpoint_1.x = atoi(text->Value());
        }
        text = endpoint.FirstChild("y").FirstChild().Text();
        if (text != nullptr) {
            endpoint_1.y = atoi(text->Value());
        }
        endpoint = fluxHandle.ChildElement("endpoint", 1);
        text = endpoint.FirstChild("x").FirstChild().Text();
        if (text != nullptr) {
            endpoint_2.x = atoi(text->Value());
        }
        text = endpoint.FirstChild("y").FirstChild().Text();
        if (text != nullptr) {
            endpoint_2.y = atoi(text->Value());
        }

        // sanity checks
        if ((flux_id < 0 || flux_id >= MAX_FLUX)
            || (endpoint_1.x < 0 || endpoint_1.x >= 250)
            || (endpoint_1.y < 0 || endpoint_1.y >= 220)
            || (endpoint_2.x < 0 || endpoint_2.x >= 250)
            || (endpoint_2.y < 0 || endpoint_2.y >= 220)) {
            std::string msg =
                "loadGalaxy: error loading flux #" + to_string(flux_id);
            msg += " , endpoint_1 (" + to_string(endpoint_1.x) + ", "
                   + to_string(endpoint_1.y) + ")";
            msg += " , endpoint_2 (" + to_string(endpoint_2.x) + ", "
                   + to_string(endpoint_2.y) + ")\n";
            ALLEGRO_ERROR("%s\n", msg.c_str());
            ALLEGRO_ASSERT(0);
        }

        // add to the DataMgr
        Flux *toAdd = new Flux(flux_id, endpoint_1, endpoint_2);
        this->flux.push_back(toAdd);
        this->fluxByLocation[make_pair(endpoint_1.x, endpoint_1.y)] = toAdd;
        this->fluxByLocation[make_pair(endpoint_2.x, endpoint_2.y)] = toAdd;

        flux = flux->NextSiblingElement("flux");
    }

    return true;
}

bool
DataMgr::LoadHumanNames() {
    string xml_file = Util::resource_path(HUMANNAMES_FILE);

    TiXmlDocument doc(xml_file);

    if (!doc.LoadFile())
        return false;

    TiXmlElement *people = doc.FirstChildElement("Names");
    if (people == NULL)
        return false;

    // load all stars first, since the planets reference them
    TiXmlElement *name = people->FirstChildElement("Name");
    while (name != NULL) {
        string firstName;
        string lastName;
        TiXmlHandle nameHandle(name);

        TiXmlText *text;

        // Check for first name
        text = nameHandle.FirstChild("First").FirstChild().Text();
        if (text == NULL)
            firstName = "";
        else
            firstName = text->Value();

        // Check for last name
        text = nameHandle.FirstChild("Last").FirstChild().Text();
        if (text == NULL)
            lastName = "";
        else
            lastName = text->Value();

        // Add the name to the list
        std::pair<std::string *, std::string *> *newName =
            new std::pair<std::string *, std::string *>;
        newName->first = new string(firstName);
        newName->second = new string(lastName);
        humanNames.push_back(newName);

        // Grab next name
        name = name->NextSiblingElement("Name");
    }

    return true;
}

Items::Items() { Reset(); }

Items::Items(const Items &rhs) { *this = rhs; }

Items::~Items() {}

Items &
Items::operator=(const Items &rhs) {
    Reset();

    for (vector<pair<ID, int>>::const_iterator i = rhs.stacks.begin();
         i != rhs.stacks.end();
         ++i) {
        stacks.push_back(*i);
    }

    return *this;
}

void
Items::Reset() {
    stacks.clear();
}

bool
Items::Serialize(Archive &ar) {
    string ClassName = "Items";
    int Schema = 0;

    if (ar.IsStoring()) {
        ar << ClassName;
        ar << Schema;

        int numStacks = (int)stacks.size();
        ar << numStacks;
        for (vector<pair<ID, int>>::iterator i = stacks.begin();
             i != stacks.end();
             ++i) {
            int id = i->first;
            int numItems = i->second;

            ar << id;
            ar << numItems;
        }
    } else {
        Reset();

        string LoadClassName;
        ar >> LoadClassName;
        if (LoadClassName != ClassName)
            return false;

        int LoadSchema;
        ar >> LoadSchema;
        if (LoadSchema > Schema)
            return false;

        int numStacks;
        ar >> numStacks;
        for (int i = 0; i < numStacks; i++) {
            int id;
            ar >> id;
            int numItems;
            ar >> numItems;
            stacks.push_back(make_pair(id, numItems));
        }
    }

    return true;
}

int
Items::GetNumStacks() {
    return (int)stacks.size();
}

void
Items::GetStack(int idx, Item &item, int &numItemsInStack) {
    item.Reset();
    numItemsInStack = 0;

    if (idx < 0)
        return;
    if (idx >= GetNumStacks())
        return;

    pair<ID, int> stack = stacks[idx];

    Item *pItem = g_game->dataMgr->GetItemByID(stack.first);
    if (pItem == NULL)
        return;

    item = *pItem;
    numItemsInStack = stack.second;
}

void
Items::AddItems(ID id, int numItemsToAdd) {
    if (numItemsToAdd <= 0)
        return;

    Item *pItem = g_game->dataMgr->GetItemByID(id);
    if (pItem == NULL)
        return;

    bool itemStackExisted = false;
    for (vector<pair<ID, int>>::iterator i = stacks.begin(); i != stacks.end();
         ++i) {
        if (i->first == id) {
            pair<ID, int> &existingStack = *i;
            existingStack.second += numItemsToAdd;
            itemStackExisted = true;
            break;
        }
    }

    if (itemStackExisted)
        return;

    stacks.push_back(make_pair(id, numItemsToAdd));
}

void
Items::RemoveItems(ID id, int numItemsToRemove) {
    if (numItemsToRemove <= 0)
        return;

    Item *pItem = g_game->dataMgr->GetItemByID(id);
    if (pItem == NULL)
        return;

    for (vector<pair<ID, int>>::iterator i = stacks.begin(); i != stacks.end();
         ++i) {
        if (i->first == id) {
            pair<ID, int> &existingStack = *i;
            existingStack.second -= numItemsToRemove;
            if (existingStack.second <= 0)
                stacks.erase(i);
            break;
        }
    }
}

void
Items::SetItemCount(ID id, int numItems) {
    if (numItems < 0)
        return;

    Item *pItem = g_game->dataMgr->GetItemByID(id);
    if (pItem == NULL)
        return;

    bool itemStackExisted = false;
    for (vector<pair<ID, int>>::iterator i = stacks.begin(); i != stacks.end();
         ++i) {
        if (i->first == id) {
            pair<ID, int> &existingStack = *i;
            existingStack.second = numItems;
            itemStackExisted = true;
            if (numItems == 0)
                stacks.erase(i);
            break;
        }
    }

    if (itemStackExisted)
        return;

    stacks.push_back(make_pair(id, numItems));
}

void
Items::Get_Item_By_ID(int id, Item &item, int &num_in_stack) {
    item.Reset();
    num_in_stack = 0;

    Item *pItem = NULL;
    for (vector<pair<ID, int>>::iterator i = stacks.begin(); i != stacks.end();
         ++i) {
        if (i->first == id) {
            pItem = g_game->dataMgr->GetItemByID(i->first);
            num_in_stack = i->second;
            break;
        }
    }
    if (pItem == NULL) {
        return;
    }
    item = *pItem;
}
