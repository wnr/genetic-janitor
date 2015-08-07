#ifndef GENETIC_JANITOR_GAME_H
#define GENETIC_JANITOR_GAME_H

#include <vector>
#include "randomUtil.h"

namespace game {
    typedef int Cell;
    typedef std::vector<std::vector<Cell>> Map;

    const int CELL_TYPE_FREE = 0;
    const int CELL_TYPE_WALL = 1;
    const int CELL_TYPE_JUNK = 2;

    Map createMap(RandomUtil& randomUtil, int size, float junkFillProbability);
    Cell getCellByCoordinate(const Map& map, int x, int y);
    void setCellByCoordinate(Map& map, int x, int y, Cell cell);
    void getSurroundingCellsByCoordinate(const Map& map, int x, int y, bool includeDiagonals, Cell destination[], int destinationOffset, int& numSurroundingCells);
    void getStateCells(const Map& map, int x, int y, bool includeDiagonals, Cell destination[], int destinationOffset);
    int getNumCellsOfType(const Map& map, Cell cell);
    std::string toString(Map map);

    struct GameState {
        Map map;
        int playerX;
        int playerY;
        int score;
        const int maxScore;
        int numActionsPerformed;

        GameState(Map map, int playerX, int playerY);
    };

    GameState createGame(Map map, int startingX, int startingY);
    bool move(GameState& gameState, int xDir, int yDir);
    void pickup(GameState& gameState);
    std::string toString(const GameState& gameState);
};

#endif //GENETIC_JANITOR_GAME_H
