#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>

using namespace std;

class User {
public:
    string userId;
    string name;
    string email;
    string mobile;

    User(string userId, string name, string email = "", string mobile = "")
        : userId(userId), name(name), email(email), mobile(mobile) {}
};

class Splitwise {
private:
    unordered_map<string, User*> users;     // <userId : user object>
    unordered_map<string, unordered_map<string, double>> balances;  // <userId (debt) : <userId (owns) : amount> basically <user 1 owes user2 x amount>  

    void addBalance(string paidBy, string userId, double amount) {
        balances[userId][paidBy] += amount;
        balances[paidBy][userId] -= amount;
    }

    double roundOff(double value) {
        return round(value * 100.0) / 100.0;
    }

public:
    void addUser(string userId, string name, string email, string mobile) {
        if (users.find(userId) == users.end()) {
            users[userId] = new User(userId, name, email, mobile);
        }
    }

    void addExpense(string paidBy, double totalAmount, vector<string> participants, string type, vector<double> amounts = {}) {
        if (type == "equal") {
            double splitAmount = roundOff(totalAmount / participants.size());
            for (int i = 0; i < participants.size(); i++) {
                if (participants[i] != paidBy) {
                    if (i == participants.size() - 1) {
                        splitAmount = totalAmount - splitAmount * (participants.size() - 1);
                    }
                    addBalance(paidBy, participants[i], splitAmount);
                }
            }
        } 
        else if (type == "exact") {
            double sum = 0;
            for (double amount : amounts) {
                sum += amount;
            }
            if (roundOff(sum) != totalAmount) {
                cout << "Error: Exact split amounts do not sum up to total amount." << endl;
                return;
            }
            for (int i = 0; i < participants.size(); i++) {
                if (participants[i] != paidBy) {
                    addBalance(paidBy, participants[i], amounts[i]);
                }
            }
        } 
        else if (type == "percent") {
            double sumPercent = 0;
            for (double percent : amounts) {
                sumPercent += percent;
            }
            if (roundOff(sumPercent) != 100) {
                cout << "Error: Percent split amounts do not sum up to 100%." << endl;
                return;
            }
            for (int i = 0; i < participants.size(); i++) {
                if (participants[i] != paidBy) {
                    addBalance(paidBy, participants[i], roundOff(totalAmount * (amounts[i] / 100)));
                }
            }
        }
    }

    void showBalances(string userId = "") {
        if(balances.size() == 0) {
            cout << "No balances available"<<endl;
            return;
        }
        if (userId == "") {
            for (auto& userBalance : balances) {
                for (auto& balance : userBalance.second) {
                    if (balance.second > 0) {
                        cout << userBalance.first << " owes " << balance.first << ": " << fixed << setprecision(2) << balance.second << endl;
                    }
                }
            }
        }
        else {
            if (balances.find(userId) == balances.end()) {
                cout << "No balances for " << userId << endl;
            } else {
                for (auto& balance : balances[userId]) {
                    if (balance.second > 0) {
                        cout << userId << " owes " << balance.first << ": " << fixed << setprecision(2) << balance.second << endl;
                    }
                    if (balance.second < 0) {
                        cout << balance.first << " owes " << userId << ": " << fixed << setprecision(2) << -1 * balance.second << endl;
                    }
                }
            }
        }
    }
};

int main() {
    Splitwise app;
    
    // Adding some users (in a real scenario, this could also be taken as input)
    // app.addUser("u1", "Alice", "alice@example.com", "1234567890");
    // app.addUser("u2", "Bob", "bob@example.com", "1234567891");
    // app.addUser("u3", "Charlie", "charlie@example.com", "1234567892");
    // app.addUser("u4", "David", "david@example.com", "1234567893");

    int n;
    cout << "Enter the Number of Users : ";
    cin >> n;

    for(int i = 1; i <= n; i++){
        string str = "u";
        str += to_string(i);
        string name;
        cout << "Enter the Name of the User "<<i<<" : ";
        cin >> name;
    }

    cout<<"Users Added Successfully !"<<endl<<"Now you can use Split-wise !"<<endl;

    string command;
    while (getline(cin, command)) {
        istringstream iss(command);
        string action;
        iss >> action;

        if (action == "show") {
            string userId;
            iss >> userId;
            app.showBalances(userId);
        }
        else if (action == "expense") {
            string paidBy, type;
            int numUsers;
            double amount;
            vector<string> participants;
            vector<double> shares;

            iss >> paidBy >> amount >> numUsers;

            for (int i = 0; i < numUsers; i++) {
                string userId;
                iss >> userId;
                participants.push_back(userId);
            }

            iss >> type;

            if (type == "exact" || type == "percent") {
                for (int i = 0; i < numUsers; i++) {
                    double share;
                    iss >> share;
                    shares.push_back(share);
                }
            }

            app.addExpense(paidBy, amount, participants, type, shares);
        }
    }

    return 0;
}
