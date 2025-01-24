#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// Constants for hashing
const ll MOD1 = 1000000007;
const ll MOD2 = 1000000009;
const ll BASE = 911382629;

// Function to compute prefix hashes
struct PrefixHash {
    int n;
    vector<ll> H1, H2;
    vector<ll> pow1, pow2;
    
    PrefixHash(const string &s) {
        n = s.size();
        H1.assign(n + 1, 0);
        H2.assign(n + 1, 0);
        pow1.assign(n + 1, 1);
        pow2.assign(n + 1, 1);
        for(int i=0;i<n;i++){
            H1[i+1] = (H1[i] * BASE + s[i]) % MOD1;
            H2[i+1] = (H2[i] * BASE + s[i]) % MOD2;
            pow1[i+1] = (pow1[i] * BASE) % MOD1;
            pow2[i+1] = (pow2[i] * BASE) % MOD2;
        }
    }
    
    // Get hash of s[l..r-1]
    pair<ll, ll> getHash(int l, int r) const {
        ll hash1 = (H1[r] - (H1[l] * pow1[r - l]) % MOD1 + MOD1) % MOD1;
        ll hash2 = (H2[r] - (H2[l] * pow2[r - l]) % MOD2 + MOD2) % MOD2;
        return {hash1, hash2};
    }
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    string s;
    cin >> s;
    int n = s.size();
    if(n ==0){
        cout << "0\n";
        return 0;
    }
    // Create reversed string
    string s_rev = s;
    reverse(s_rev.begin(), s_rev.end());
    // Compute prefix hashes
    PrefixHash ph(s);
    PrefixHash phr(s_rev);
    // Compute k_max for each folding point
    // Folding points are between i and i+1, i from0 to n-1
    // For each folding point p=i, find max k where s[p -k +1 ..p] == reverse(s[p+1 ..p+k])
    // Which is s[p -k +1 ..p] == s_rev[n - (p +k +1) ..n - (p +1)]
    vector<int> k_max(n, 0);
    for(int p=0; p<n; p++){
        int max_possible_k = min(p+1, n - p -1);
        if(max_possible_k <=0){
            k_max[p]=0;
            continue;
        }
        int low=1, high=max_possible_k, res=0;
        while(low <= high){
            int mid = low + (high - low)/2;
            int l1 = p - mid +1;
            int r1 = p +1;
            // s[l1 .. p] vs reverse(s[p+1 .. p+mid])
            // Which is s[l1 .. p] vs s_rev[n - (p + mid +1) .. n - (p +1)]
            int rev_l = n - (p + mid +1);
            int rev_r = n - (p +1);
            if(rev_l <0 || rev_r >n){
                high = mid -1;
                continue;
            }
            pair<ll, ll> hash1 = ph.getHash(l1, p +1);
            pair<ll, ll> hash2 = phr.getHash(rev_l, rev_r);
            if(hash1 == hash2){
                res = mid;
                low = mid +1;
            }
            else{
                high = mid -1;
            }
        }
        k_max[p] = res;
    }
    // Initialize DP
    // dp[i] = minimal length for s[0..i-1]
    // dp[0] =0
    // dp[i] = min(dp[i], dp[i-1] +1, dp[i - 2*k] +1 for valid k)
    // To handle large n, use a vector with n+1 elements
    // Initialize with large values
    vector<int> dp(n +1, INT32_MAX);
    dp[0] =0;
    for(int p=0; p<n; p++){
        // Option1: not fold at p, add one character
        if(dp[p] +1 < dp[p +1]){
            dp[p +1] = dp[p] +1;
        }
        // Option2: fold at p with k_max[p]
        if(k_max[p] >=1){
            int k = k_max[p];
            int start = p - k +1;
            int end = p + k +1;
            if(start >=0 && end <=n){
                if(dp[start] +1 < dp[end]){
                    dp[end] = dp[start] +1;
                }
            }
        }
    }
    cout << dp[n] << "\n";
}
