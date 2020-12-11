#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <cmath>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <climits>
#include <queue>
#include <deque>
#include <utility>
#include <assert.h>

using namespace std;
int n,m;
int d1;
int d2;
bool seen[256*256*256+1];

vector<string> grid;
const int mask1 = 15;
const int mask2 = (15<<4);
const int mask3 = (15<<8);
const int mask4 = (15<<12);


const int m1 = (1<<8)-1;
const int m2 = (m1<<8);
const int pmask= (m1<<16);
const int checkMask = (1<<16)-1;
const int hugeMask = (((1<<24)-1)<<8);

vector<pair<int,int>> directions={{-1,0}, {1,0}, {0,-1}, {0,1}};

bool valid(int x, int y, int state){
    return x<n && x>=0 && y>=0 && y<m && grid[x][y]!='*';
}

bool validBox(int x, int y, int state){
    int myCoord= (x<<4)+y;
    int box2= ((state & m2)>>8);
    int box1= (state & m1);

    return x<n && x>=0 && y>=0 && y<m && grid[x][y]!='*' && myCoord!=box1 && myCoord!=box2;
}

struct node{
    int dist;
    int state;
    node(int a, int b){
        dist=a;
        state=b;
    }
};

struct compareNode{
    bool operator()(node const & n1, node const & n2){
        return n1.dist > n2.dist;
    }
};

int main(){
    cin>>n>>m;
    int state=0; // x, p, b3b4, b1b2
    bool boxFlag=false;
    int target=-1;

    grid.clear();
    string line;
    for(int i=0;i<n;++i){
        cin>>line;
        string word="";
        for(int j=0;j<m;++j){
            if(line[j]=='#'){
                if(!boxFlag){ state |=  ((i<<4)+j); boxFlag=true;}
                else{state |= ((i<<12)+(j<<8)); }
            }
            else if(line[j]=='s'){
                state |= ((i<<20)+(j<<16));
            }
            else if(line[j]=='@'){
                if(target < 0){target = ((i<<4)+j); }
                else{
                    d1= (target<<8) + ((i<<4)+j);
                    d2= ((i<<12)+(j<<8)) + target;
                }
            }

            if(line[j]=='*'){
                word+='*';
            }
            else{word+='.';}
        }
        grid.push_back(word);
    }

    int result=1e9;
    deque<node> q;
    q.push_back(node(0, state));
    int count=0;
    while (!q.empty()){
        node currentNode= q.front();
        q.pop_front();
        int cState= currentNode.state;

        int p=((cState) & pmask)>>16;
        int x= (p & mask2)>>4;
        int y= p & mask1;
        if(!valid(x,y,cState) || seen[cState] ){
            continue;
        }
    
        int check= (cState & checkMask);
        if(check==d1 || check==d2){
            result= currentNode.dist;
            break;
        }
        
        seen[cState]=1;

        int res;
        for(pair<int,int> offset: directions){
            int nx= x+offset.first;
            int ny= y+offset.second;
            if(!valid(nx,ny,cState)){continue;}
            
            int b1= ((cState) & mask2)>>4;
            int b2= (cState) & mask1;

            if(nx==b1 && ny==b2){
                int fx= b1 + offset.first;
                int fy= b2 + offset.second;
                if(!validBox(fx,fy,cState)){
                    continue;
                }
                int newState = ((cState & (~m1)) & (~pmask));
                newState = (newState | (nx<<20)) | (ny<<16);
                newState = (newState | (fx<<4)) | fy ;
                q.push_back(node(currentNode.dist+1,newState));
                continue;
            }

            int b3= ((cState) & mask4)>>12;
            int b4= ((cState) & mask3)>>8;
            if(nx==b3 && ny==b4){
                int fx= b3 + offset.first;
                int fy= b4 + offset.second;
                if(!validBox(fx,fy,cState)){
                    continue;
                }
                int newState = ((cState & (~m2)) & (~pmask));
                newState = (newState | (nx<<20)) | (ny<<16);
                newState = (newState | (fx<<12)) | (fy<<8) ;
                q.push_back(node(currentNode.dist+1,newState));
                continue;
            }

            int newState = (cState & (~pmask));
            newState = (newState | (nx<<20)) | (ny<<16);
            if(!seen[newState] ){
                q.push_back(node(currentNode.dist+1, newState));
            }
            

        }
        
    }
    if(result==(int)1e9){result=-1;}
    cout<<result<<endl;
}