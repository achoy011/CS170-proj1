/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on October 22, 2022, 7:04 PM
 */

// VERSION 6
// Added the Misplaced Tile and Manhattan Distance heuristics
// Added more output to demonstrate the algorithm is working properly
// (aka. spitting out the top of the queue every time a node is expanded)
// Also updated the expand() comments 'cause, as it turns out
// findNumPos() spits out the number coordinates in [y][x] format. Fun.

// Libraries
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ctime>
using namespace std;

// Global Variables
int maxQSize = 0;

// Structures
// State node
struct Node {
    short state[3][3];   // Puzzle grid, 0 represents the blank space
    // gn = depth = path cost
    // hn = hueristic distance to goal
    unsigned long long int gn; 
    unsigned short hn;
};
// Custom comparison class to sort by g(n) + h(n) in priority queue
class cmpClass {
    public:
    bool operator()(const Node &lhs, const Node &rhs) {
        return (lhs.gn + lhs.hn) > (rhs.gn + rhs.hn);
    }
};

// Function prototypes
// MAIN FUNCTIONS
bool aStar(priority_queue<Node, vector<Node>, cmpClass>&, vector<Node>&, const short);
int heuristic(Node, const short);
bool testState(const Node &, const Node &);
void expand(priority_queue<Node, vector<Node>, cmpClass>&, vector<Node>&, const short);

// HELPER FUNCTIONS
pair<int, int> findNumPos(const Node &, int);
void nodeNumSwap(Node &, pair<int, int>, pair<int, int>);
void displayNode(const Node);
/*
 * 
 */
int main(int argc, char** argv) {
    // The initial state
    Node initial;
    // Algorithm variable
    short algorithm;
    
    // Get algorithm choice from user
    cout << "Please enter a number for the algorithm you would like to use: " << endl;
    cout << "\'1\' - Uniform Cost Search" << endl;
    cout << "\'2\' - Misplaced Tile Heuristic" << endl;
    cout << "\'3\' - Manhattan Distance Heuristic" << endl;
    cin >> algorithm;
    cout << endl;
    
    // Get input and initialize heuristics
    cout << "Please enter the starting state of the puzzle from the top left number to the bottom ";
    cout << "right number, ie. \"1 2 3 4 5 6 7 8 0\"" << endl;
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cin >> initial.state[x][y];
        }
    }
    cout << endl;
    initial.gn = 0;
    initial.hn = heuristic(initial, algorithm);
    
    // Output initial state as confirmation
    cout << "INITIAL STATE: " << endl;
    displayNode(initial);
    
    // Initialize queue and queue history
    vector<Node> history;
    priority_queue<Node, vector<Node>, cmpClass> q;
    q.push(initial);
    
    int start = time(0);
    // If algorithm succeeded
    if(aStar(q, history, algorithm)) {
        cout << endl << "Puzzle solved!" << endl;
        cout << "This should be the solved puzzle: " << endl;
        displayNode(q.top());
    }
    // If algorithm failed
    else cout << endl << "Failed to find solution" << endl;
    int stop = time(0);
    
    // Output nodes expanded and depth for statistics
    cout << "Solution depth: " << q.top().gn << endl;
    cout << "Nodes expanded: " << history.size()-1 << endl;
    cout << "Maximum Node Queue Size: " << maxQSize << endl;
    cout << "Time taken: " << stop - start << " seconds" << endl;
    
    return 0;
}

// ================================================
// This function holds the generic search algorithm
// ================================================
bool aStar(priority_queue<Node, vector<Node>, cmpClass> &q, vector<Node> &history, const short algorithm) {
    // Initialize goal state
    Node goal;
    // Required internet consultation: https://stackoverflow.com/questions/30178879/how-can-i-assign-an-array-from-an-initializer-list
    // (Not for code but for general C++ rules)
    int arr[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            goal.state[x][y] = arr[(y*3)+x];
        }
    }
    // Output the goal state in case something goes horribly wrong
    cout << "GOAL STATE: " << endl;
    displayNode(goal);
    cout << endl;
    goal.hn = goal.gn = 0;
    
    // While loop
    while(!q.empty()) {
        if(q.size() > maxQSize) maxQSize = q.size();
        // Test if the new front-most node is the goal state
        if(testState(goal, q.top()) && q.top().hn == 0) return true;
        // Expand the current node and pop
        cout << "Expanding node with g(n) = " << q.top().gn << " and h(n) = " << q.top().hn << ": " << endl;
        // Demonstrative output
        displayNode(q.top());
        expand(q, history, algorithm);
    }
    return false;
}

// =======================================================================
// THE ONLY FUNCTION THAT SHOULD CHANGE BETWEEN ALL 3 VERSIONS OF THE CODE
// This code calculates the h(n) of a particular node
// Version 1: Uniform Cost Search - always returns 0
// Version 2: Misplaced Tile heuristic - returns the amount of tiles not
//           in the correct place
// Version 3: Manhattan Distance heuristic - returns the sum of the 
//            distance each displaced node is from their intended position
// =======================================================================
int heuristic(Node curr, short ver) {
    // Array of the location all numbers' goal coordinates
    pair<int, int> arr[8] = { pair<int, int>(0, 0),
                              pair<int, int>(1, 0),
                              pair<int, int>(2, 0),
                              pair<int, int>(0, 1),
                              pair<int, int>(1, 1),
                              pair<int, int>(2, 1),
                              pair<int, int>(0, 2),
                              pair<int, int>(1, 2) };
    int hn = 0; // h(n) counter
    
    // Uniform Cost Search
    if(ver == 1) return hn;
    
    // Misplaced Tile Heuristic
    else if(ver == 2) {
        // If number is not in correct coordinates, increment h(n)
        for(int i = 0; i < 8; i++) {
            if(findNumPos(curr, i+1) != arr[i]) hn++;
        }
        return hn;
    }
    
    // Manhattan Distance Heuristic
    else if(ver == 3) {
        // If number is not in correct coordinates, distance is abs(current x - goal x) +
        // abs(current y - goal y) aka. x distance + y distance
        for(int i = 0; i < 8; i++) {
            pair<int, int> temp = findNumPos(curr, i+1);
            hn += abs(temp.first - arr[i].first) + abs(temp.second - arr[i].second);
        }
        return hn;
    }
    
    // In case user inputted a number not between 1-3 
    cout << "Not a valid algorithm" << endl;
    return hn;
}

// =============================================================================
// This function checks to see if a certain state is equivalent to another state
// This can be used to check the goal state or repeated states
// =============================================================================
bool testState(const Node &node1, const Node &node2) {
    // Iterate through every position in both node states
    // If there is a single discrepancy, they are not equal
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(node1.state[i][j] != node2.state[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// ===========================================================================
// This function expands a given state, making sure to not add repeated states
// ===========================================================================
void expand(priority_queue<Node, vector<Node>, cmpClass> &q, vector<Node> &history, const short algorithm) {
    // Array that holds the x/y transforms to find adjacent tile positions
    // to help with looping
    // WEIRD TECHNICALITY: Up/Down are reversed due to the nature of how I
    // made the 2D arrays, whoops
    pair<int, int> adjacentArr[4] = { pair<int, int>(0, -1),    // Left
                                      pair<int, int>(1, 0),     // Below
                                      pair<int, int>(0, 1),     // Right
                                      pair<int, int>(-1, 0) };  // Up
    
    // Get the position of the "blank" in the base node
    pair<int, int> zeroPos = findNumPos(q.top(), 0);
    Node temp = q.top();
    history.push_back(q.top());
    q.pop();
    
    // For loop for each tile around the blank 
    for(int i = 0; i < 4; i++) {
        // Flag to see if expanded node was already opened
        bool alreadyThere = false;
        
        // Get position of adjacent tile
        pair<int, int> adjPos = zeroPos;
        adjPos.first += adjacentArr[i].first;
        adjPos.second += adjacentArr[i].second;
        
        // If adjacent tile position is within boundaries, aka. it can be shifted
        // to the blank spot, then continue
        if((adjPos.first >= 0 && adjPos.first < 3) && (adjPos.second >= 0 && adjPos.second < 3)) {
            // Create a new node in which a tile has been shifted into the blank spot
            Node newNode = temp;
            newNode.gn = temp.gn+1;                 // Iterate cost (depth)
            nodeNumSwap(newNode, zeroPos, adjPos);  // Perform tile shift
            newNode.hn = heuristic(newNode, algorithm);        // Calculate heuristic
            
            // Search through previous nodes to see if the new state was
            // visited before, if so, set flag to true
            for(int j = 0; j < history.size(); j++) {
                if(testState(history[j], newNode)) alreadyThere = true;
            }
            // If flag wasn't triggered, then this is a new state: add to queue
            if(!alreadyThere) q.push(newNode);
            alreadyThere = false;   // Reset flag
        }
    }
}

// ================
// HELPER FUNCTIONS
// ================

// =====================================================================
// Find and return the position of the inputted number in the given node
// =====================================================================
pair<int, int> findNumPos(const Node &node, int num) {
    // i = xPos, j = yPos, searches column by column for number
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(node.state[i][j] == num) return pair<int, int>(i, j);
        }
    }
    // This should only occur if the inputted number was not between 0-9,
    // which means something has gone horrifically wrong
    return pair<int, int>(-1, -1);
}

// ============================================================================
// Shift a tile in the puzzle, in theory a number should only be swapped with 0
// ============================================================================
void nodeNumSwap(Node &node, pair<int, int> pos1, pair<int, int> pos2) {
    // Very basic swap algorithm, I hope I don't have to explain this
    char temp = node.state[pos1.first][pos1.second];
    node.state[pos1.first][pos1.second] = node.state[pos2.first][pos2.second];
    node.state[pos2.first][pos2.second] = temp;
    return;
}

// ===============================
// Output the node's current state
// ===============================
void displayNode(const Node node) {
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cout << node.state[x][y] << " ";
        }
        cout << endl;
    }
    return;
}