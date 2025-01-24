#include <bits/stdc++.h>
using namespace std;

// Function to compute gcd
long long compute_gcd(long long a, long long b){
    while(b){
        long long tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

// Extended GCD to find x, y such that a*x + b*y = gcd(a,b)
long long extended_gcd_func(long long a, long long b, long long &x, long long &y){
    if(b == 0){
        x =1;
        y =0;
        return a;
    }
    long long x1, y1;
    long long g = extended_gcd_func(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return g;
}

// Function to find modular inverse of a mod m
long long mod_inverse(long long a, long long m){
    long long x, y;
    long long g = extended_gcd_func(a, m, x, y);
    if(g !=1){
        // Inverse doesn't exist
        return -1;
    }
    else{
        long long inv = (x % m + m) % m;
        return inv;
    }
}

// Function to compute S(t) mod g
long long compute_S_mod_g(int t, int d, long long g){
    if(d ==0){
        return 0;
    }
    // h = g / gcd(d, g)
    long long gcd_dg = compute_gcd(d, g);
    long long h = g / gcd_dg;
    // Compute 10^t mod (9*h)
    // Since (10^t -1)/9 mod h = (10^t mod (9*h) -1) /9 mod h
    // Handle t=0 separately
    if(t ==0){
        return 0;
    }
    long long mod_val = 9 * h;
    long long power_mod =1;
    for(int i=0;i<t;i++) {
        power_mod = (power_mod *10) % mod_val;
    }
    long long numerator = (power_mod -1 + mod_val) % mod_val;
    long long div = numerator /9;
    long long s_mod_h = div % h;
    long long S_mod_g = (s_mod_h * d) % g;
    return S_mod_g;
}

// Function to subtract two strings a - b, assuming a >= b and both have same length
string subtract_strings(const string &a, const string &b){
    string res = a;
    int n = a.size();
    int carry =0;
    for(int i=n-1;i>=0;i--){
        int digit_a = a[i] - '0';
        int digit_b = b[i] - '0';
        int sub = digit_a - digit_b - carry;
        if(sub <0){
            sub +=10;
            carry =1;
        }
        else{
            carry =0;
        }
        res[i] = sub + '0';
    }
    // Remove leading zeros
    int start =0;
    while(start < res.size()-1 && res[start]=='0') start++;
    return res.substr(start);
}

// Function to compare two strings a and b, assuming a and b have same length
bool compare_strings(const string &a, const string &b){
    return a < b;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(0);
    long long b;
    int d;
    string a;
    cin >> b >> d >> a;
    int L = a.size();
    // Precompute number of 2's and 5's in b
    int count2 =0, count5=0;
    long long tmp_b = b;
    while(tmp_b %2 ==0){
        count2++;
        tmp_b /=2;
    }
    while(tmp_b %5 ==0){
        count5++;
        tmp_b /=5;
    }
    // Maximum t can be up to L
    int t_max_possible = L;
    // To handle a up to 10^10000, set t up to min(L,10000)
    t_max_possible = min(t_max_possible, 10000);
    int t_ans =0;
    for(int t=1;t<=t_max_possible;t++){
        // Compute g = gcd(b, 10^t)
        int e = min(t, count2);
        int f = min(t, count5);
        long long g =1;
        for(int i=0;i<e;i++) g *=2;
        for(int i=0;i<f;i++) g *=5;
        // Compute S(t) mod g
        long long S_mod_g = compute_S_mod_g(t, d, g);
        // Check if g divides S(t)
        if(S_mod_g !=0){
            continue;
        }
        // Now, compute m =10^t /g
        // Since g divides 10^t, m is integer
        // m = 10^t /g
        // To compute 10^t /g, note that g =2^e *5^f, so m=2^{t-e} *5^{t-f}
        long long m =1;
        for(int i=0;i<t - e;i++) m *=2;
        for(int i=0;i<t - f;i++) m *=5;
        // n =b /g
        long long n = b /g;
        // S(t)/g = (d * (10^t -1)/9 ) /g = d*(10^t -1)/(9*g)
        // But since g divides S(t), it's integer
        // Compute S(t) /g mod n
        // S(t) = d * (10^t -1)/9
        // Compute S(t) mod (g*n)
        // S(t) = d * (10^t -1)/9
        // S(t) mod (g*n) = (S(t) mod (g*n))
        // Implemented as compute_S(t) mod (g*n)
        // Compute S(t) mod (g*n)
        // If d ==0, S(t)=0
        long long S_mod_gn =0;
        if(d !=0){
            // Compute S(t) mod (g*n)
            // S(t) = d * (10^t -1)/9
            // Compute (10^t -1)/9 mod (g*n)
            long long h = g * n;
            // Compute (10^t -1)/9 mod h
            // Reuse compute_S_mod_g with h
            // Here, h =g*n
            // And S(t) = d*(10^t -1)/9
            // Since g divides S(t), and n =b/g,
            // We can compute S(t) mod h
            // Reuse the compute_S_mod_g function
            // S(t) mod h
            // h might be up to 1e6 *1e6=1e12, but t up to 10000
            // Implement similarly
            // Reuse compute_S_mod_g with h
            // Here, g is already factored out
            // Thus, compute S(t) mod h
            // Which is d * (10^t -1)/9 mod h
            // Implement as follows:
            // h =g*n
            // If d and h are not coprime, adjust accordingly
            // Here, d is a digit, h can be up to1e12
            // Implement S(t) mod h
            // Use the same compute_S_mod_g function
            // Here, S(t) = d * (10^t -1)/9
            // Compute S(t) mod h
            // We can implement a similar function
            // But to simplify, compute S(t) directly
            // Since h can be up to1e12, which fits in long long
            // So, compute (10^t -1)/9 mod h
            // Then multiply by d and mod h
            // Implement as follows:
            long long power_mod =1;
            long long mod_val =9 * h;
            for(int i=0;i<t;i++) {
                power_mod = (power_mod *10) % mod_val;
            }
            long long numerator = (power_mod -1 + mod_val) % mod_val;
            long long div = numerator /9;
            long long s_mod_h = (div % h);
            S_mod_gn = (s_mod_h * d) % h;
        }
        // Now, S(t)/g mod n
        long long S_g =0;
        if(d !=0){
            S_g = S_mod_gn /g;
        }
        // c = (-S_g) mod n
        long long c = (-S_g) %n;
        if(c <0) c +=n;
        // Compute inverse of m mod n
        long long m_inv = mod_inverse(m, n);
        if(m_inv ==-1){
            continue; // No inverse, no solution
        }
        // y0 = (c * m_inv) mod n
        long long y0 = (c * m_inv) %n;
        // Now, compute y_max = floor((a - S(t))/10^t)
        // First, compute a - S(t)
        // Represent S(t) as string
        string S_t = "";
        if(d ==0){
            S_t = "";
            for(int i=0;i<t;i++) S_t += '0';
        }
        else{
            S_t = "";
            for(int i=0;i<t;i++) S_t += to_string(d)[0];
        }
        // If d !=0, S(t) is t digits of d
        // Else, S(t) is t digits of '0'
        // Now, perform a - S(t)
        // Compare a and S(t)
        // a has L digits
        // S(t) has t digits
        // If L < t, a < S(t)
        bool possible = false;
        string a_minus_S = "";
        if(t > L){
            // a < S(t)
            possible = false;
        }
        else{
            // Pad a with leading zeros if necessary
            string a_padded = a;
            // S(t) has t digits
            // a has L digits
            // If L >t, pad S(t) with (L -t) leading zeros
            string S_padded = S_t;
            if(L >t){
                string zeros(L -t, '0');
                S_padded = zeros + S_t;
            }
            // Else, L ==t
            // Now, compare a_padded and S_padded
            bool a_ge_S = false;
            if(a_padded.size() > S_padded.size()){
                a_ge_S = true;
            }
            else if(a_padded.size() < S_padded.size()){
                a_ge_S = false;
            }
            else{
                a_ge_S = a_padded >= S_padded;
            }
            if(a_ge_S){
                // Perform a - S_padded
                a_minus_S = subtract_strings(a_padded, S_padded);
                // Now, y_max = floor((a - S(t))/10^t)
                // Which is first (L -t) digits of a_minus_S
                // If L -t >6, y_max >=1e6
                // Else, convert to integer
                if(L > t +6){
                    // y_max >=1e6, and y0 <=1e6
                    possible = true;
                }
                else{
                    int digits = L -t;
                    string y_max_str = "";
                    if(d !=0 && L >t){
                        y_max_str = a_minus_S.substr(0, L -t);
                    }
                    else if(d ==0 && L >t){
                        y_max_str = a_minus_S.substr(0, L -t);
                    }
                    else{
                        y_max_str = "0";
                    }
                    // Remove leading zeros
                    int start =0;
                    while(start < y_max_str.size()-1 && y_max_str[start] == '0') start++;
                    y_max_str = y_max_str.substr(start);
                    long long y_max_val =0;
                    if(y_max_str.empty()) y_max_val =0;
                    else{
                        y_max_val = stoll(y_max_str);
                    }
                    if(y0 <= y_max_val){
                        possible = true;
                    }
                }
            }
            else{
                possible = false;
            }
        }
        if(possible){
            t_ans = t;
        }
    }
    cout << t_ans;
}
