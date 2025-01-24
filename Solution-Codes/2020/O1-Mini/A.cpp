#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    ll r, c;
    cin >> r >> c;

    // Compute central rows
    vector<ll> central_rows;
    if(r %2 ==1){
        central_rows.push_back(r/2);
    }
    else{
        central_rows.push_back(r/2 -1);
        central_rows.push_back(r/2);
    }

    // Compute central columns
    vector<ll> central_cols;
    if(c %2 ==1){
        central_cols.push_back(c/2);
    }
    else{
        central_cols.push_back(c/2 -1);
        central_cols.push_back(c/2);
    }

    // Function to compute minimum distance to central positions
    auto min_distance = [&](ll i, ll j) -> ll{
        ll dist = LLONG_MAX;
        for(auto ir: central_rows){
            for(auto jc: central_cols){
                dist = min(dist, abs(i - ir) + abs(j - jc));
            }
        }
        return dist;
    };

    // Compute s
    double log_r = (c >1) ? log((double)r) / log((double)(c)) : 0;
    ll s =1;
    if(c >1){
        log_r = log((double)r) / log((double)(c));
        s = ceil(log_r) +1;
    }
    else{
        s = r;
    }

    ll best_p = 1;
    ll best_i = 0, best_j =0;
    ll best_dist = LLONG_MAX;

    for(ll p=1;p<=c;p++){
        // Compute l=(i,j)
        ll i =0;
        if(c-1 !=0){
            i = (p-1)*r / (c-1);
        }
        else{
            i =0;
        }
        ll j =0;
        if(c !=0){
            j = ((p-1)*r + i) %c;
        }
        // Check condition
        if(p !=1 && j ==0){
            continue;
        }
        // Compute distance
        ll dist = min_distance(i, j);
        if(dist < best_dist || (dist == best_dist && p < best_p)){
            best_dist = dist;
            best_p = p;
            best_i = i;
            best_j = j;
        }
    }

    // Output 1-based indices
    cout << best_p << " " << (best_i +1) << " " << (best_j +1) << " " << s;
}
