#include "decoder.h"
#include "queue.h"
#include "stack.h"
using namespace std;

decoder::decoder(const PNG & tm, pair<int,int> s)
   :start(s),mapImg(tm) {
        PNG ret = PNG(mapImg);
        vector<vector<bool>> visited(ret.width(), vector<bool>(ret.height(), false));
        vector<vector<int>> distanceToStart(ret.width(), vector<int>(ret.height(), 0));
        vector<vector< pair<int,int> >> parent(ret.width(), vector<pair<int,int>>(ret.height(), make_pair(0,0)));
        Queue<pair<int,int>> exploreNext;

        if (start.first >= 0 && start.first < (int)ret.width() && 
            start.second >= 0 && start.second < (int)ret.height()) {
            visited[start.first][start.second] = true;
        }
        // setLOB(ret, start, 0);
        parent[start.first][start.second] = start;
        exploreNext.enqueue(start);
        
        pair<int,int> pathPt;
        pair<int,int> pathPtNext;

        while (exploreNext.isEmpty() == false) {
            pair<int,int> curr = exploreNext.dequeue();
            pathPt = curr;

            vector<pair<int,int>> p = neighbors(curr);
            for (int i = 0; i < 4; i++) {
                cout<< "before good" << endl; // call here 4 times, but good failing
                if(good(visited, distanceToStart, curr, p[i])) {
                    visited[p[i].first][p[i].second] = true;
                    distanceToStart[p[i].first][p[i].second] = distanceToStart[curr.first][curr.second] + 1;
                    parent[p[i].first][p[i].second] = curr;
                    cout<< "set Parent" << endl;
                    // setLOB(ret, p[i], distanceToStart[p[i].first][p[i].second]);
                    exploreNext.enqueue(p[i]);
                }    
            }
        }

        while (pathPt != start) {
            pathPtNext = parent[pathPt.first][pathPt.second];
            pathPts.insert(pathPts.begin(), pathPt);
            pathPt = pathPtNext;
            cout<< "pathPts" << endl;
        }

        pathPts.insert(pathPts.begin(), start);
   }

PNG decoder::renderSolution(){
    PNG ret = PNG(mapImg);

    for (int i = 0; i < pathLength(); i++) {
        pair<int,int> p = pathPts[i];
        RGBAPixel *pRGB = ret.getPixel(p.first, p.second);
        pRGB->r = 255;
        pRGB->g = 0;
        pRGB->b = 0;
    }

    return ret;
}

PNG decoder::renderMaze(){

    PNG ret = PNG(mapImg);
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

            if(good(visited, distanceToStart, curr, p[i])) {
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

void decoder::setGrey(PNG & im, pair<int,int> loc){
    RGBAPixel *p = im.getPixel(loc.first, loc.second);
    p->r = 2 * (p->r / 4);
    p->g = 2 * (p->g / 4);
    p->b = 2 * (p->b / 4);
}

pair<int,int> decoder::findSpot(){
    return pathPts[pathPts.size() - 1];
}

int decoder::pathLength(){
    return pathPts.size();
}

bool decoder::good(vector<vector<bool>> & v, vector<vector<int>> & d, pair<int,int> curr, pair<int,int> next){
    if (next.first >= 0 && next.first < (int)mapImg.width() && 
        next.second >= 0 && next.second < (int)mapImg.height() && 
        v[next.first][next.second] == false) {

            cout << "check good" << endl;
            RGBAPixel *pNext = mapImg.getPixel(next.first, next.second);
            if (compare(*pNext, d[curr.first][curr.second])) {
                return true;
            }
    }
    return false;
}

vector<pair<int,int>> decoder::neighbors(pair<int,int> curr) {
    vector<pair<int,int>> ret;

    ret.push_back(make_pair(curr.first - 1, curr.second)); // left
    ret.push_back(make_pair(curr.first, curr.second + 1)); // bottom
    ret.push_back(make_pair(curr.first + 1, curr.second)); // right
    ret.push_back(make_pair(curr.first, curr.second - 1)); // below

    return ret;
}


bool decoder::compare(RGBAPixel p, int d){
    // int loRed = p.r & 3; // mask with 0b11=3 to get 2 lowest order bits
    // int loGreen = p.g & 3; 
    // int loBlue = p.b & 3;
    // int n = (loRed << 4) + (loGreen << 2) + loBlue; // put the number together into one
    // return (n == (d+1)%64);

//     if ((((p.r % 4) * 16) + ((p.g % 4) * 4) + (p.g % 4)) == (d + 1) % 64) return true;
//     cout << "compare return false" << endl;
//    return false;

   int pRed = (p.r & 0b00000011) << 4;
   int pGreen = (p.g & 0b00000011) << 2;
   int pBlue = p.b & 0b00000011;
   int n = pRed + pGreen + pBlue;

   if (n == (d + 1) % 64) return true;
    return false;

}
