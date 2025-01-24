#include <bits/stdc++.h>
using namespace std;

// Flush output after writing to avoid issues with interactive judge
void flush_output() {
    cout.flush();
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Read the number of batches
    int b;
    cin >> b;

    // Process each batch
    for(int batch_i = 0; batch_i < b; batch_i++){
        // Read n
        int n;
        cin >> n;

        // We will keep track of test results in some data structure
        // For example, votes[i][j] = -1 means "unknown", 
        //  1 means "i declared j is correct", 
        //  0 means "i declared j is malfunctioning".
        vector<vector<int>> votes(n, vector<int>(n, -1));

        // Up to 12 rounds
        int MAX_ROUNDS = 12;

        // Our strategy: in each round, decide a schedule: whom does each machine test?
        // Store the schedule in a vector<int> testSched, where testSched[i] = x_i
        // If x_i = 0 => machine i is idle; otherwise x_i != i and all x_i are distinct among non-zero entries.

        for(int round = 0; round < MAX_ROUNDS; round++){
            // Decide some testing schedule
            // This is up to you how to do it. For example, do something naive or do a carefully planned approach.

            // Example (completely naive): everyone tests machine (i+1 mod n).  (Just for illustration!)
            vector<int> testSched(n, 0);  // all 0 => all idle
            for(int i = 0; i < n; i++){
                testSched[i] = (i + 1) % n + 1; 
                // But remember to ensure "all positive numbers in the sequence must be distinct".
                // So this example is invalid because multiple i might produce the same tested machine.
                // You need a permutation if you want them distinct, or partial usage of 0's.
            }

            // Convert testSched from 1-based indexing to a single line of text
            // They must be distinct among non-zero => you must ensure that in your code.
            cout << "test";
            for(int i = 0; i < n; i++){
                cout << " " << testSched[i];
            }
            cout << "\n";
            flush_output(); // important for interactive

            // Now read the judge response: a string of length n
            string result;
            cin >> result;

            // Parse the result
            // E.g. if result[i] is '1', then votes[i][ testSched[i]-1 ] = 1
            // if '0', then votes[i][ testSched[i]-1 ] = 0
            // if '-', then testSched[i] == 0 => idle
            for(int i = 0; i < n; i++){
                char c = result[i];
                if(c == '-') {
                    // Machine i was idle => nothing to record
                } else if(c == '1') {
                    // i said testSched[i] is correct
                    int testedIndex = testSched[i]-1; // convert from 1-based to 0-based
                    votes[i][testedIndex] = 1;
                } else if(c == '0') {
                    int testedIndex = testSched[i]-1;
                    votes[i][testedIndex] = 0;
                }
            }

            // Here you might decide if you have enough data to answer.
            // For example, if your strategy is to do X rounds, you can break early if you already concluded everything.
        }

        // After <= 12 rounds, decide the final status of each machine
        // We’ll do a majority vote approach:
        // For each machine j, gather all votes[i][j] where i != j and i tested j.
        // Because more than half of the i's are correct, the majority vote is the truth about j.

        vector<int> finalStatus(n, 1);  // assume correct until proven otherwise

        for(int j = 0; j < n; j++){
            // Collect all votes about j
            int countCorrect = 0, countWrong = 0;
            for(int i = 0; i < n; i++){
                if(i == j) continue;
                if(votes[i][j] == 1) countCorrect++;
                if(votes[i][j] == 0) countWrong++;
            }
            // If majority says j is correct, label j as correct, else malfunctioning
            // "Strictly more than half" are correct machines, so the correct "label" should have the majority.
            // But be mindful that malfunctioning testers can lie arbitrarily. 
            // The assumption is: correct testers form > n/2 => they will be the majority among distinct testers.
            if(countCorrect >= countWrong) {
                finalStatus[j] = 1;
            } else {
                finalStatus[j] = 0;
            }
        }

        // Build the output string
        string answerStr(n, '0');
        for(int i = 0; i < n; i++){
            if(finalStatus[i] == 1) answerStr[i] = '1';
            else answerStr[i] = '0';
        }

        // Print the answer
        cout << "answer " << answerStr << "\n";
        flush_output();

        // Then the judge will feed us the next batch's n (or end if b was exhausted)
    }

    return 0;
}
