#include <vector>
#include <iostream>
#include <numeric>

using namespace std;

// Function to handle the READ command
void HandleReadCommand(vector<int>& user, vector<int>& page, int& total_user, int x, int y) {
    if (user[x] == 0) {
        user[x] = y;
        page[y]++;
        total_user++;
    } else {
        page[user[x]]--;
        user[x] = y;
        page[y]++;
    }
}

// Function to handle the CHEER command
double HandleCheerCommand(const vector<int>& page, const int total_user, int x) {
    if (total_user == 0) {
        return 0.0;
    } else if (total_user == 1) {
        return (user[x] == 0) ? 0.0 : 1.0;
    } else {
        return accumulate(page.begin(), page.begin() + user[x], 0) / static_cast<double>(total_user - 1);
    }
}

int main() {
    vector<int> user(100001,0);
    vector<int> page(1001,0);

    int x, y, total_user = 0, amount;
    string command;
    cin >> amount;

    for (int i = 0; i < amount; ++i) {
        cin >> command;
        if (command == "READ") {
            cin >> x >> y;
            HandleReadCommand(user, page, total_user, x, y);
        } else { // CHEER
            cin >> x;
            cout << HandleCheerCommand(page, total_user, x) << endl;
        }
    }

    return 0;
}
