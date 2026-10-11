#include "EconomyManager.h"
#include <iomanip>
#include <algorithm>

EconomyManager::EconomyManager() {
  stockpile.iron = 120;
  stockpile.crystal = 40;
  stockpile.hydrocarbon = 60;
  stockpile.deuterium = 25;
  stockpile.energy = 100;
  stockpile.maxEnergy = 200;
}

bool EconomyManager::canBuildMine(const Sphere::HexTile &tile, std::string &outReason) const {
  if (tile.deposit.type == Sphere::ResourceType::NONE) {
    outReason = "No mineral or ore deposit on this tile.";
    return false;
  }
  if (tile.structure.type != Sphere::StructureType::NONE) {
    outReason = "Tile already contains a structure (" + Sphere::getStructureName(tile.structure.type) + ").";
    return false;
  }
  if (tile.deposit.reserves <= 0) {
    outReason = "Deposit is completely depleted.";
    return false;
  }
  if (stockpile.iron < MINE_COST_IRON) {
    outReason = "Insufficient Iron (Need " + std::to_string(MINE_COST_IRON) + ", have " + std::to_string(stockpile.iron) + ").";
    return false;
  }
  if (stockpile.energy < MINE_COST_ENERGY) {
    outReason = "Insufficient Energy (Need " + std::to_string(MINE_COST_ENERGY) + ", have " + std::to_string(stockpile.energy) + ").";
    return false;
  }
  return true;
}

bool EconomyManager::buildMine(Sphere::HexTile &tile, std::string &outMsg) {
  std::string reason;
  if (!canBuildMine(tile, reason)) {
    outMsg = reason;
    return false;
  }

  stockpile.iron -= MINE_COST_IRON;
  stockpile.energy -= MINE_COST_ENERGY;

  tile.structure.type = Sphere::StructureType::SURFACE_MINE;
  tile.structure.owner = 0;
  tile.structure.level = 1;
  tile.structure.health = 100;
  tile.structure.maxHealth = 100;
  tile.structure.isActive = true;

  outMsg = "Constructed Surface Mine on Tile #" + std::to_string(tile.id) +
           " (" + Sphere::getResourceName(tile.deposit.type) + " deposit)! Cost: " +
           std::to_string(MINE_COST_IRON) + " Fe, " + std::to_string(MINE_COST_ENERGY) + " Energy.";
  return true;
}

bool EconomyManager::canUpgradeMine(const Sphere::HexTile &tile, std::string &outReason) const {
  if (tile.structure.type != Sphere::StructureType::SURFACE_MINE) {
    outReason = "Tile does not have a Surface Mine to upgrade.";
    return false;
  }
  if (tile.deposit.reserves <= 0) {
    outReason = "Deposit is depleted; cannot upgrade.";
    return false;
  }
  if (stockpile.iron < UPGRADE_COST_IRON) {
    outReason = "Insufficient Iron (Need " + std::to_string(UPGRADE_COST_IRON) + ", have " + std::to_string(stockpile.iron) + ").";
    return false;
  }
  if (stockpile.crystal < UPGRADE_COST_CRYSTAL) {
    outReason = "Insufficient Crystals (Need " + std::to_string(UPGRADE_COST_CRYSTAL) + ", have " + std::to_string(stockpile.crystal) + ").";
    return false;
  }
  return true;
}

bool EconomyManager::upgradeMine(Sphere::HexTile &tile, std::string &outMsg) {
  std::string reason;
  if (!canUpgradeMine(tile, reason)) {
    outMsg = reason;
    return false;
  }

  stockpile.iron -= UPGRADE_COST_IRON;
  stockpile.crystal -= UPGRADE_COST_CRYSTAL;

  tile.structure.type = Sphere::StructureType::DEEP_CORE_EXTRACTOR;
  tile.structure.level = 2;
  tile.structure.health = 200;
  tile.structure.maxHealth = 200;

  outMsg = "Upgraded to Deep Core Extractor on Tile #" + std::to_string(tile.id) +
           "! Extraction yield doubled (+100%).";
  return true;
}

bool EconomyManager::canBuildSolar(const Sphere::HexTile &tile, std::string &outReason) const {
  if (tile.terrainType == 0) {
    outReason = "Cannot construct Solar Array on Ocean tiles.";
    return false;
  }
  if (tile.structure.type != Sphere::StructureType::NONE) {
    outReason = "Tile already has a structure.";
    return false;
  }
  if (stockpile.iron < SOLAR_COST_IRON) {
    outReason = "Insufficient Iron (Need " + std::to_string(SOLAR_COST_IRON) + ", have " + std::to_string(stockpile.iron) + ").";
    return false;
  }
  return true;
}

bool EconomyManager::buildSolar(Sphere::HexTile &tile, std::string &outMsg) {
  std::string reason;
  if (!canBuildSolar(tile, reason)) {
    outMsg = reason;
    return false;
  }

  stockpile.iron -= SOLAR_COST_IRON;

  tile.structure.type = Sphere::StructureType::SOLAR_ARRAY;
  tile.structure.owner = 0;
  tile.structure.level = 1;
  tile.structure.health = 80;
  tile.structure.maxHealth = 80;
  tile.structure.isActive = true;

  outMsg = "Constructed Solar Array on Tile #" + std::to_string(tile.id) +
           "! Generates +" + std::to_string(SOLAR_GEN_ENERGY) + " Energy per turn.";
  return true;
}

bool EconomyManager::canRecruitUnit(int unitType, std::string &outReason) const {
  if (unitType == 0) { // Scout
    if (stockpile.iron < RECRUIT_SCOUT_IRON || stockpile.energy < RECRUIT_SCOUT_ENERGY) {
      outReason = "Need 30 Fe, 10 Energy for Scout.";
      return false;
    }
  } else if (unitType == 1) { // Warrior
    if (stockpile.iron < RECRUIT_WARRIOR_IRON || stockpile.energy < RECRUIT_WARRIOR_ENERGY) {
      outReason = "Need 80 Fe, 30 Energy for Warrior.";
      return false;
    }
  } else if (unitType == 2) { // Settler
    if (stockpile.iron < RECRUIT_SETTLER_IRON || stockpile.energy < RECRUIT_SETTLER_ENERGY) {
      outReason = "Need 60 Fe, 20 Energy for Settler/Worker.";
      return false;
    }
  }
  return true;
}

bool EconomyManager::payRecruitCost(int unitType) {
  std::string reason;
  if (!canRecruitUnit(unitType, reason)) return false;

  if (unitType == 0) {
    stockpile.iron -= RECRUIT_SCOUT_IRON;
    stockpile.energy -= RECRUIT_SCOUT_ENERGY;
  } else if (unitType == 1) {
    stockpile.iron -= RECRUIT_WARRIOR_IRON;
    stockpile.energy -= RECRUIT_WARRIOR_ENERGY;
  } else if (unitType == 2) {
    stockpile.iron -= RECRUIT_SETTLER_IRON;
    stockpile.energy -= RECRUIT_SETTLER_ENERGY;
  }
  return true;
}

TurnEconomyReport EconomyManager::processTurn(Sphere &planet) {
  currentTurn++;
  TurnEconomyReport report{};
  report.turn = currentTurn;

  // Process all structures across the planet
  for (auto &tile : planet.hexTiles) {
    if (tile.structure.owner != 0 || !tile.structure.isActive) continue;

    if (tile.structure.type == Sphere::StructureType::SOLAR_ARRAY) {
      report.activeSolar++;
      report.generatedEnergy += SOLAR_GEN_ENERGY;
      stockpile.energy = std::min(stockpile.energy + SOLAR_GEN_ENERGY, stockpile.maxEnergy);
    } else if (tile.structure.type == Sphere::StructureType::SURFACE_MINE ||
               tile.structure.type == Sphere::StructureType::DEEP_CORE_EXTRACTOR) {
      report.activeMines++;
      int upkeep = (tile.structure.type == Sphere::StructureType::SURFACE_MINE) ?
                   MINE_UPKEEP_ENERGY : UPGRADE_UPKEEP_ENERGY;

      if (stockpile.energy < upkeep) {
        report.messages.push_back("Mine on Tile #" + std::to_string(tile.id) + " halted: Not enough Energy!");
        continue;
      }

      stockpile.energy -= upkeep;
      report.consumedEnergy += upkeep;

      if (tile.deposit.reserves <= 0) {
        report.messages.push_back("Mine on Tile #" + std::to_string(tile.id) + " is idle: Deposit depleted.");
        continue;
      }

      int multiplier = (tile.structure.type == Sphere::StructureType::DEEP_CORE_EXTRACTOR) ? 2 : 1;
      int yield = std::min(tile.deposit.baseYield * multiplier, tile.deposit.reserves);
      tile.deposit.reserves -= yield;

      switch (tile.deposit.type) {
        case Sphere::ResourceType::IRON:
          stockpile.iron += yield;
          report.deltaIron += yield;
          break;
        case Sphere::ResourceType::CRYSTAL:
          stockpile.crystal += yield;
          report.deltaCrystal += yield;
          break;
        case Sphere::ResourceType::HYDROCARBON:
          stockpile.hydrocarbon += yield;
          report.deltaHydrocarbon += yield;
          break;
        case Sphere::ResourceType::DEUTERIUM:
          stockpile.deuterium += yield;
          report.deltaDeuterium += yield;
          break;
        default:
          break;
      }

      if (tile.deposit.reserves <= 0) {
        report.messages.push_back("ALERT: " + Sphere::getResourceName(tile.deposit.type) +
                                  " deposit on Tile #" + std::to_string(tile.id) + " has been EXHAUSTED!");
      }
    }
  }

  // Print Turn Summary to terminal
  std::cout << "\n============================== [ TURN " << currentTurn << " REPORT ] ==============================\n";
  std::cout << " Operational Mines: " << report.activeMines << " | Solar Plants: " << report.activeSolar << "\n";
  std::cout << " Production: ";
  if (report.deltaIron > 0) std::cout << "+" << report.deltaIron << " Iron ";
  if (report.deltaCrystal > 0) std::cout << "+" << report.deltaCrystal << " Crystal ";
  if (report.deltaHydrocarbon > 0) std::cout << "+" << report.deltaHydrocarbon << " Hydrocarbon ";
  if (report.deltaDeuterium > 0) std::cout << "+" << report.deltaDeuterium << " Deuterium ";
  if (report.deltaIron == 0 && report.deltaCrystal == 0 && report.deltaHydrocarbon == 0 && report.deltaDeuterium == 0) {
    std::cout << "No mines active.";
  }
  std::cout << "\n Energy: +" << report.generatedEnergy << " generated, -" << report.consumedEnergy << " upkeep\n";
  std::cout << " Current Stockpile: " << getStockpileString() << "\n";

  for (const auto &msg : report.messages) {
    std::cout << "  > " << msg << "\n";
  }
  std::cout << "========================================================================\n\n";

  return report;
}

std::string EconomyManager::getStockpileString() const {
  std::ostringstream ss;
  ss << "Fe:" << stockpile.iron << " | Cry:" << stockpile.crystal
     << " | Fuel:" << stockpile.hydrocarbon << " | Deu:" << stockpile.deuterium
     << " | Pwr:" << stockpile.energy << "/" << stockpile.maxEnergy;
  return ss.str();
}

void EconomyManager::printInspection(const Sphere::HexTile &tile, int tileId) const {
  std::cout << "\n+------------------------- [ TILE #" << std::setw(3) << tileId << " INSPECTION ] -------------------------+\n";
  std::cout << "| Biome: " << std::left << std::setw(20) << Sphere::getTerrainName(tile.terrainType)
            << " | Altitude: " << (tile.isPentagon ? "Volcanic Crater (Pentagon)" : "Standard Hex") << "\n";

  if (tile.deposit.type != Sphere::ResourceType::NONE) {
    std::cout << "| Resource: " << std::left << std::setw(17) << Sphere::getResourceName(tile.deposit.type)
              << " | Reserves: " << tile.deposit.reserves << "/" << tile.deposit.maxReserves
              << " | Yield: +" << tile.deposit.baseYield << "/turn\n";
  } else {
    std::cout << "| Resource: None (Barren terrain)\n";
  }

  if (tile.structure.type != Sphere::StructureType::NONE) {
    std::cout << "| Structure: " << std::left << std::setw(22) << Sphere::getStructureName(tile.structure.type)
              << " | Level: " << tile.structure.level
              << " | Status: " << (tile.structure.isActive ? "OPERATIONAL" : "IDLE") << "\n";
  } else {
    std::cout << "| Structure: None (Undeveloped)\n";
  }

  std::cout << "+-------------------------------------------------------------------------+\n";
  std::cout << "| Actions: [M] Build Mine (40 Fe, 20 Pwr) | [U] Upgrade Mine (75 Fe, 35 Cry)\n";
  std::cout << "|          [B] Build Solar (50 Fe)        | [P] Recruit Unit (Settler/Warrior/Scout)\n";
  std::cout << "+-------------------------------------------------------------------------+\n\n";
}
