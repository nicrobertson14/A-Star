// Written by Nicholas Robertson
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <sstream>

using namespace std;

class Node {
public:
    Node(){
        fScore = INT_MAX/2;
        gScore = 0;
    }
    Node(bool isFree, pair<int,int> pos){
        free = isFree;
        position = pos;
        fScore = INT_MAX/2;
        gScore = 0;
    }
    int fScore, gScore;
    bool free;
    pair<int,int> position, prevPosition;
};

class Grid {
public:
    Grid(int _width, int _height){
        width = _width;
        height = _height;
        vector<Node> row(vector<Node>(width,Node(true,make_pair(0,0))));
        nodeGrid = vector<vector<Node> > (height,row);
    }
    int width, height;
    vector<vector<Node> > nodeGrid;
};

// Returns a pointer to Grid object based on input file
Grid * makeGrid(char * filename){
    string line;
    ifstream file(filename);
    istringstream iss;
    
    // getting grid dimensions
    getline(file,line);
    int width, height;
    iss.str(line);
    iss >> width;
    iss >> height;
    
    Grid *myGrid = new Grid(width,height);
    
    // reads input file and creates grid nodes
    if(file){
        int i,j = 0;
        while(getline(file,line)){
            for(i = 0; i < line.length(); i++){
                if(line[i] == '1'){
                    myGrid->nodeGrid[j][i/2] = Node(false,make_pair(i/2,j));
                }
                else if(line[i] == '0') {
                    myGrid->nodeGrid[j][i/2] = Node(true,make_pair(i/2,j));
                }
            }
            j++;
        }
    }
    return myGrid;
}

// Returns the Manhattan distance between two Nodes
int getDistance(Node pos1, Node pos2){
    return abs(pos1.position.first - pos2.position.first) + abs(pos1.position.second - pos2.position.second);
}

// Checks if grid position is traversable and within grid boundaries
bool isFree(Grid *grid, Node currNode){
    if (currNode.position.second < grid->height && currNode.position.second >= 0 && currNode.position.first >= 0 && currNode.position.first < grid->width && grid->nodeGrid[currNode.position.second][currNode.position.first].free == true){
        return true;
    }
    return false;
}

// Returns neighbour coordinates in upward direction
Node getNeighbour(pair<int,int> pos, string dir){
    if(dir == "DOWN"){
        return Node(true,make_pair(pos.first, pos.second + 1));
    }
    else if (dir == "UP"){
        return Node(true,make_pair(pos.first, pos.second - 1));
    }
    else if(dir == "LEFT"){
        return Node(true,make_pair(pos.first - 1, pos.second));
    }
    else {
        return Node(true,make_pair(pos.first + 1, pos.second));
    }
}

// Returns a vector of traversable neighbour Nodes
vector<Node> getNeighbours(Grid *grid, pair<int,int> currPos){
    vector<Node> neighbours;
    // checking UP neighbour
    if(currPos.second + 1 < grid->height && isFree(grid, getNeighbour(currPos,"DOWN"))){
        neighbours.push_back(getNeighbour(currPos,"DOWN"));
    }
    // checking DOWN neighbour
    if(currPos.second > 0 && isFree(grid, getNeighbour(currPos,"UP"))){
        neighbours.push_back(getNeighbour(currPos,"UP"));
    }
    // checking LEFT neighbour
    if(currPos.first > 0 && isFree(grid, getNeighbour(currPos,"LEFT"))){
        neighbours.push_back(getNeighbour(currPos,"LEFT"));
    }
    // checking RIGHT neighbour
    if(currPos.first + 1 < grid->width && isFree(grid, getNeighbour(currPos,"RIGHT"))){
        neighbours.push_back(getNeighbour(currPos,"RIGHT"));
    }

    return neighbours;
}

// Prints a string of directions based on a vector of coordinates
void printPath(vector<pair<int,int> > path){
    string directions;
    for(int i = path.size() - 2; i >= 0; i--){
        if(path[i].second > path[i + 1].second){
            directions += "D ";
        }
        else if(path[i].second < path[i + 1].second){
            directions += "U ";
        }
        else if(path[i].first > path[i + 1].first){
            directions += "R ";
        }
        else {
            directions += "L ";
        }
    }
    cout << directions << endl;
}

// Compiles a vector of each 'previous position' coordinate pair from the goal to the start
void retracePath(map<pair<int,int>,Node> gridMap, pair<int,int> startPos, pair<int,int> finalPos){
    vector<pair<int,int> > path;
    path.push_back(finalPos);
    pair<int,int> prevPos = finalPos;
    while(prevPos != startPos){
        prevPos = gridMap[prevPos].prevPosition;
        path.push_back(prevPos);
    }
    printPath(path);
}

// Boolean functor to compare the fScore of two nodes
struct node_compare {
    bool operator()(Node one, Node two){
        return one.fScore > two.fScore;
    }
};

int main(int argc, char **argv){
    // Initializing 2D grid of nodes from input file
    Grid *theGrid = makeGrid(argv[1]);
    
    pair<int,int> start = make_pair(atoi(argv[2]),atoi(argv[3]));
    pair<int,int> goal = make_pair(atoi(argv[4]),atoi(argv[5]));
    Node currNode = Node(true,start);
    Node goalNode = Node(true,goal);
    
    // checking if start and goal positions are traversable
    if( !(isFree(theGrid,currNode)) && !(isFree(theGrid,goalNode)) ){
        cout << "Invalid start/end point" << endl;
        return 1;
    }
    
    // HashMaps for quick access to g(n), f(n), openSet and closedSet
    map<pair<int,int>,bool> closedSet;
    map<pair<int,int>,bool> openSetMap;
    map<pair<int,int>,Node> gridMap;
    
    // Open set is a priority queue sorted by ascending fScores (low fScore = high priority)
    priority_queue<Node,vector<Node>,node_compare> openSet;
    
    // Distance from start to start = 0
    currNode.gScore = 0;
    
    // Heuristic estimate of distance from start to goal (manhattan distance)
    currNode.fScore = getDistance(currNode,goalNode);
    
    // Initially the open set contains only the start node
    openSet.push(currNode);
    gridMap[start] = currNode;

    while(!openSet.empty()){
        currNode = openSet.top();                               // set highest priority node as current
        if (currNode.position == goal){                         // if goal is reached, retrace path and return
            retracePath(gridMap,start,goal);
            return 0;
        }
        // Once visited, current node is removed from open set and added to closed
        openSet.pop();
        openSetMap.erase(currNode.position);
        closedSet[currNode.position] = true;
        
        // Gets all traversable neighbours of the current node
        vector<Node> neighbours = getNeighbours(theGrid, currNode.position);
        
        for (int i = 0; i < neighbours.size(); i++){
            // Ignore nodes from the closed set
            if(closedSet.count(neighbours[i].position)){
                continue;
            }

            // add new nodes to the open set and set fScore, gScore & previous position coordinates
            if(!openSetMap.count(neighbours[i].position)){
                neighbours[i].gScore = gridMap[currNode.position].gScore + 1;   //distance between 2 neighbours = 1
                neighbours[i].fScore = neighbours[i].gScore + getDistance(neighbours[i],goalNode);
                neighbours[i].prevPosition = currNode.position;
                openSetMap[neighbours[i].position] = true;
                openSet.push(neighbours[i]);
                gridMap[neighbours[i].position] = neighbours[i];
            }
            
            // if shorter path to neighbour is found, update scores, previous position and reinsert in open set
            int tmpGscore = gridMap[currNode.position].gScore + 1;
            if(gridMap[neighbours[i].position].gScore > tmpGscore){
                neighbours[i].gScore = tmpGscore;
                neighbours[i].fScore = tmpGscore + getDistance(neighbours[i],goalNode);
                neighbours[i].prevPosition = currNode.position;
                gridMap[neighbours[i].position] = neighbours[i];
                openSet.push(neighbours[i]);
            }
        }
    }
    
    cout << "No valid path found" << endl;
    return 1;
}

