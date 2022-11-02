/* 
 * File:   main.cpp
 * Author: Arthur Choy
 * Created on October 22, 2022, 7:04 PM
 */

// VERSION 2
// Fixed some output and works with test case 1 and 2
// AS IT TURNS OUT, COMPARISONS TYPE CAST
// DID YOU KNOW THAT? I DIDN'T KNOW THAT
// I haven't tested the other test cases yet
// Added a whole mess of debug outputs, to clean up in version 3

// Libraries
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// State node
struct Node {
    short state[3][3];   // Puzzle grid, 0 represents the blank space
    // gn = depth = path cost
    // hn = hueristic distance to goal
    int gn, hn;
};

// Function prototypes
// MAIN FUNCTIONS
bool aStar(vector<Node>&, int &);
int heuristic(Node);
bool testState(const Node &, const Node &);
void expand(vector<Node>&, int);

// HELPER FUNCTIONS
pair<int, int> findNumPos(const Node &, int);
void nodeNumSwap(Node &, pair<int, int>, pair<int, int>);
bool cmpFunc(const Node &lhs, const Node &rhs) {
    return (lhs.gn + lhs.hn) < (rhs.gn + rhs.hn);
}
/*
 * 
 */
int main(int argc, char** argv) {
    Node initial;
    
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cin >> initial.state[x][y];
        }
    }
    initial.gn = 0;
    initial.hn = heuristic(initial);
    cout << "INITIAL STATE: " << endl;
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cout << initial.state[x][y] << " ";
        }
        cout << endl;
    }
    
    vector<Node> q;
    q.push_back(initial);
    int pos;
    
    if(aStar(q, pos)) {
        cout << "depth: " << q[pos].gn << endl;
        for(int y = 0; y < 3; y++) {
            for(int x = 0; x < 3; x++) {
                cout << q[pos].state[x][y];
            }
        }
    }
    else cout << "Failed to find solution" << endl;
    
    return 0;
}

// ================================================
// This function holds the generic search algorithm
// ================================================
bool aStar(vector<Node> &q, int &pos) {
    // Initialize goal state
    Node goal;
    // Required internet consultation: https://stackoverflow.com/questions/30178879/how-can-i-assign-an-array-from-an-initializer-list
    // (Not for code but for general C++ rules)
    int arr[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };
    cout << "INITIALIZING GOAL STATE: " << endl;
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cout << "arr at " << (y*3)+x << ": " << arr[(y*3)+x] << endl;
            goal.state[x][y] = arr[(y*3)+x];
            cout << "goal at " << x << " & " << y << ": " << goal.state[x][y] << endl;
        }
    }
    cout << "GOAL STATE: " << endl;
    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            cout << goal.state[x][y] << " ";
        }
        cout << endl;
    }
    goal.hn = goal.gn = 0;
    
    // Initialize position
    pos = -1;
    
    cout << "pos: " << pos << " | q.size()-1: " << (q.size()-1) << endl;
    
    // While loop
    cout << "pong" << endl;
    while(pos < static_cast<int>(q.size()-1)) {
        cout << "ping" << endl;
        // "Pop" the front-most node in the queue
        pos++;
        // Test if the new front-most node is the goal state
        if(testState(goal, q[pos]) && q[pos].hn == 0) return true;
        // Expand the current node
        expand(q, pos);
        // Sort the remaining nodes to prioritize checking the node with the
        // smallest g(n) + h(n) first
        sort(q.begin()+pos, q.end(), cmpFunc);
    }
    cout << "what" << endl;
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
// AT THE MOMENT, YOU SHOULD BE WORKING WITH VERSION 1
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int heuristic(Node curr) {
    return 0;
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
                cout << "DING DONG, RETURN FALSE: " << endl;
                cout << "node 1 at " << i << "/" << j << ": " << node1.state[i][j];
                cout << "node 2 at " << i << "/" << j << ": " << node2.state[i][j];
                return false;
            }
        }
    }
    cout << "bing bong, return true" << endl;
    return true;
}

// ===========================================================================
// This function expands a given state, making sure to not add repeated states
// ===========================================================================
void expand(vector<Node> &q, int pos) {
    // Array that holds the x/y transforms to find adjacent tile positions
    // to help with looping
    // WEIRD TECHNICALITY: Up/Down are reversed due to the nature of how I
    // made the 2D arrays, whoops
    pair<int, int> adjacentArr[4] = { pair<int, int>(0, -1),    // Above
                                      pair<int, int>(1, 0),     // Right
                                      pair<int, int>(0, 1),     // Below
                                      pair<int, int>(-1, 0) };  // Left
    
    // Get the position of the 0/"blank" in the base node
    pair<int, int> zeroPos = findNumPos(q[pos], 0);
    
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
            Node newNode = q[pos];
            newNode.gn++;                           // Iterate cost
            nodeNumSwap(newNode, zeroPos, adjPos);  // Perform tile shift
            newNode.hn = heuristic(newNode);        // Calculate heuristic
            
            // Search through "queue" to see if the new state was or is currently
            // in the queue, if so, set flag to true
            for(vector<Node>::iterator it = q.begin(); it != q.end(); it++) {
                if(testState((*it), newNode)) alreadyThere = true;
            }
            // If flag wasn't triggered, then this is a new state: add to queue
            if(!alreadyThere) q.push_back(newNode);
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