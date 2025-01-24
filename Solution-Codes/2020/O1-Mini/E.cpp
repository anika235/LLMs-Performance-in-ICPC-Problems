#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Fenwick Tree for range linear updates and point queries
struct FenwickTree {
    int n;
    vector<ll> tree;

    FenwickTree(int size) : n(size), tree(size + 2, 0) {}

    void update(int idx, ll val) {
        while(idx <= n){
            tree[idx] += val;
            idx += idx & -idx;
        }
    }

    ll query(int idx) const {
        ll res = 0;
        int i = idx;
        while(i > 0){
            res += tree[i];
            i -= i & -i;
        }
        return res;
    }
};

// Function to perform range linear update: add a*x + b to [l, r]
void range_linear_update(FenwickTree &BIT_a, FenwickTree &BIT_b, int l, int r, ll a, ll b){
    // Update BIT_a
    BIT_a.update(l, a);
    BIT_a.update(r+1, -a);
    // Update BIT_b
    BIT_b.update(l, b - a*(l-1));
    BIT_b.update(r+1, -(b - a*(l-1)));
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, k;
    cin >> n >> k;
    // Initialize two Fenwick Trees for a and b
    FenwickTree BIT_a(n);
    FenwickTree BIT_b(n);
    for(int op=0; op<k; op++){
        char c;
        int x1, x2;
        cin >> c >> x1 >> x2;
        if(c == 'R'){
            // Add 0*x +1 to [x1, x2]
            range_linear_update(BIT_a, BIT_b, x1, x2, 0, 1);
        }
        else if(c == 'D'){
            // Add 0*x -1 to [x1, x2]
            range_linear_update(BIT_a, BIT_b, x1, x2, 0, -1);
        }
        else if(c == 'H' || c == 'V'){
            // Hill or Valley
            // Determine the sign
            ll sign = (c == 'H') ? 1 : -1;
            // Calculate mid
            int mid = (x1 + x2) / 2;
            // Add to [x1, mid]: a=sign*1, b=sign*(-x1 +1)
            ll a1 = sign * 1;
            ll b1 = sign * (-x1 + 1);
            range_linear_update(BIT_a, BIT_b, x1, mid, a1, b1);
            // Add to [mid+1, x2]: a=sign*(-1), b=sign*(x2 +1)
            ll a2 = sign * (-1);
            ll b2 = sign * (x2 + 1);
            range_linear_update(BIT_a, BIT_b, mid+1, x2, a2, b2);
        }
    }
    // Now, compute and output the heights
    // To speed up, iterate from 1 to n and store results
    // Use a buffer to store the outputs and print at once
    // To handle large outputs efficiently
    stringstream ss;
    for(int i=1; i<=n; i++){
        ll height = BIT_a.query(i)*i + BIT_b.query(i);
        ss << height << "\n";
    }
    cout << ss.str();
}
