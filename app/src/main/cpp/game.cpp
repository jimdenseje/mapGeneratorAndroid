#include "game.h"

unsigned int seed = 1;
int minRoomSize = 25;
bool nextLevel = false;
int playerViewAngel = 0;

bool keyStates[5] = {};

vector<int> secondMap(CANVAS_WIDTH * CANVAS_HEIGHT, 0);
vector<int> mainMap(CANVAS_WIDTH * CANVAS_HEIGHT, 0);

bool mainMapLocked = false;

vector<int> colors(mainMap.size(), 0);
vector<int> backgroundMap(mainMap.size(), 0);

vector<int> levelSquareMap(CANVAS_WIDTH * CANVAS_HEIGHT, 0);
vector<int> levelMap(CANVAS_WIDTH * CANVAS_HEIGHT, 0);

void resetKeyStates();

void removeNotNullValuesFromMap(std::vector<int>& mapFrom, std::vector<int>& mapTo) {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            if (mapFrom[(y * CANVAS_WIDTH) + x] != 0) {
                mapTo[(y * CANVAS_WIDTH) + x] = 0;
            }
        }
    }
}

void replaceNotNullValuesFromMap(std::vector<int>& mapFrom, std::vector<int>& mapTo) {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            if (mapFrom[(y * CANVAS_WIDTH) + x] != 0) {
                mapTo[(y * CANVAS_WIDTH) + x] = mapFrom[(y * CANVAS_WIDTH) + x];
            }
        }
    }
}

double getDistance(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow((x1 - x2), 2) + std::pow((y1 - y2), 2));
}

unsigned int getRandomInt(unsigned int min, unsigned int max) {
    /*
     * plus one on min and max to be able to use 0 as a input and result
     * -1 on return
     */
    unsigned int randomNumber = (min+1) + (rand() % (max+1));
    return randomNumber - 1;
}

// Function to get the number of elements in the mainMap
int getNumberOfElement(int elementId) {
    int count = 0;
    for (int p : mainMap) {
        if (p == elementId) {
            count++;
        }
    }
    return count;
}

// Function to check if an element has a certain number of connections
bool hasNumberOfConnections(int elementId, int connections) {
    int count = 0;
    for (int p : mainMap) {
        if (p == elementId) {
            count++;
            if (count >= connections) {
                return true;
            }
        }
    }
    return false;
}

// Function to find the number of nearby squares
int findNumberOfNearBySquares(int upToDistance, int x, int y) {
    int numberOfNearBySquares = 0;

    if (mainMap[y * CANVAS_WIDTH + x] == 0 || upToDistance < 1) {
        return -1;
    }

    for (int distance = 1; distance <= upToDistance; distance++) {
        if ((y * CANVAS_WIDTH + x) - (CANVAS_WIDTH * distance) >= 0 && mainMap[(y * CANVAS_WIDTH + x) - (CANVAS_WIDTH * distance)] == mainMap[y * CANVAS_WIDTH + x]) {
            numberOfNearBySquares++;
        }
        if ((y * CANVAS_WIDTH + x) + (CANVAS_WIDTH * distance) < mainMap.size() && mainMap[(y * CANVAS_WIDTH + x) + (CANVAS_WIDTH * distance)] == mainMap[y * CANVAS_WIDTH + x]) {
            numberOfNearBySquares++;
        }
        if ((y * CANVAS_WIDTH + x) + distance < mainMap.size() && mainMap[(y * CANVAS_WIDTH + x) + distance] == mainMap[y * CANVAS_WIDTH + x] && ((y * CANVAS_WIDTH + x) + distance) % CANVAS_WIDTH != 0) {
            numberOfNearBySquares++;
        }
        if ((y * CANVAS_WIDTH + x) - distance >= 0 && mainMap[(y * CANVAS_WIDTH + x) - distance] == mainMap[y * CANVAS_WIDTH + x] && ((y * CANVAS_WIDTH + x) - distance + 1) % CANVAS_WIDTH != 0) {
            numberOfNearBySquares++;
        }
    }
    return numberOfNearBySquares;
}

// Function to remove lonely squares
void removeLonlySquares(int minConnections, int upToDistance) {
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT; y++) {

            if (findNumberOfNearBySquares(upToDistance, x, y) > minConnections) {
                continue;
            }

            switch (getRandomInt(0, 3)) {
            case 0:
                if ((y * CANVAS_WIDTH + x) - CANVAS_WIDTH >= 0 && mainMap[(y * CANVAS_WIDTH + x) - CANVAS_WIDTH] != mainMap[y * CANVAS_WIDTH + x]) {
                    mainMap[y * CANVAS_WIDTH + x] = mainMap[(y * CANVAS_WIDTH + x) - CANVAS_WIDTH];
                }
                break;
            case 1:
                if ((y * CANVAS_WIDTH + x) - 1 >= 0 && mainMap[(y * CANVAS_WIDTH + x) - 1] != mainMap[y * CANVAS_WIDTH + x] && (y * CANVAS_WIDTH + x) % CANVAS_WIDTH != 0) {
                    mainMap[y * CANVAS_WIDTH + x] = mainMap[(y * CANVAS_WIDTH + x) - 1];
                }
                break;
            case 2:
                if ((y * CANVAS_WIDTH + x) + 1 < mainMap.size() && mainMap[(y * CANVAS_WIDTH + x) + 1] != mainMap[y * CANVAS_WIDTH + x] && (y * CANVAS_WIDTH + x + 1) % CANVAS_WIDTH != 0) {
                    mainMap[y * CANVAS_WIDTH + x] = mainMap[(y * CANVAS_WIDTH + x) + 1];
                }
                break;
            case 3:
                if ((y * CANVAS_WIDTH + x) + CANVAS_WIDTH < mainMap.size() && mainMap[(y * CANVAS_WIDTH + x) + CANVAS_WIDTH] != mainMap[y * CANVAS_WIDTH + x]) {
                    mainMap[y * CANVAS_WIDTH + x] = mainMap[(y * CANVAS_WIDTH + x) + CANVAS_WIDTH];
                }
                break;
            }
        }
    }

    levelSquareMap = mainMap;
}

int findNumberOfNearByEnemySquaresToTheRightAndTop(int upToDistance, int x, int y) {
    int numberOfNearBySquares = 0;

    if (y * CANVAS_WIDTH + x >= mainMap.size() || mainMap[y * CANVAS_WIDTH + x] == 0 || upToDistance < 1) {
        //std::cout << "overflow" << std::endl;
        return -1;
    }

    for (int distance = 1; distance <= upToDistance; distance++) {
        // UP
        int upIndex = y * CANVAS_WIDTH + x - (CANVAS_WIDTH * distance);
        if (upIndex >= 0 && upIndex < mainMap.size() &&
            mainMap[upIndex] != 0 &&
            mainMap[upIndex] != mainMap[y * CANVAS_WIDTH + x]) {
            numberOfNearBySquares++;
        }

        // RIGHT
        int rightIndex = y * CANVAS_WIDTH + x + distance;
        if (rightIndex < mainMap.size() &&
            mainMap[rightIndex] != 0 &&
            mainMap[rightIndex] != mainMap[y * CANVAS_WIDTH + x] &&
            (y * CANVAS_WIDTH + x + distance) % CANVAS_WIDTH != 0) {
            numberOfNearBySquares++;
        }
    }

    return numberOfNearBySquares;
}

int findNumberOfNearByEnemySquares(int upToDistance, int x, int y) {
    int numberOfNearBySquares = 0;

    if (((y * CANVAS_WIDTH) + x) >= mainMap.size() || upToDistance < 1) {
        //std::cout << "overflow" << std::endl;
        return 0;
    }

    for (int distance = 1; distance <= upToDistance; distance++) {
        int index_up = ((y * CANVAS_WIDTH) + x - (CANVAS_WIDTH * distance));
        int index_down = ((y * CANVAS_WIDTH) + x + (CANVAS_WIDTH * distance));
        int index_right = ((y * CANVAS_WIDTH) + x + distance);
        int index_left = ((y * CANVAS_WIDTH) + x - distance);

        if (index_up >= 0 && index_up < mainMap.size() &&
            mainMap[index_up] != 0 &&
            mainMap[index_up] != mainMap[((y * CANVAS_WIDTH) + x)]) {
            numberOfNearBySquares++;
        }

        if (index_down >= 0 && index_down < mainMap.size() &&
            mainMap[index_down] != 0 &&
            mainMap[index_down] != mainMap[((y * CANVAS_WIDTH) + x)]) {
            numberOfNearBySquares++;
        }

        if (index_right >= 0 && index_right < mainMap.size() &&
            mainMap[index_right] != 0 &&
            mainMap[index_right] != mainMap[((y * CANVAS_WIDTH) + x)] &&
            (index_right % CANVAS_WIDTH) != 0) {
            numberOfNearBySquares++;
        }

        if (index_left >= 0 && index_left < mainMap.size() &&
            mainMap[index_left] != 0 &&
            mainMap[index_left] != mainMap[((y * CANVAS_WIDTH) + x)] &&
            ((index_left + 1) % CANVAS_WIDTH) != 0) {
            numberOfNearBySquares++;
        }
    }

    //std::cout << numberOfNearBySquares << std::endl;

    return numberOfNearBySquares;
}

std::vector<int> makeWalls_1(std::vector<int>& mainMap) {
    std::vector<int> wallBlock(mainMap.size(), 0);

    for (int x = 0; x < CANVAS_WIDTH; ++x) {
        for (int y = 0; y < CANVAS_HEIGHT; ++y) {
            if (findNumberOfNearBySquares(1, x, y) < 4) {
                wallBlock[y * CANVAS_WIDTH + x] = mainMap[y * CANVAS_WIDTH + x];
            }
        }
    }

    return wallBlock;
}

void makeWalls(vector<int>& mainMap) {
    std::vector<int> mainMap2 = mainMap;

    
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT; y++) {
            if (findNumberOfNearByEnemySquaresToTheRightAndTop(1, x, y) > 0) {
                mainMap2[y * CANVAS_WIDTH + x] = 0;
            }
        }
    }

    
    for (int x = 0; x < CANVAS_WIDTH - 1; x++) {
        for (int y = 1; y < CANVAS_HEIGHT; y++) {
            if (mainMap2[y * CANVAS_WIDTH + x - CANVAS_WIDTH + 1] != mainMap2[y * CANVAS_WIDTH + x] &&
                mainMap2[y * CANVAS_WIDTH + x - CANVAS_WIDTH + 1] != 0) {
                mainMap2[y * CANVAS_WIDTH + x] = 0;
            }
        }
    }

    
    vector<int> wallBlock(mainMap.size(), 0);

    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT; y++) {
            if (mainMap2[y * CANVAS_WIDTH + x] == 0) {
                wallBlock[y * CANVAS_WIDTH + x] = levelSquareMap[y * CANVAS_WIDTH + x];
            }
        }
    }

    
    // findMiddleOfRums
    std::vector<int> findAllWalls = makeWalls_1(mainMap);

    struct Room {
        unsigned int index;
        unsigned int x_sum;
        unsigned int x_min;
        unsigned int x_max;
        unsigned int y_sum;
        unsigned int y_min;
        unsigned int y_max;
        unsigned int points;
    };

    
    Room rooms[100]; // Max number of rooms we can store (adjust this based on your needs)
    int roomCount = 0; // Keep track of the number of rooms found

    
    // Loop through each coordinate of the canvas
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT; y++) {
            unsigned int index = mainMap[y * CANVAS_WIDTH + x];

            // Check if the room with this index already exists
            bool foundRoom = false;
            int roomArrIndex = 0;

            for (unsigned int i = 0; i < roomCount; i++) {
                if (rooms[i].index == index) {
                    foundRoom = true;
                    roomArrIndex = i;
                    break;
                }
            }

            // If room doesn't exist, create a new one
            if (!foundRoom) {
                roomArrIndex = roomCount;
                rooms[roomCount] = {
                    index,
                    0,                      // x_sum
                    INT_MAX,                // x_min
                    0,                      // x_max
                    0,                      // y_sum
                    INT_MAX,                // y_min
                    0,                      // y_max
                    0                       // points
                };
                roomCount++;
            }

            if (rooms[roomArrIndex].x_max < x) {
                rooms[roomArrIndex].x_max = x;
            }
            
            if (rooms[roomArrIndex].x_min > x) {
                rooms[roomArrIndex].x_min = x;
            }
            
            rooms[roomArrIndex].x_sum += x;

            if (rooms[roomArrIndex].y_max < y) {
                rooms[roomArrIndex].y_max = y;
            }
            
            if (rooms[roomArrIndex].y_min > y) {
                rooms[roomArrIndex].y_min = y;
            }

            rooms[roomArrIndex].y_sum += y;
            rooms[roomArrIndex].points++;
        }
    }

    for (int i = 0; i < roomCount; i++) {
        printDebug("Room ");
        printDebug(i);
        printDebug(" - Index: ");
        printDebug(rooms[i].index);
        printDebug(" x_min: ");
        printDebug(rooms[i].x_min);
        printDebug(" x_max: ");
        printDebug(rooms[i].x_max);
        printDebug(" y_min: ");
        printDebug(rooms[i].y_min);
        printDebug(" y_max: ");
            }

    
    mainMap = wallBlock; // default mainMap has been changed here
    vector<int> points(mainMap.size(), 0);


    

    for (int i = 0; i < roomCount; i++) {

        
        int x = std::ceil(rooms[i].x_sum / rooms[i].points);
        int y = std::ceil(rooms[i].y_sum / rooms[i].points);

        
        //std::cout << "Cal1 | " << rooms[i].at("index") << " | x: " << x << " y: " << y
          //  << " pixelNum: " << (y * CANVAS_WIDTH + x) << std::endl;

        if (findNumberOfNearByEnemySquares(1, x, y) == 0 && mainMap[y * CANVAS_WIDTH + x] == 0) {
            points[y * CANVAS_WIDTH + x] = 1;
            
                    }
        else {
            x = rooms[i].x_min + std::round((rooms[i].x_max - rooms[i].x_min) / 2.0F);
            y = rooms[i].y_min + std::round((rooms[i].y_max - rooms[i].y_min) / 2.0F);

                        //std::cout << "Cal2 | " << rooms[i].at("index") << " | x: " << x << " y: " << y
              //  << " pixelNum: " << (y * CANVAS_WIDTH + x) << std::endl;

            if (findNumberOfNearByEnemySquares(1, x, y) == 0 && mainMap[y * CANVAS_WIDTH + x] == 0) {
                points[y * CANVAS_WIDTH + x] = 2;
            }
            else if (x - 1 >= 0 && findNumberOfNearByEnemySquares(1, x - 1, y) == 0 && mainMap[y * CANVAS_WIDTH + x - 1] == 0) {
                points[y * CANVAS_WIDTH + x - 1] = 3;
            }
            else {
                points[y * CANVAS_WIDTH + x] = 4;
            }
                    }
        
    }

    
    // get closest
    struct Point {
        int fromX, fromY;
        int x, y;
        double distance;
    };

    Point point;

    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT; y++) {
            if (points[y * CANVAS_WIDTH + x] != 0) {
                point.fromX = x;
                point.fromY = y;
                goto found_point;
            }
            
        }
    }

found_point:

    
    vector<int> path(mainMap.size(), 0);

    while (true) {
        point.x = -1;
        point.y = -1;
        point.distance = std::numeric_limits<double>::max();

        for (int x2 = 0; x2 < CANVAS_WIDTH; x2++) {
            for (int y2 = 0; y2 < CANVAS_HEIGHT; y2++) {
                if (x2 == point.fromX && y2 == point.fromY) {
                    continue;
                }

                if (points[y2 * CANVAS_WIDTH + x2] != 0) {
                    double distance = getDistance(point.fromX, point.fromY, x2, y2);
                    if (distance < point.distance) {
                        point.x = x2;
                        point.y = y2;
                        point.distance = distance;
                    }
                }
                
            }
        }

        points[point.fromY * CANVAS_WIDTH + point.fromX] = 0;
        path[point.fromY * CANVAS_WIDTH + point.fromX] = 2;

        if (point.x == -1) {
            break;
        }

        if (std::abs(point.fromX - point.x) < std::abs(point.fromY - point.y)) {
            for (int x = std::min(point.fromX, point.x); x <= std::max(point.fromX, point.x); x++) {
                if (path[point.fromY * CANVAS_WIDTH + x] == 0) {
                    path[point.fromY * CANVAS_WIDTH + x] = 1;
                }
            }

            for (int y = std::min(point.fromY, point.y); y <= std::max(point.fromY, point.y); y++) {
                if (path[y * CANVAS_WIDTH + point.x] == 0) {
                    path[y * CANVAS_WIDTH + point.x] = 1;
                }
            }
        }
        else {
            for (int x = std::min(point.fromX, point.x); x <= std::max(point.fromX, point.x); x++) {
                if (path[point.y * CANVAS_WIDTH + x] == 0) {
                    path[point.y * CANVAS_WIDTH + x] = 1;
                }
            }

            for (int y = std::min(point.fromY, point.y); y <= std::max(point.fromY, point.y); y++) {
                if (path[y * CANVAS_WIDTH + point.fromX] == 0) {
                    path[y * CANVAS_WIDTH + point.fromX] = 1;
                }
            }
        }

        point.fromY = point.y;
        point.fromX = point.x;
        
    }

    removeNotNullValuesFromMap(path, mainMap);
}

// Function to merge squares
void mergeSquares(int minConnections) {
    int oldTrys = -1;
    int trys = 0;
    while (oldTrys != trys) {

        if (trys % 10 == 0 && trys != 0) {
            LOGV("jim", "%s", ("A1 TRYS: " + to_string(trys)).c_str());
        }

        oldTrys = trys;
        for (int p = 0; p < CANVAS_WIDTH * CANVAS_HEIGHT; p++) {
            if (hasNumberOfConnections(mainMap[p], minConnections)) {
                continue;
            }

            switch (getRandomInt(0, 3)) {
            case 0:
                if (p - CANVAS_WIDTH >= 0 && mainMap[p - CANVAS_WIDTH] != mainMap[p]) {
                    mainMap[p] = mainMap[p - CANVAS_WIDTH];
                }
                break;
            case 1:
                if (p - 1 >= 0 && mainMap[p - 1] != mainMap[p] && p % CANVAS_WIDTH != 0) {
                    mainMap[p] = mainMap[p - 1];
                }
                break;
            case 2:
                if (p + 1 < mainMap.size() && mainMap[p + 1] != mainMap[p] && (p + 1) % CANVAS_WIDTH != 0) {
                    mainMap[p] = mainMap[p + 1];
                }
                break;
            case 3:
                if (p + CANVAS_WIDTH < mainMap.size() && mainMap[p + CANVAS_WIDTH] != mainMap[p]) {
                    mainMap[p] = mainMap[p + CANVAS_WIDTH];
                }
                break;
            }
            trys++;
            
        }
        
        
    }
    levelSquareMap = mainMap;
}

// Function to clean up and add borders
void cleanUpAndAddBorders(vector<int>& mainMap) {
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            if (mainMap[y * CANVAS_WIDTH + x] > 0) {
                mainMap[y * CANVAS_WIDTH + x] = 1;
            }
            else {
                mainMap[y * CANVAS_WIDTH + x] = 0;
            }
            
            
        }
    }

    for (int x = 0; x < CANVAS_WIDTH; x++) {
        mainMap[0 * CANVAS_WIDTH + x] = 1;
        mainMap[(CANVAS_HEIGHT - 1) * CANVAS_WIDTH + x] = 1;
    }

    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        mainMap[y * CANVAS_WIDTH + 0] = 1;
        mainMap[y * CANVAS_WIDTH + (CANVAS_WIDTH - 1)] = 1;
    }
}

int timeout(unsigned long timeout)
{
    usleep(timeout);
    cout << "Application Timed Out" << endl;
    std::terminate();
}

void generateMapLevel() {

    // The parameters to the function are put after the comma
    //std::thread timeoutThread(timeout, 1000 * 1000);

    clock_t start = clock();

    string oldnew = "";

    vector<int> cleanblock(CANVAS_WIDTH * CANVAS_HEIGHT, 0);

    int plottedElements = 0;

    
    for (int y = 0; y < CANVAS_HEIGHT; y++) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            mainMap[y * CANVAS_WIDTH + x] = plottedElements++;
        }
    }

    // Main loop for map generation
    int count = 1;
    int cc = 0;
    bool MapGenerated = false;

    while (!MapGenerated) {

        if (count < (CANVAS_WIDTH * CANVAS_HEIGHT) / minRoomSize) {
            LOGV("jim", "%s", "A");

            mergeSquares(count++);
        }
        else if (cc < 10) {

            LOGV("jim", "%s", "B");

            removeLonlySquares(1, 1);

            LOGV("jim", "%s", "C");


            cc++;
            if (cc == 10) {
                makeWalls(mainMap);
                                cleanUpAndAddBorders(mainMap);
                                levelMap = mainMap;

                clock_t end = clock();

                LOGV("jim", "%s", ("Execution time: " + to_string(((float)(end - start) / (float)CLOCKS_PER_SEC) * (float)1000)).c_str());

                MapGenerated = true;
                            }
        }

        
    }

    //usleep(50 * 1000);

    //TerminateThread(timeoutThread.native_handle(), 1);
    //timeoutThread.detach(); // After TerminateThread

}

void nextLevelTimeout(unsigned long timeout)
{
    while (true) {
        usleep(timeout);
        nextLevel = true;
    }
}

double degreeToRadiant(double degree) {
    return M_PI * (double)degree / (double)180;
}

void movePlayerFastAuto(int x, int y) {
    static vector<int> leftRooms = {};

    int count = 0;

    while (!nextLevel) {
        if (mainMapLocked) {
            usleep(1 * 1000);
            continue;
        }

        mainMapLocked = true;

        mainMap[y * CANVAS_WIDTH + x] = 0;

        int oldRoom = levelSquareMap[y * CANVAS_WIDTH + x];

        if (getRandomInt(1, 2) == 1) {
            x++;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
                || std::find(leftRooms.begin(), leftRooms.end(), levelSquareMap[y * CANVAS_WIDTH + x]) != leftRooms.end()
            ) {
                x--;
            }
        }
        else {
            x--;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
                || std::find(leftRooms.begin(), leftRooms.end(), levelSquareMap[y * CANVAS_WIDTH + x]) != leftRooms.end()
            ) {
                x++;
            }
        }

        if (getRandomInt(1, 2) == 1) {
            y++;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
                || std::find(leftRooms.begin(), leftRooms.end(), levelSquareMap[y * CANVAS_WIDTH + x]) != leftRooms.end()
            ) {
                y--;
            }
        }
        else {
            y--;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
                || std::find(leftRooms.begin(), leftRooms.end(), levelSquareMap[y * CANVAS_WIDTH + x]) != leftRooms.end()
            ) {
                y++;
            }
        }

        count++;
        if (oldRoom != levelSquareMap[y * CANVAS_WIDTH + x]) {
            leftRooms.push_back(oldRoom);
            count = 0;
        }

        if (count >= 1000) {
            nextLevel = true;
        }

        if (keyStates[4]) {
            nextLevel = true;
        }
        resetKeyStates();

        mainMap[y * CANVAS_WIDTH + x] = 12;
        mainMapLocked = false;

        usleep(2 * 1000);
    }
    nextLevel = false;
    leftRooms.clear();
}

void movePlayerUserInput(int x, int y) {
    while (!nextLevel) {
        if (mainMapLocked) {
            usleep(1 * 1000);
            continue;
        }

        mainMapLocked = true;

        secondMap = levelSquareMap;
        replaceNotNullValuesFromMap(levelMap, secondMap);

        secondMap[y * CANVAS_WIDTH + x] = 53; //player
        mainMap = backgroundMap;

        const int res = 1;
        int raycastX = CANVAS_WIDTH; // ((double)res / (double)2);

        double povI = 0.0 - ((double)POV / 2.0) + 1.0;
        for (;;) {
            if (povI >= ((double)POV / 2.0)) {
                break;
            }
            //LOGV("jim", "%s", to_string(povI).c_str());

            raycastX -= res;

            for (int distance = 1; true; distance++) {
                int pointX = x + round(cos(degreeToRadiant(playerViewAngel + 90 + povI)) * distance);
                int pointY = y + round(sin(degreeToRadiant(playerViewAngel + 90 + povI)) * distance);

                if (levelMap[pointY * CANVAS_WIDTH + pointX] > 0) {
                    int LastPointX = x + round(cos(degreeToRadiant(playerViewAngel + 90 + povI)) * (distance - 1));
                    int LastpointY = y + round(sin(degreeToRadiant(playerViewAngel + 90 + povI)) * (distance - 1));

                    int fullcolor = 1;

                    if (fullcolor < 1 && 35 - (distance) < 2) {
                    }
                    else {

                        for (int raycastY = distance; raycastY <= CANVAS_HEIGHT - distance; raycastY++) {
                            if (raycastX == CANVAS_WIDTH) {
                                break;
                            }

                            if (fullcolor < 1 && pointX - 1 >= 0 && levelMap[pointY * CANVAS_WIDTH + pointX - 1] > 0) {
                                mainMap[raycastY * CANVAS_WIDTH + (raycastX)] = 35 - (distance);
                            }
                            else if (fullcolor < 2) {

                                //mainMap[raycastY * canvasWidth + (raycastX)] = 35 - (distance);
                                //int distanceWallCorrection = ceil((double)distance * (double)cos(degreeToRadiant(playerViewAngel - povI - 90)));
                                if (levelMap[pointY * CANVAS_WIDTH + pointX] == 1) {
                                    mainMap[raycastY * CANVAS_WIDTH + (raycastX)] = levelSquareMap[LastpointY * CANVAS_WIDTH + LastPointX];
                                }

                                if (levelMap[pointY * CANVAS_WIDTH + pointX] == 2) {
                                    if (raycastY % 2 == 0) { //&& raycastX % 2 == 0
                                        mainMap[raycastY * CANVAS_WIDTH + (raycastX)] = 2;
                                    }
                                    else {
                                        mainMap[raycastY * CANVAS_WIDTH + (raycastX)] = levelSquareMap[LastpointY * CANVAS_WIDTH + LastPointX];

                                    }
                                }

                            }
                            else {
                                mainMap[raycastY * CANVAS_WIDTH + (raycastX)] = 35 - (distance);
                            }

                        }

                    }
                    break;
                }
                else {
                    
                    /*
                    * Raygun
                    */
                    if (31 - (distance * 1) < 2) {
                        secondMap[pointY * CANVAS_WIDTH + pointX] = 0;
                    }
                    else {
                        secondMap[pointY * CANVAS_WIDTH + pointX] = 31 - (distance * 1);
                    }
                    
                }

            }

            //povI += 1.875;
            povI += ((double)POV / ((double)CANVAS_WIDTH));
        }

        mainMapLocked = false;

        //keyStates[getRandomInt(0, 3)] = true;

        if (keyStates[0]) {
            int newX = x + (int)round(cos(degreeToRadiant(playerViewAngel + 90)) * 1);
            int newY = y + (int)round(sin(degreeToRadiant(playerViewAngel + 90)) * 1);

            if (levelMap[newY * CANVAS_WIDTH + newX] == 0) {
                x = newX;
                y = newY;
            }
        }

        if (keyStates[1]) {
            int newX = x - (int)round(cos(degreeToRadiant(playerViewAngel + 90)) * 1);
            int newY = y - (int)round(sin(degreeToRadiant(playerViewAngel + 90)) * 1);

            if (levelMap[newY * CANVAS_WIDTH + newX] == 0) {
                x = newX;
                y = newY;
            }
        }

        if (keyStates[2]) {
            playerViewAngel += 10;
            if (playerViewAngel >= 360) {
                playerViewAngel -= 360;
            }
        }

        if (keyStates[3]) {
            playerViewAngel -= 10;
            if (playerViewAngel < 0) {
                playerViewAngel += 360;
            }
        }

        if (keyStates[4]) {
            nextLevel = true;
        }

        resetKeyStates();

        usleep(2 * 1000);
    }
    nextLevel = false;
}

void movePlayer(int x, int y) {
    while (!nextLevel) {
        if (mainMapLocked) {
            usleep(1 * 1000);
            continue;
        }
        mainMapLocked = true;

        mainMap[y * CANVAS_WIDTH + x] = 0;

        if (keyStates[1]) {
            y++;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
            ) {
                y--;
            }
        }

        if (keyStates[0]) {
            y--;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
            ) {
                y++;
            }
        }

        if (keyStates[2]) {
            x--;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
            ) {
                x++;
            }
        }

        if (keyStates[3]) {
            x++;
            if (
                mainMap[y * CANVAS_WIDTH + x] != 0
            ) {
                x--;
            }
        }

        if (keyStates[4]) {
            nextLevel = true;
        }

        resetKeyStates();

        mainMap[y * CANVAS_WIDTH + x] = 12;
        mainMapLocked = false;

        usleep(2 * 1000);

    }
    nextLevel = false;
}

void resetKeyStates() {
    for (u_char i = 0; i < 5; i++) {
        keyStates[i] = false;
    }
}

void setupGame() {
    resetKeyStates();
}

void initGame() {

    /* Init Color generator ONCE */
    for (int p = 0; p < CANVAS_HEIGHT * CANVAS_WIDTH; p++) {
        int rgb = 0;
        if ((p == 0 && DISPLAY_TYPE == 1) || (p == 1 && DISPLAY_TYPE == 2)) {
            int r = 0;
            int g = 0;
            int b = 0;
            rgb = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);

        }
        else if ((p == 1 && DISPLAY_TYPE == 1) || (p == 0 && DISPLAY_TYPE == 2)) {
            int r = 255;
            int g = 255;
            int b = 255;
            rgb = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);

        }
        else if (p < 25) {
            int r = p * 10;
            int g = p * 4;
            int b = p * 4;
            rgb = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);
        }
        else if (p < 35) {
            int r = 255;
            int g = 100 - ((p - 25) * 8);
            int b = 100 - ((p - 25) * 8);
            rgb = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);
        }
        else {
            int r = (int)getRandomInt(0, 128);
            int g = (int)getRandomInt(0, 128);
            int b = (int)getRandomInt(0, 128);
            rgb = ((r & 0x0ff) << 16) | ((g & 0x0ff) << 8) | (b & 0x0ff);

        }
        colors[p] = rgb;
    }

#ifdef __ANDROID__
    LOGV("jim", "%s", "You are running on Android");
#elif defined(__linux__)
    cout << "You are running Linux" << endl;
#elif defined(_WIN64)
    cout << "You are running Microsoft Windows (64-bit)" << endl;
#elif defined(_WIN32)
    cout << "You are running Microsoft Windows (32-bit)" << endl;
#elif defined(ESP_PLATFORM)
    cout << "You are running ESP32" << endl;
#else
    cout << "You are running on a unknown OS" << endl;
#endif

    //std::thread nextLevelThread(nextLevelTimeout, 14000 * 1000);

    //backgroundMap
    for (int x = 0; x < CANVAS_WIDTH; x++) {
        for (int y = 0; y < CANVAS_HEIGHT / 2; y++) {
            backgroundMap[(y * CANVAS_WIDTH) + x] = 100;
        }
        for (int y = CANVAS_HEIGHT / 2; y < CANVAS_HEIGHT; y++) {
            backgroundMap[(y * CANVAS_WIDTH) + x] = 101;
        }
    }

    
    for (int x = 0; ; x++) {
        //seed = time(NULL);
        //seed = 10; //rand();

        seed = rand();

        //seed = 1;
        //seed = 1731412631734;
        //cout << seed << endl;
        srand(seed);

        
        generateMapLevel();

        /*
        vector<int> buffer = levelSquareMap;
        replaceNotNullValuesFromMap(mainMap, buffer);
        mainMap = buffer;

        buffer.clear();
        //delete(&buffer); //breaks stuff
        */

        if (x % 3 == 0) {
            movePlayerUserInput(1, 1);
        } else if (x % 3 == 1) {
            movePlayerFastAuto(1, 1);
        } else {
            movePlayer(1, 1);
        }

        //usleep(500 * 1000);
        //return 0;
    }
}