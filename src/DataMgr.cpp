/*
        STARFLIGHT - THE LOST COLONY
        DataMgr.cpp - ?
        Author: Dave "coder1024" Calkins
        Date: 01/22/2007
*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include <yaml-cpp/yaml.h>

#include "Archive.h"
#include "DataMgr.h"
#include "Game.h"
#include "GameState.h"
#include "QuestMgr.h"
#include "Util.h"

ALLEGRO_DEBUG_CHANNEL("DataMgr")

using namespace std;

#define ITEMS_FILE "data/strfltitems.yaml"
#define GALAXY_FILE "data/galaxy.yaml"
#define HUMANNAMES_FILE "data/human.yaml"

namespace YAML {
template <> struct convert<Item> {
    static Node encode(const Item &item) {
        Node node;
        node["ID"] = item.id;
        node["Type"] = Item::ItemTypeToString(item.itemType);
        node["Name"] = item.name;
        node["Value"] = item.value;
        node["Size"] = item.size;
        node["Speed"] = item.speed;
        node["Danger"] = item.danger;
        node["Damage"] = item.damage;
        node["Age"] = Item::ItemAgeToString(item.itemAge);
        node["ShipRepairMetal"] = item.shipRepairMetal;
        node["BlackMarket"] = item.blackMarketItem;
        node["Portrait"] = item.portrait;
        node["planetid"] = item.planetid;
        node["y"] = item.y;
        node["x"] = item.x;
        node["Description"] = item.description;

        return node;
    }
    static bool decode(const Node &node, Item &item) {
        if (!node.IsMap()) {
            return false;
        }
        item.name = node["Name"].as<string>();
        item.id = node["ID"].as<int>();
        item.itemType = Item::ItemTypeFromString(node["Type"].as<string>());
        if (node["Value"]) {
            item.value = node["Value"].as<double>();
        }
        if (node["Size"]) {
            item.size = node["Size"].as<double>();
        }
        if (node["Speed"]) {
            item.speed = node["Speed"].as<double>();
        }
        if (node["Danger"]) {
            item.danger = node["Danger"].as<double>();
        }
        if (node["Damage"]) {
            item.damage = node["Damage"].as<double>();
        }
        if (node["Age"]) {
            item.itemAge = Item::ItemAgeFromString(node["Age"].as<string>());
        }
        if (node["ShipRepairMetal"]) {
            item.shipRepairMetal = node["ShipRepairMetal"].as<bool>();
        }
        if (node["BlackMarket"]) {
            item.blackMarketItem = node["BlackMarket"].as<bool>();
        }
        if (node["Portrait"]) {
            item.portrait = node["Portrait"].as<string>();
        }
        if (node["planetid"]) {
            item.planetid = node["planetid"].as<int>();
        }
        if (node["x"]) {
            item.x = node["x"].as<int>();
        }
        if (node["y"]) {
            item.y = node["y"].as<int>();
        }
        if (node["Description"]) {
            item.description = node["Description"].as<string>();
        }

        return true;
    }
};

template <> struct convert<Star> {
    static Node encode(const Star &star) {
        Node node;
        node["id"] = star.id;
        node["name"] = star.name;
        node["x"] = star.x;
        node["y"] = star.y;
        node["spectralclass"] = Star::SpectralClassToString(star.spectralClass);
        node["color"] = star.color;
        node["temperature"] = star.temperature;
        node["mass"] = star.mass;
        node["radius"] = star.radius;
        node["luminosity"] = star.luminosity;

        return node;
    }
    static bool decode(const Node &node, Star &star) {
        if (!node.IsMap()) {
            return false;
        }
        star.id = node["id"].as<ID>();
        star.name = node["name"].as<string>();
        star.x = node["x"].as<int>();
        star.y = node["y"].as<int>();
        star.spectralClass =
            Star::SpectralClassFromString(node["spectralclass"].as<string>());
        star.color = node["color"].as<string>();
        star.temperature = node["temperature"].as<unsigned long>();
        star.mass = node["mass"].as<double>();
        star.radius = node["radius"].as<double>();
        star.luminosity = node["luminosity"].as<double>();

        return true;
    }
};

template <> struct convert<Planet> {
    static Node encode(const Planet &planet) {
        Node node;
        node["id"] = planet.id;
        node["hoststar"] = planet.hostStarID;
        node["name"] = planet.name;
        node["size"] = Planet::PlanetSizeToString(planet.size);
        node["type"] = Planet::PlanetTypeToString(planet.type);
        node["color"] = planet.color;
        node["temperature"] =
            Planet::PlanetTemperatureToString(planet.temperature);
        node["gravity"] = Planet::PlanetGravityToString(planet.gravity);
        node["atmosphere"] =
            Planet::PlanetAtmosphereToString(planet.atmosphere);
        node["weather"] = Planet::PlanetWeatherToString(planet.weather);

        return node;
    }
    static bool decode(const Node &node, Planet &planet) {
        if (!node.IsMap()) {
            return false;
        }
        planet.id = node["id"].as<int>();
        planet.hostStarID = node["hoststar"].as<int>();
        planet.name = node["name"].as<string>();
        planet.size = Planet::PlanetSizeFromString(node["size"].as<string>());
        planet.type = Planet::PlanetTypeFromString(node["type"].as<string>());
        planet.color = node["color"].as<string>();
        planet.temperature = Planet::PlanetTemperatureFromString(
            node["temperature"].as<string>());
        planet.gravity =
            Planet::PlanetGravityFromString(node["gravity"].as<string>());
        planet.atmosphere =
            Planet::PlanetAtmosphereFromString(node["atmosphere"].as<string>());
        planet.weather =
            Planet::PlanetWeatherFromString(node["weather"].as<string>());
        if (node["landable"]) {
            planet.landable = node["landable"].as<bool>();
        } else {
            planet.landable = true;
        }

        return true;
    }
};

template <> struct convert<Point2D> {
    static Node encode(const Point2D &point) {
        Node node;
        node[0] = point.x;
        node[1] = point.y;

        return node;
    }
    static bool decode(const Node &node, Point2D &point) {
        if (!node.IsSequence()) {
            return false;
        }
        point.x = node[0].as<int>();
        point.y = node[1].as<int>();

        return true;
    }
};

template <> struct convert<Flux> {
    static Node encode(const Flux &flux) {
        auto ep1 = flux.get_endpoint1();
        auto ep2 = flux.get_endpoint2();
        Node node;
        node["id"] = flux.get_id();
        node["endpoints"][0] = ep1;
        node["endpoints"][1] = ep2;

        return node;
    }
    static bool decode(const Node &node, Flux &flux) {
        int id;
        Point2D ep1;
        Point2D ep2;
        if (!node.IsMap()) {
            return false;
        }
        id = node["id"].as<int>();
        ep1 = node["endpoints"][0].as<Point2D>();
        ep2 = node["endpoints"][1].as<Point2D>();

        flux = Flux(id, ep1, ep2);

        return true;
    }
};
} // namespace YAML

Item::Item() { Reset(); }

Item::Item(const Item &rhs) { *this = rhs; }

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
Star::GetNumPlanets() const {
    return (int)planets.size();
}

const Planet *
Star::GetPlanet(int idx) const {
    const Planet *result = nullptr;

    if ((idx >= 0) && (idx < (int)planets.size())) {
        result = planets[idx];
    }

    return result;
}

const Planet *
Star::GetPlanetByID(ID id) const {
    auto i = planetsByID.find(id);
    if (i != planetsByID.end()) {
        return i->second;
    }

    return nullptr;
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
    for (auto &i : stars) {
        delete i;
    }

    for (auto &i : flux) {
        delete i;
    }
    flux.clear();
}

const Star *
DataMgr::GetStarByID(ID id) const {
    Star *result = NULL;

    auto i = starsByID.find(id);
    if (i != starsByID.end()) {
        result = i->second;
    }

    return result;
}

const Star *
DataMgr::GetStarByLocation(CoordValue x, CoordValue y) const {
    Star *result = NULL;

    auto i = starsByLocation.find(make_pair(x, y));
    if (i != starsByLocation.end()) {
        result = i->second;
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

pair<string, string>
DataMgr::GetRandMixedName() {
    if (humanNames.size() == 0) {
        g_game->message("ERROR: The human names data has not been loaded.");
        return {"<Error>", "<Error>"};
    }

    int randomID = Util::Random(0, (int)humanNames.size() - 1);
    int randomID2 = Util::Random(0, (int)humanNames.size() - 1);

    return {humanNames[randomID].first, humanNames[randomID2].second};
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
    string items_file = Util::resource_path(ITEMS_FILE);

    YAML::Node node = YAML::LoadFile(items_file);
    auto yaml_items = node["items"];
    ALLEGRO_ASSERT(yaml_items.IsSequence());

    for (auto i : yaml_items) {
        ALLEGRO_ASSERT(i.IsMap());
        auto item = new Item(i.as<Item>());
        auto existing = itemsByID.find(item->id);
        if (existing == itemsByID.end()) {
            items.push_back(item);
            itemsByID[item->id] = item;
        }
    }

    return true;
}

bool
DataMgr::LoadGalaxy() {
    string galaxy_file = Util::resource_path(GALAXY_FILE);
    YAML::Node node = YAML::LoadFile(galaxy_file);
    auto galaxy = node["galaxy"];
    ALLEGRO_ASSERT(galaxy.IsSequence());

    for (auto i : galaxy) {
        ALLEGRO_ASSERT(i.IsMap());
        if (i["star"]) {
            Star *star = new Star(i["star"].as<Star>());
            auto existing = starsByID.find(star->id);
            if (existing == starsByID.end()) {
                stars.push_back(star);
                starsByID[star->id] = star;
                starsByLocation[{star->x, star->y}] = star;
            }
        } else if (i["planet"]) {
            Planet *planet = new Planet(i["planet"].as<Planet>());
            auto existing = allPlanetsByID.find(planet->id);
            if (existing == allPlanetsByID.end()) {
                allPlanets.push_back(planet);
                allPlanetsByID[planet->id] = planet;
            }
        } else if (i["flux"]) {
            Flux *f = new Flux(i["flux"].as<Flux>());
            flux.push_back(f);
            auto ep1 = f->get_endpoint1();
            auto ep2 = f->get_endpoint2();
            fluxByLocation[{ep1.x, ep1.y}] = f;
            fluxByLocation[{ep2.x, ep2.y}] = f;
        }
    }

    for (auto i = allPlanets.begin(), e = allPlanets.end(); i != e; ++i) {
        Planet *p = *i;
        Star *star = starsByID[p->hostStarID];
        star->planets.push_back(p);
        star->planetsByID[p->id] = p;
    }

    return true;
}

bool
DataMgr::LoadHumanNames() {
    string name_file = Util::resource_path(HUMANNAMES_FILE);
    YAML::Node node = YAML::LoadFile(name_file);
    auto names = node["Names"];

    if (!names.IsSequence()) {
        return false;
    }

    for (auto i : names) {
        ALLEGRO_ASSERT(i.IsMap());
        if (i["Name"]) {
            auto first = i["Name"]["First"].as<string>();
            auto last = i["Name"]["Last"].as<string>();
            humanNames.push_back({first, last});
        } else {
            return false;
        }
    }

    return true;
}

Items::Items() { Reset(); }

Items::Items(const Items &rhs) { *this = rhs; }

Items::~Items() {}

Items &
Items::operator=(const Items &rhs) {
    Reset();

    stacks = rhs.stacks;

    return *this;
}

void
Items::Reset() {
    stacks.clear();
}

InputArchive &
operator>>(InputArchive &ar, Items &items) {
    string class_name = string(Items::class_name);
    int schema = 0;

    items.Reset();

    string load_class_name;
    ar >> load_class_name;
    if (load_class_name != class_name) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + std::string(class_name) + ", got "
                + load_class_name);
    }

    int load_schema;
    ar >> load_schema;
    if (load_schema > schema) {
        std::error_code ec(al_get_errno(), std::system_category());
        throw std::system_error(
            ec,
            "Invalid save file: expected " + class_name + ", got "
                + load_class_name);
    }

    int num_stacks;
    ar >> num_stacks;
    for (int i = 0; i < num_stacks; i++) {
        int id;
        ar >> id;
        int numItems;
        ar >> numItems;
        items.stacks[id] = numItems;
    }
    return ar;
}

OutputArchive &
operator<<(OutputArchive &ar, const Items &items) {
    string class_name = string(Items::class_name);
    int schema = 0;

    ar << class_name;
    ar << schema;

    int num_stacks = static_cast<int>(items.stacks.size());
    ar << num_stacks;
    for (auto &i : items.stacks) {
        ar << i.first;
        ar << i.second;
    }

    return ar;
}

void
Items::AddItems(ID id, int numItemsToAdd) {
    if (numItemsToAdd <= 0)
        return;

    stacks[id] += numItemsToAdd;
}

void
Items::RemoveItems(ID id, int numItemsToRemove) {
    if (numItemsToRemove <= 0)
        return;
    stacks[id] -= numItemsToRemove;
    if (stacks[id] <= 0) {
        stacks.erase(id);
    }
}

void
Items::SetItemCount(ID id, int numItems) {
    if (numItems < 0)
        return;

    if (numItems == 0) {
        stacks.erase(id);
    } else {
        stacks[id] = numItems;
    }
}

int
Items::get_count(ID id) const {
    auto i = stacks.find(id);
    if (i != stacks.end()) {
        return i->second;
    } else {
        return 0;
    }
}
