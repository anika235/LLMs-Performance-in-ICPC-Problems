#include <bits/stdc++.h>
using namespace std;

typedef long long ll;

struct Node {
    int id;
    vector<pair<int, int>> children; // pair of child id and road speed
} nodes_struct;

int n;
ll c;

// To store road speeds to parent
vector<int> parent_speed;

// Memoization table: dp[u][r] = minimal cost
// Using vector of unordered_map
vector<unordered_map<ll, ll>> dp_table;

// Function to compute maximum speed of roads to children
ll get_max_s(int u) {
    ll m = 0;
    for(auto &[v, s] : nodes_struct.children[u].children) {
        m = max(m, (ll)s);
    }
    return m;
}

// DFS function
ll dfs(int u, ll r, int parent) {
    // Check if already computed
    if(dp_table[u].find(r) != dp_table[u].end()) {
        return dp_table[u][r];
    }

    // Determine degree
    int degree = nodes_struct.children[u].children.size();
    if(parent != -1) degree++; // include parent road

    ll max_s = 0;
    for(auto &[v, s] : nodes_struct.children[u].children) {
        max_s = max(max_s, (ll)s);
    }

    if(parent != -1) {
        // Include the road to parent in max_s only if not installing signs
        // But in state0, s_u = max(r, max_s)
    }

    // If degree >=2
    if(degree >=2){
        // Option1: install signs
        ll cost1 = degree * c;
        for(auto &[v, s] : nodes_struct.children[u].children){
            cost1 += dfs(v, s, u);
        }
        // Option2: not install signs
        ll s_u = max(r, max_s);
        // If not root, need to ensure s_u >= road to parent
        if(parent != -1 && s_u < (ll)parent_speed[u]){
            // Invalid option
            // Option2 is not feasible
            dp_table[u][r] = cost1;
            return cost1;
        }
        ll cost2 = 0;
        for(auto &[v, s] : nodes_struct.children[u].children){
            cost2 += (s_u - (ll)s);
        }
        for(auto &[v, s] : nodes_struct.children[u].children){
            cost2 += dfs(v, s_u, u);
        }
        ll res = min(cost1, cost2);
        dp_table[u][r] = res;
        return res;
    }
    else if(degree ==1){
        if(parent == -1){
            // Root with degree1
            // Option1: install signs
            ll cost1 = c;
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost1 += dfs(v, s, u);
            }
            // Option2: not install signs
            ll s_u = max(r, max_s);
            ll cost2 = 0;
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost2 += (s_u - (ll)s);
            }
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost2 += dfs(v, s_u, u);
            }
            ll res = min(cost1, cost2);
            dp_table[u][r] = res;
            return res;
        }
        else{
            // Non-root with degree1, cannot install signs
            ll s_u = max(r, (ll)parent_speed[u]);
            ll cost = 0;
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost += (s_u - (ll)s);
            }
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost += dfs(v, s_u, u);
            }
            dp_table[u][r] = cost;
            return cost;
        }
    }
    else{
        // degree ==0, leaf node
        // If root with degree0
        if(parent == -1){
            // Single node
            // Option1: install signs (no roads, cost0)
            // Option2: not install signs (no roads, cost0)
            dp_table[u][r] = 0;
            return 0;
        }
        else{
            // Non-root leaf
            ll s_u = max(r, (ll)parent_speed[u]);
            // No children
            dp_table[u][r] = 0;
            return 0;
        }
    }
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    cin >> n >> c;
    // Initialize nodes
    vector<vector<pair<int, int>>> adj(n+1, vector<pair<int, int>>());
    for(int i=0;i<n-1;i++){
        int u, v, s;
        cin >> u >> v >> s;
        adj[u].emplace_back(v, s);
        adj[v].emplace_back(u, s);
    }
    // Build tree rooted at 1
    // Assign children and parent_speed
    // Initialize
    vector<int> parent(n+1, -1);
    parent_speed.assign(n+1, 0);
    // Initialize nodes_struct
    nodes_struct.children.resize(n+1);
    // BFS to assign parent and children
    queue<int> q;
    q.push(1);
    parent[1] = -2; // mark as root
    while(!q.empty()){
        int u = q.front(); q.pop();
        for(auto &[v, s] : adj[u]){
            if(parent[v] == -1){
                parent[v] = u;
                parent_speed[v] = s;
                nodes_struct.children[u].children.emplace_back(v, s);
                q.push(v);
            }
        }
    }
    // Initialize DP table
    dp_table.resize(n+1);
    // Start DFS from root with r=0
    // Implement dfs correctly
    // Implemented as a recursive function with memoization
    // To prevent stack overflow, use iterative DFS with memoization
    // But for simplicity, use recursion with increased stack size
    // Use a global stack limit if necessary
    // In practice, C++ should handle up to 20000 recursion depth
    // Implement the dfs function correctly
    // Modify the dfs function to handle the correct logic
    // Re-implement the dfs function with correct logic
    // Redefine the dfs function
    // Clear dp_table
    dp_table.assign(n+1, unordered_map<ll, ll>());
    // Redefine dfs with correct logic
    // Re-implement the dfs function
    // To handle degrees correctly
    // Implement using recursion
    // Re-implement the function with correct conditions
    // Rewriting the dfs function
    // Using memoization
    // Rewriting as follows:

    // Reinitialize dp_table
    dp_table.assign(n+1, unordered_map<ll, ll>());
    
    // Define a new dfs function
    function<ll(int, ll)> dfs_correct = [&](int u, ll r) -> ll {
        if(dp_table[u].find(r) != dp_table[u].end()) {
            return dp_table[u][r];
        }
        // Determine degree
        int degree = nodes_struct.children[u].children.size();
        if(parent[u] != -1){
            degree +=1;
        }
        if(degree >=2){
            // Option1: install signs
            ll cost1 = (ll)degree * c;
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost1 += dfs_correct(v, (ll)s);
            }
            // Option2: not install signs
            ll max_s = 0;
            for(auto &[v, s] : nodes_struct.children[u].children){
                max_s = max(max_s, (ll)s);
            }
            ll s_u = max(r, max_s);
            if(parent[u] != -1 && s_u < (ll)parent_speed[u]){
                // Option2 invalid
                dp_table[u][r] = cost1;
                return cost1;
            }
            ll cost2 = 0;
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost2 += (s_u - (ll)s);
            }
            for(auto &[v, s] : nodes_struct.children[u].children){
                cost2 += dfs_correct(v, s_u);
            }
            ll res = min(cost1, cost2);
            dp_table[u][r] = res;
            return res;
        }
        else if(degree ==1){
            if(parent[u] == -1){
                // Root with degree1
                // Option1: install signs
                ll cost1 = (ll)c;
                for(auto &[v, s] : nodes_struct.children[u].children){
                    cost1 += dfs_correct(v, (ll)s);
                }
                // Option2: not install signs
                ll max_s = 0;
                for(auto &[v, s] : nodes_struct.children[u].children){
                    max_s = max(max_s, (ll)s);
                }
                ll s_u_opt2 = max(r, max_s);
                ll cost2 = 0;
                for(auto &[v, s] : nodes_struct.children[u].children){
                    cost2 += (s_u_opt2 - (ll)s);
                }
                for(auto &[v, s] : nodes_struct.children[u].children){
                    cost2 += dfs_correct(v, s_u_opt2);
                }
                ll res = min(cost1, cost2);
                dp_table[u][r] = res;
                return res;
            }
            else{
                // Non-root with degree1, cannot install signs
                // s_u = max(r, road to parent s_e}
                ll s_u = max(r, (ll)parent_speed[u]);
                // Set roads to children to s_u
                ll cost =0;
                for(auto &[v, s] : nodes_struct.children[u].children){
                    cost += (s_u - (ll)s);
                }
                for(auto &[v, s] : nodes_struct.children[u].children){
                    cost += dfs_correct(v, s_u);
                }
                dp_table[u][r] = cost;
                return cost;
            }
        }
        else{
            // degree ==0
            if(parent[u] == -1){
                // Single node
                dp_table[u][r] = 0;
                return 0;
            }
            else{
                // Leaf node
                // s_u = max(r, road to parent s_e}
                ll s_u = max(r, (ll)parent_speed[u]);
                // No children
                dp_table[u][r] = 0;
                return 0;
            }
        }
    };

    // Call dfs_correct on root with r=0
    ll result = dfs_correct(1, 0);
    cout << result;
}
