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
    int myCoord= (x<<4)+y;
    int box2= ((state & m2)>>8);
    int box1= (state & m1);

    return x<n && x>=0 && y>=0 && y<m && grid[x][y]!='*' && myCoord!=box1 && myCoord!=box2;
}

int bfs(int choice, pair<int,int> offset, int state){
    int p = ((state & pmask)>>16);
    int dst,t1,t2;
    if(choice==0){
        dst= (state & m1);
        t1= ((dst & mask2)>>4);
        t2= (dst & mask1);
    }
    else{
        dst= ((state & m2) >> 8);
        t1= ((dst & mask2)>>4);
        t2= (dst & mask1);
    }
    t1=t1-offset.first;
    t2=t2-offset.second;

    unordered_set<int> past;
    deque<int> q;
    q.push_back(p);
    while(!q.empty()){
        int group =q.front();
        q.pop_front();
        int y= (group & mask1);
        int x= ((group & mask2) >>4);
        int currentDist = ((group & hugeMask)>>8);

        if(!valid(x,y,state) || past.find((x<<4)+y)!=past.end() ){
            continue;
        }
        if(x==t1 && y==t2){
            return currentDist;
        }
        past.insert((x<<4)+y);
        for(pair<int,int> offset: directions){
            int nx= x+offset.first;
            int ny=y+offset.second;
            q.push_back(((currentDist+1)<<8) + (nx<<4) + ny);
        }
    }
    return -1;
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
    unordered_map<int, int> seen;
    priority_queue<node, vector<node>, compareNode> q;
    q.push(node(0, state));
    int count=0;
    while (!q.empty()){
        node currentNode= q.top();
        q.pop();

        if(seen.find(currentNode.state) != seen.end()){
            if(currentNode.dist > seen[currentNode.state]){
                continue;
            }
        }

        if(currentNode.dist>result){break;}
    
        int check= (currentNode.state & checkMask);
        if(check==d1 || check==d2){
            result=min(result,currentNode.dist);
            continue;
        }
        

        /*
        if(count<20){
            cout<<"b1 is "<<(check&(m1))<<" b2 is "<<((check&m2)>>8)<<" p is "<<(((currentNode.state)&pmask)>>16)<<" dist is "<<currentNode.dist<<endl;
            count++;
        }*/

        int res;
        for(pair<int,int> offset: directions){
            res = bfs(0,offset,currentNode.state)+1;

            if(res>0){
                int b1= ((currentNode.state) & mask2)>>4;
                int b2= (currentNode.state) & mask1;
                int fx= b1+offset.first;
                int fy= b2+offset.second;
                if(valid(fx,fy,currentNode.state)){
                    int newDist= currentNode.dist + res;
                    int newState = ((currentNode.state & (~m1)) & (~pmask));
                    newState = (newState | (b1<<20)) | (b2<<16);
                    newState = (newState | (fx<<4)) | fy ;

                    if(seen.find(newState) != seen.end()){
                        int oldDist= seen[newState];
                        if(newDist<oldDist){
                            seen[newState]= newDist;
                            q.push(node(newDist,newState));
                        }
                    }
                    else{
                        seen[newState]= newDist;
                        q.push(node(newDist, newState));
                    }
                }
            }

            res = bfs(1,offset, currentNode.state) +1;
            if(res >0){
                int b3= ((currentNode.state) & mask4)>>12;
                int b4= ((currentNode.state) & mask3)>>8;
                int fx= b3+offset.first;
                int fy= b4+offset.second;
                if(valid(fx,fy,currentNode.state)){
                    int newDist= currentNode.dist + res;
                    int newState = ((currentNode.state & (~m2)) & (~pmask));
                    newState = (newState | (b3<<20)) | (b4<<16);
                    newState = (newState | (fx<<12)) | (fy<<8) ;

                    if(seen.find(newState) != seen.end()){
                        int oldDist= seen[newState];
                        if(newDist<oldDist){
                            seen[newState]= newDist;
                            q.push(node(newDist,newState));
                        }
                    }
                    else{
                        seen[newState]= newDist;
                        q.push(node(newDist, newState));
                    }
                }

            }
            

        }
        
    }
    if(result==(int)1e9){result=-1;}

    cout<<result<<endl;
}