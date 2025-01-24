#include <bits/stdc++.h>
using namespace std;

struct Region {
    int x1, y1, z1, x2, y2, z2;
};

struct Robot {
    long long x, y, z;
    int f; // 0:x+, 1:x-, 2:y+, 3:y-, 4:z+, 5:z-
    int d; // same encoding as f
};

// Map face string to integer
int face_map(string s) {
    if (s == "x+") return 0;
    if (s == "x-") return 1;
    if (s == "y+") return 2;
    if (s == "y-") return 3;
    if (s == "z+") return 4;
    if (s == "z-") return 5;
    return -1;
}

// Encode position as a unique long long
long long encode_pos(long long x, long long y, long long z, int f) {
    return x * 1000000000000LL + y * 1000000LL + z * 10LL + f;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, k;
    cin >> n >> k;
    vector<Region> regions(n);
    for(int i=0;i<n;i++) {
        cin >> regions[i].x1 >> regions[i].y1 >> regions[i].z1 >> regions[i].x2 >> regions[i].y2 >> regions[i].z2;
    }
    vector<Robot> robots(k);
    for(int i=0;i<k;i++) {
        string fx, dx;
        cin >> robots[i].x >> robots[i].y >> robots[i].z >> fx >> dx;
        robots[i].f = face_map(fx);
        robots[i].d = face_map(dx);
    }
    if(k <=1){
        cout << "ok";
        return 0;
    }
    // Direction to delta
    int dx_dir[6] = {1, -1, 0, 0, 0, 0};
    int dy_dir[6] = {0, 0, 1, -1, 0, 0};
    int dz_dir[6] = {0, 0, 0, 0, 1, -1};
    // Maximum steps
    int max_steps = 100000;
    // Initialize previous positions
    // Simulate
    for(int t=1;t<=max_steps;t++){
        // Store old positions
        vector<long long> old_pos(k);
        vector<long long> new_pos(k);
        for(int i=0;i<k;i++) {
            old_pos[i] = encode_pos(robots[i].x, robots[i].y, robots[i].z, robots[i].f);
        }
        // Compute new positions
        for(int i=0;i<k;i++) {
            int d = robots[i].d;
            long long nx = robots[i].x + dx_dir[d];
            long long ny = robots[i].y + dy_dir[d];
            long long nz = robots[i].z + dz_dir[d];
            int nf = d;
            int nd = robots[i].f;
            new_pos[i] = encode_pos(nx, ny, nz, nf);
            // Update for next step
            // robots[i].x = nx;
            // robots[i].y = ny;
            // robots[i].z = nz;
            // robots[i].f = nf;
            // robots[i].d = nd;
        }
        // Check collision type 1
        unordered_map<long long, int> pos_count;
        for(int i=0;i<k;i++) {
            pos_count[new_pos[i]]++;
        }
        bool collision = false;
        for(auto &[pos, cnt] : pos_count){
            if(cnt >=2){
                cout << t;
                return 0;
            }
        }
        // Check collision type 2
        // Build map from old_pos to new_pos
        unordered_map<long long, long long> map_old_to_new;
        for(int i=0;i<k;i++) {
            map_old_to_new[old_pos[i]] = new_pos[i];
        }
        for(int i=0;i<k;i++) {
            long long new_A = new_pos[i];
            long long old_A = old_pos[i];
            auto it = map_old_to_new.find(new_A);
            if(it != map_old_to_new.end()){
                if(it->second == old_A){
                    cout << t;
                    return 0;
                }
            }
        }
        // Update robots
        for(int i=0;i<k;i++) {
            // Decode new_pos[i] to update robot state
            long long pos = new_pos[i];
            int f = pos % 10;
            pos /= 10;
            long long z = pos % 1000000;
            pos /= 1000000;
            long long y = pos % 1000000;
            pos /= 1000000;
            long long x = pos;
            robots[i].x = x;
            robots[i].y = y;
            robots[i].z = z;
            robots[i].f = f;
            robots[i].d = face_map(to_string((f==0?"x+":f==1?"x-":f==2?"y+":f==3?"y-":f==4?"z+":f==5?"z-":"")));
        }
    }
    cout << "ok";
}
