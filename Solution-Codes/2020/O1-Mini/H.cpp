#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    int b;
    cin >> b;
    while(b--){
        int n;
        cin >> n;
        // Initialize a vector of vectors to store reports for each machine
        vector<vector<char>> reports(n, vector<char>());
        // To assign testing targets, we'll rotate the testing assignments
        // over multiple rounds to ensure each machine is tested by multiple others
        int rounds = 12;
        for(int r=0; r<rounds; r++){
            // Assign targets: machine i tests machine (i + r +1) % n
            vector<int> targets(n, 0);
            for(int i=0; i<n; i++){
                // Ensure a machine does not test itself
                int target = (i + r +1) % n;
                if(target == i){
                    target = (target +1) % n;
                }
                targets[i] = target +1; // 1-based indexing
            }
            // Ensure all positive numbers are distinct
            // If n >1, this assignment ensures distinct targets
            // If n ==1, targets[i] =0
            if(n ==1){
                targets[0] =0;
            }
            // Prepare the test command
            cout << "test";
            for(int i=0; i<n; i++) cout << " " << targets[i];
            cout << "\n";
            cout.flush();
            // Read the response
            string response;
            cin >> response;
            // Process the response
            for(int i=0; i<n; i++){
                if(targets[i] !=0){
                    reports[targets[i]-1].push_back(response[i]);
                }
            }
            // Optionally, implement early stopping if confident
            // For simplicity, continue all rounds
        }
        // After all rounds, determine the status of each machine
        string answer = "";
        for(int i=0; i<n; i++){
            // Count '1's and '0's in reports[i]
            int ones =0, zeros=0;
            for(auto c: reports[i]){
                if(c == '1') ones++;
                else if(c == '0') zeros++;
            }
            // More than half of the n machines are working correctly
            // So majority of reports are correct
            if(ones > zeros){
                answer += '1';
            }
            else{
                answer += '0';
            }
        }
        // Output the answer
        cout << "answer " << answer << "\n";
        cout.flush();
    }
}
