#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n;
    cin >> n;
    // Initialize degrees
    vector<int> degree(n+1, 0);
    ll sum =0;
    for(int i=0;i<n-1;i++){
        int a, b, d;
        cin >> a >> b >> d;
        degree[a]++;
        degree[b]++;
        sum += d;
    }
    // Calculate sum_deg_over2
    int sum_deg_over2=0;
    for(int i=1;i<=n;i++) {
        if(degree[i] >2){
            sum_deg_over2 += (degree[i] -2);
        }
    }
    // Calculate ceil(sum_deg_over2 /2)
    ll extra = (ll)(sum_deg_over2 +1)/2;
    // Total traversal sum
    ll total_sum = sum + 2*extra;
    cout << total_sum;
}
