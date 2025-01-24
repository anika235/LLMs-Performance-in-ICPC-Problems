#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef pair<int, int> pii;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    double t;
    cin >> n >> t;
    vector<pair<double, double>> points(n);
    for(auto &p: points) cin >> p.first >> p.second;
    int max_count = 1;
    // To speed up, precompute all points as separate arrays
    vector<double> xs(n), ys(n);
    for(int i=0;i<n;i++){ xs[i]=points[i].first; ys[i]=points[i].second; }
    // Iterate over all pairs
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            double dx = xs[j] - xs[i];
            double dy = ys[j] - ys[i];
            double norm = sqrt(dx*dx + dy*dy);
            if(norm == 0) continue;
            double ux = -dy / norm;
            double uy = dx / norm;
            // Compute d_c1 and d_c2
            double d1 = xs[i]*ux + ys[i]*uy - t;
            double d2 = xs[i]*ux + ys[i]*uy + t;
            // For both d1 and d2
            for(auto d_c : {d1, d2}){
                int count = 0;
                for(int k=0;k<n;k++){
                    double dist = xs[k]*ux + ys[k]*uy - d_c;
                    if(abs(dist) <= t + 1e-9) count++;
                }
                if(count > max_count) max_count = count;
            }
        }
    }
    // Edge case: if n < 3
    if(n < 3){
        max_count = n;
    }
    cout << max_count;
}
