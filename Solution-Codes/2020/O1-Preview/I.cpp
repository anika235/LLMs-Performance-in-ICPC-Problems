#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, v, c;
    cin >> n >> v >> c;

    vector<pair<long long,long long>> quests(n); 
    // We'll store (d_i, x_i) or better (d_i, x_i) so we can sort by d easily.
    // Actually, we want to keep them in a structure that is easy to sort by descending d.

    // But we also need x, so let's store as: {d, x}.
    for(int i=0; i<n; i++){
        long long x, d;
        cin >> x >> d;
        quests[i] = {d, x};
    }

    // Sort by d descending. If tie in d, we can break tie by anything — 
    // let's just break tie by x descending for consistency (though not strictly necessary).
    sort(quests.begin(), quests.end(), [&](auto &a, auto &b){
        if(a.first != b.first) return a.first > b.first;  // bigger d first
        return a.second > b.second; // if same d, bigger x first (arbitrary)
    });

    long long XP = 0; // track total XP
    for(auto &q : quests){
        long long d = q.first;
        long long x = q.second;
        // current level
        long long level = XP / v; 
        // decide how many XP we get
        if(level < d){
            // earn c*x
            XP += (c * x);
        } else {
            // earn x
            XP += x;
        }
    }

    cout << XP << "\n";

    return 0;
}
