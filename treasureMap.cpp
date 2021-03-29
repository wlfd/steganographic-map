#include "treasureMap.h"
#include "queue.h"
using namespace std;

treasureMap::treasureMap(const PNG & baseim, const PNG & mazeim, pair<int,int> s){
    start = s;
    base = baseim;
    maze = mazeim;

}

void treasureMap::setGrey(PNG & im, pair<int,int> loc){
    RGBAPixel *p = im.getPixel(loc.first, loc.second);
    p->r = 2 * (p->r / 4);
    p->g = 2 * (p->g / 4);
    p->b = 2 * (p->b / 4);

}

void treasureMap::setLOB(PNG & im, pair<int,int> loc, int d){
    RGBAPixel *p = im.getPixel(loc.first, loc.second);

    // bit mask and shift
    int dRed = (d & 0b110000) >> 4; 
    int dGreen = (d & 0b001100) >> 2;
    int dBlue = d & 0b000011;

    p->r = (p->r & 0b11111100) + dRed;
    p->g = (p->g & 0b11111100) + dGreen;
    p-> b = (p->b & 0b11111100) + dBlue;

    // d = d1 d2 d3 d4 d5 d6;
    // p->r = r1 r2 r3 r4 d1 d2;
    // p->g = g1 g2 g3 g4 d3 d4; 
    // p->b = b1 b2 b3 b4 d5 d6;

}

PNG treasureMap::renderMap(){
    PNG ret = PNG(base);
    vector<vector<bool>> visited(ret.width(), vector<bool>(ret.height(), false));
    vector<vector<int>> distanceToStart(ret.width(), vector<int>(ret.height(), 0));
    Queue<pair<int,int>> exploreNext;

    if (start.first >= 0 && start.first < (int)ret.width() && 
        start.second >= 0 && start.second < (int)ret.height()) {
         visited[start.first][start.second] = true;
    }
    setLOB(ret, start, 0);
    exploreNext.enqueue(start);

    while (exploreNext.isEmpty() == false) {
        pair<int,int> curr = exploreNext.dequeue();
        vector<pair<int,int>> p = neighbors(curr);

        // for (int i = 0; i < 4; i++) {
        //     pair<int,int> p = neighbors(curr)[i];

        //     if(good(visited, curr, p)) {
        //         visited[p.first][p.second] = true;
        //         distanceToStart[p.first][p.second] = distanceToStart[curr.first][curr.second] + 1;
        //         setLOB(ret, p, distanceToStart[p.first][p.second]);
        //         exploreNext.enqueue(p);
        //     }
        // }

        for (int i = 0; i < 4; i++) {

            if(good(visited, curr, p[i])) {
                visited[p[i].first][p[i].second] = true;
                distanceToStart[p[i].first][p[i].second] = distanceToStart[curr.first][curr.second] + 1;
                setLOB(ret, p[i], distanceToStart[p[i].first][p[i].second]);
                exploreNext.enqueue(p[i]);
            }
        }
    }

    return ret;
}


PNG treasureMap::renderMaze(){
    PNG ret = PNG(base);
    vector<vector<bool>> visited(ret.width(), vector<bool>(ret.height(), false));
    vector<vector<int>> distanceToStart(ret.width(), vector<int>(ret.height(), 0));
    Queue<pair<int,int>> exploreNext;

    if (start.first >= 0 && start.first < (int)ret.width() && 
        start.second >= 0 && start.second < (int)ret.height()) {
         visited[start.first][start.second] = true;
    }
    setGrey(ret, start);
    exploreNext.enqueue(start);

    while (exploreNext.isEmpty() == false) {
        pair<int,int> curr = exploreNext.dequeue();
        vector<pair<int,int>> p = neighbors(curr);

        for (int i = 0; i < 4; i++) {

            if(good(visited, curr, p[i])) {
                visited[p[i].first][p[i].second] = true;
                distanceToStart[p[i].first][p[i].second] = distanceToStart[curr.first][curr.second] + 1;
                setGrey(ret, p[i]);
                exploreNext.enqueue(p[i]);
            }
        }
    }

    // red square at start
    for (int i = -3; i <= 3; i++) {
        for (int j = -3; j <= 3; j++) {
            if ((start.first + i) >= 0 && (start.first + i) < (int)ret.width() && // changing to (unsigned) made it work --> before, red box at bottom from truncating out of bound
                (start.second + j) >= 0 && (start.second + j) < (int)ret.height()) {
                RGBAPixel *pRed = ret.getPixel(start.first + i, start.second + j);
                pRed->r = 255;
                pRed->g = 0;
                pRed->b = 0;
            }
        }
    }

    return ret;
}

bool treasureMap::good(vector<vector<bool>> & v, pair<int,int> curr, pair<int,int> next){
    if (next.first >= 0 && next.first < (int)base.width() && 
        next.second >= 0 && next.second < (int)base.height() && 
        v[next.first][next.second] == false) {

            RGBAPixel *pCurr = maze.getPixel(curr.first, curr.second);
            RGBAPixel *pNext = maze.getPixel(next.first, next.second);
            if (*pCurr == (*pNext)) {
                return true;
            }
    }
    return false;
}

vector<pair<int,int>> treasureMap::neighbors(pair<int,int> curr) {
    vector<pair<int,int>> ret;

    ret.push_back(make_pair(curr.first - 1, curr.second)); // left
    ret.push_back(make_pair(curr.first, curr.second + 1)); // bottom
    ret.push_back(make_pair(curr.first + 1, curr.second)); // right
    ret.push_back(make_pair(curr.first, curr.second - 1)); // top

    return ret;
}

