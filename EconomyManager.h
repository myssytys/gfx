#ifndef ECONOMY_MANAGER_H
#define ECONOMY_MANAGER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Sphere.h"

struct PlayerStockpile {
  int iron = 120;
  int crystal = 40;
  int hydrocarbon = 60;
  int deuterium = 25;
  int energy = 100;
  int maxEnergy = 200;
};

struct TurnEconomyReport {
  int turn = 1;
  int deltaIron = 0;
  int deltaCrystal = 0;
  int deltaHydrocarbon = 0;
  int deltaDeuterium = 0;
  int generatedEnergy = 0;
  int consumedEnergy = 0;
  int activeMines = 0;
  int activeSolar = 0;
  std::vector<std::string> messages;
};

class EconomyManager {
public:
  PlayerStockpile stockpile;
  int currentTurn = 1;

  EconomyManager();

  // Building & upgrade costs
  static const int MINE_COST_IRON = 40;
  static const int MINE_COST_ENERGY = 20;
  static const int MINE_UPKEEP_ENERGY = 5;

  static const int UPGRADE_COST_IRON = 75;
  static const int UPGRADE_COST_CRYSTAL = 35;
  static const int UPGRADE_UPKEEP_ENERGY = 10;

  static const int SOLAR_COST_IRON = 50;
  static const int SOLAR_GEN_ENERGY = 25;

  static const int RECRUIT_SCOUT_IRON = 30;
  static const int RECRUIT_SCOUT_ENERGY = 10;

  static const int RECRUIT_WARRIOR_IRON = 80;
  static const int RECRUIT_WARRIOR_ENERGY = 30;

  static const int RECRUIT_SETTLER_IRON = 60;
  static const int RECRUIT_SETTLER_ENERGY = 20;

  // Actions
  bool canBuildMine(const Sphere::HexTile &tile, std::string &outReason) const;
  bool buildMine(Sphere::HexTile &tile, std::string &outMsg);

  bool canUpgradeMine(const Sphere::HexTile &tile, std::string &outReason) const;
  bool upgradeMine(Sphere::HexTile &tile, std::string &outMsg);

  bool canBuildSolar(const Sphere::HexTile &tile, std::string &outReason) const;
  bool buildSolar(Sphere::HexTile &tile, std::string &outMsg);

  bool canRecruitUnit(int unitType, std::string &outReason) const;
  bool payRecruitCost(int unitType);

  // Turn processing
  TurnEconomyReport processTurn(Sphere &planet);

  // Formatted string summary for HUD / console
  std::string getStockpileString() const;
  void printInspection(const Sphere::HexTile &tile, int tileId) const;
};

#endif
