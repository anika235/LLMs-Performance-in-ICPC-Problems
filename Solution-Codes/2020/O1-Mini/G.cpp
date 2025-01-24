#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef pair<int, int> pii;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    struct Phone {
        int x, y, z;
        int idx;
    };
    vector<Phone> phones(n);
    vector<int> ys, zs;
    for(int i=0;i<n;i++){
        cin >> phones[i].x >> phones[i].y >> phones[i].z;
        phones[i].idx = i+1;
        ys.push_back(phones[i].y);
        zs.push_back(phones[i].z);
    }
    // Coordinate compression
    sort(ys.begin(), ys.end());
    ys.erase(unique(ys.begin(), ys.end()), ys.end());
    sort(zs.begin(), zs.end());
    zs.erase(unique(zs.begin(), zs.end()), zs.end());
    int m_y = ys.size();
    int m_z = zs.size();
    // Function to get compressed y
    auto get_compressed_y = [&](int y) -> int {
        return lower_bound(ys.begin(), ys.end(), y) - ys.begin() +1;
    };
    // Function to get compressed z
    auto get_compressed_z = [&](int z) -> int {
        return lower_bound(zs.begin(), zs.end(), z) - zs.begin() +1;
    };
    // Category2: max(xi + yi) where xi >xk and yi >yk
    // Sort descending x, then descending y
    vector<Phone> sorted_cat2 = phones;
    sort(sorted_cat2.begin(), sorted_cat2.end(), [&](const Phone &a, const Phone &b) -> bool{
        if(a.x != b.x) return a.x > b.x;
        return a.y > b.y;
    });
    // Initialize max_x_y
    vector<ll> max_x_y(m_y +2, LLONG_MIN);
    for(auto &p: sorted_cat2){
        int cy = get_compressed_y(p.y);
        max_x_y[cy] = max(max_x_y[cy], (ll)p.x + p.y);
    }
    // Compute suffix_max_x_y
    vector<ll> suffix_max_x_y(m_y +2, LLONG_MIN);
    suffix_max_x_y[m_y +1] = LLONG_MIN;
    for(int y = m_y; y >=1; y--){
        suffix_max_x_y[y] = max(suffix_max_x_y[y+1], max_x_y[y]);
    }
    // Category3: max(xi + zi) where xi >xk and zi >zk
    // Sort descending x, then descending z
    vector<Phone> sorted_cat3 = phones;
    sort(sorted_cat3.begin(), sorted_cat3.end(), [&](const Phone &a, const Phone &b) -> bool{
        if(a.x != b.x) return a.x > b.x;
        return a.z > b.z;
    });
    // Initialize max_x_z
    vector<ll> max_x_z(m_z +2, LLONG_MIN);
    for(auto &p: sorted_cat3){
        int cz = get_compressed_z(p.z);
        max_x_z[cz] = max(max_x_z[cz], (ll)p.x + p.z);
    }
    // Compute suffix_max_x_z
    vector<ll> suffix_max_x_z(m_z +2, LLONG_MIN);
    suffix_max_x_z[m_z +1] = LLONG_MIN;
    for(int z = m_z; z >=1; z--){
        suffix_max_x_z[z] = max(suffix_max_x_z[z+1], max_x_z[z]);
    }
    // Category5: max_x
    ll global_max_x = LLONG_MIN, global_max_y = LLONG_MIN, global_max_z_val = LLONG_MIN;
    for(auto &p: phones){
        global_max_x = max(global_max_x, (ll)p.x);
        global_max_y = max(global_max_y, (ll)p.y);
        global_max_z_val = max(global_max_z_val, (ll)p.z);
    }
    // Category6: max_y
    ll max_y = global_max_y;
    // Category7: max_z
    ll max_z = global_max_z_val;
    // Now, compute opportunity cost for each phone
    // Initialize the minimum opportunity cost and its index
    ll min_cost = LLONG_MAX;
    int min_idx = n+1;
    for(int i=0;i<n;i++){
        ll cost = 0;
        // Category1 is skipped, set to 0
        // Category2
        int cy = get_compressed_y(phones[i].y);
        if(cy < m_y){
            ll cat2 = suffix_max_x_y[cy +1] - (ll)phones[i].x - (ll)phones[i].y;
            cost = max(cost, cat2);
        }
        // Category3
        int cz = get_compressed_z(phones[i].z);
        if(cz < m_z){
            ll cat3 = suffix_max_x_z[cz +1] - (ll)phones[i].x - (ll)phones[i].z;
            cost = max(cost, cat3);
        }
        // Category4 is skipped
        // Category5
        if(global_max_x > (ll)phones[i].x){
            ll cat5 = global_max_x - (ll)phones[i].x;
            cost = max(cost, cat5);
        }
        // Category6
        if(global_max_y > (ll)phones[i].y){
            ll cat6 = global_max_y - (ll)phones[i].y;
            cost = max(cost, cat6);
        }
        // Category7
        if(global_max_z_val > (ll)phones[i].z){
            ll cat7 = global_max_z_val - (ll)phones[i].z;
            cost = max(cost, cat7);
        }
        // Update minimum cost and index
        if(cost < min_cost){
            min_cost = cost;
            min_idx = phones[i].idx;
        }
        else if(cost == min_cost && phones[i].idx < min_idx){
            min_idx = phones[i].idx;
        }
    }
    cout << min_cost << " " << min_idx;
}
