#include <string>
#include "game.h"
#include "randomUtil.h"

using namespace game;
using namespace std;

// === MAP ===

Map game::createMap(RandomUtil& randomUtil, int size, float junkFillProbability) {
    Map map(size, vector<Cell>(size));

    for (int r = 0; r < size; r++) {
        for (int c = 0; c < size; c++) {
            Cell cell = CELL_TYPE_FREE;

            if (r == 0 || r == size - 1 || c == 0 || c == size - 1) {
                cell = CELL_TYPE_WALL;
            } else if (randomUtil.shouldHappen(junkFillProbability)) {
                cell = CELL_TYPE_JUNK;
            }

            map[r][c] = cell;
        }
    }

    return map;
}

Cell game::getCellByCoordinate(const Map& map, int x, int y) {
    return map[y][x];
}

void game::setCellByCoordinate(Map &map, int x, int y, Cell cell) {
    map[y][x] = cell;
}

vector<Cell> game::getSurroundingCellsByCoordinate(const Map &map, int x, int y, bool includeDiagonals) {
    vector<Cell> cells;
    cells.push_back(getCellByCoordinate(map, x - 1, y));
    cells.push_back(getCellByCoordinate(map, x + 1, y));
    cells.push_back(getCellByCoordinate(map, x, y - 1));
    cells.push_back(getCellByCoordinate(map, x, y + 1));

    if (includeDiagonals) {
        cells.push_back(getCellByCoordinate(map, x + 1, y - 1));
        cells.push_back(getCellByCoordinate(map, x + 1, y + 1));
        cells.push_back(getCellByCoordinate(map, x - 1, y + 1));
        cells.push_back(getCellByCoordinate(map, x - 1, y - 1));
    }

    return cells;
}

vector<Cell> game::getStateCells(const Map &map, int x, int y, bool includeDiagonals) {
    vector<Cell> cells = getSurroundingCellsByCoordinate(map, x, y, includeDiagonals);
    cells.push_back(getCellByCoordinate(map, x, y));
    return cells;
}

int game::getNumCellsOfType(const Map &map, Cell cell) {
    int num = 0;

    const int SIZE = map.size();

    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (getCellByCoordinate(map, x, y) == cell) {
                num++;
            }
        }
    }

    return num;
}

string game::toString(Map map) {
    string output;

    const int SIZE = (int)map.size();

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            Cell cell = getCellByCoordinate(map, x, y);

            if (cell == CELL_TYPE_FREE) {
                output += " ";
            } else if (cell == CELL_TYPE_WALL) {
                output += "#";
            } else if (cell == CELL_TYPE_JUNK) {
                output += "x";
            }
        }

        output += "\n";
    }

    return output;
}

// === Game ===

GameState::GameState(Map map, int playerX, int playerY) : map(map), maxScore(getNumCellsOfType(map, CELL_TYPE_JUNK)), playerX(playerX), playerY(playerY), score(0), numActionsPerformed(0) {}

GameState game::createGame(Map map, int startingX, int startingY) {
    GameState gameState(map, startingX, startingY);
    return gameState;
}

bool game::move(GameState &gameState, int xDir, int yDir) {
    gameState.numActionsPerformed++;

    int newX = gameState.playerX + xDir;
    int newY = gameState.playerY + yDir;

    Cell cell = getCellByCoordinate(gameState.map, newX, newY);

    if (cell == CELL_TYPE_WALL) {
        return false;
    }

    gameState.playerX = newX;
    gameState.playerY = newY;

    return true;
}

void game::pickup(GameState &gameState) {
    gameState.numActionsPerformed++;

    Cell cell = getCellByCoordinate(gameState.map, gameState.playerX, gameState.playerY);

    if (cell == CELL_TYPE_JUNK) {
        gameState.score++;
        setCellByCoordinate(gameState.map, gameState.playerX, gameState.playerY, CELL_TYPE_FREE);
    }
}

std::string game::toString(const GameState &gameState) {
    string mapOutput = toString(gameState.map);

    long positionInOutput = gameState.playerY * (gameState.map.size() + 1) + gameState.playerX;

    char playerSymbol = 'O';
    if (getCellByCoordinate(gameState.map, gameState.playerX, gameState.playerY) == CELL_TYPE_JUNK) {
        playerSymbol = '0';
    }

    mapOutput[positionInOutput] = playerSymbol;

    string info = "Score: ";
    info += to_string(gameState.score);
    info += "/";
    info += to_string(gameState.maxScore);
    info += " Actions: ";
    info += to_string(gameState.numActionsPerformed);
    info += "\n";

    return info + mapOutput;
}