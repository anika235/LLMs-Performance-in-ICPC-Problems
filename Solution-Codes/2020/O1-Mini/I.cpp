#include <bits/stdc++.h>
using namespace std;

struct Quest {
    int x;
    int d;
};

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int n, v, c;
    cin >> n >> v >> c;
    vector<Quest> quests(n);
    long long sum_total_x = 0;
    for(int i=0; i<n; i++){
        cin >> quests[i].x >> quests[i].d;
        sum_total_x += quests[i].x;
    }
    // Sort quests in decreasing order of d_i * v
    sort(quests.begin(), quests.end(), [&](const Quest &a, const Quest &b) -> bool{
        long long da = (long long)a.d * v;
        long long db = (long long)b.d * v;
        if(da != db) return da > db;
        return a.x > b.x; // Tie-breaker, optional
    });
    // Min heap based on x_i
    priority_queue<int, vector<int>, std::greater<int>> pq;
    long long sum_before_XP = 0;
    long long sum_included_x = 0;
    for(int i=0; i<n; i++){
        long long deadline = (long long)quests[i].d * v;
        if(sum_before_XP < deadline){
            // Include the quest
            pq.push(quests[i].x);
            sum_before_XP += (long long)c * quests[i].x;
            sum_included_x += quests[i].x;
        }
        else{
            if(!pq.empty() && pq.top() < quests[i].x){
                // Replace the smallest x_j with current x_i
                int smallest_x = pq.top();
                pq.pop();
                sum_before_XP -= (long long)c * smallest_x;
                sum_included_x -= smallest_x;
                // Include current quest
                pq.push(quests[i].x);
                sum_before_XP += (long long)c * quests[i].x;
                sum_included_x += quests[i].x;
            }
            // Else, do not include the current quest
        }
    }
    // Calculate total_XP
    // total_XP = sum_total_x + (c -1) * sum_included_x
    long long total_XP = sum_total_x + (long long)(c -1) * sum_included_x;
    cout << total_XP;
}
