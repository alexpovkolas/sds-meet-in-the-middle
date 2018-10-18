#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

//#define __PROFILE__

#ifdef __PROFILE__

#include <cstdio>
#include <fstream>

#endif

using namespace std;

struct Item {
    int id;
    int weight;
    int cost;
};

inline bool operator<(const Item& lhs, const Item& rhs) {
    return lhs.id < rhs.id;
}

struct Sack {
    int weight;
    int cost;
    set<Item> &items;
};

Sack best_sack(vector<Item> &items, int weight) {
    

}

int main() {
#ifdef __PROFILE__
    ifstream in("input");
    cin.rdbuf(in.rdbuf());
#endif

    int n = 0;
    int w = 0;

    cin >> n >> w;
    vector<Item> items(n);

    for (int i = 0; i < n; ++i) {
        items[i].id = i;
        cin >> items[i].weight >> items[i].cost;
    }

    Sack result = best_sack(items, w);
    cout << result.items.size();
    for (auto it = result.items.begin(); it != result.items.end(); ++it)
        std::cout << it->id;

    return 0;
}
