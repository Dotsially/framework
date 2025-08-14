#ifndef AI
#define AI


#include "common.cpp"
 
template<typename T>
struct BreadthFirstSearch {
    std::queue<T> frontier;
    std::unordered_set<T> reached;
    std::unordered_map<T, T> cameFrom;
};

//Perfoms bfs on a type T
template<typename T>
void AiBreathFirstSearch(BreadthFirstSearch<T>* bfs, T start, T goal, std::function<std::vector<T>(T)> graphNeighborFunction, uint32_t maxDepth){
    std::unordered_map<T, uint32_t> depthMap;
    
    bfs->frontier.push(start);
    bfs->reached.insert(start);
    bfs->cameFrom[start] = start;
    depthMap[start] = 0;

    uint32_t count = 0;
    while(bfs->frontier.size() > 0){
        T current = bfs->frontier.front();
        bfs->frontier.pop();

        uint32_t currentDepth = depthMap[current];

        if(current == goal) break;

        if (currentDepth >= maxDepth) continue;

        for (const T& next : graphNeighborFunction(current)){
            if(bfs->reached.find(next) == bfs->reached.end()){
                bfs->frontier.push(next);
                bfs->reached.insert(next);    
                bfs->cameFrom[next] = current;

                depthMap[next] = currentDepth +1;
            }     
        }
    }
};


template<typename T>
std::vector<T> AiBfsReconstructPath(BreadthFirstSearch<T>* bfs, T start, T goal){
    std::vector<T> path;
    T current = goal;

    // If goal not reached, cameFrom will not contain it
    if (bfs->cameFrom.find(goal) == bfs->cameFrom.end()) {
        return path; // empty path = not found
    }

    while (current != start) {
        path.push_back(current);
        current = bfs->cameFrom.at(current);
    }

    path.push_back(start);
    std::reverse(path.begin(), path.end()); // Reverse to get path from start -> goal
    return path;
}














#endif