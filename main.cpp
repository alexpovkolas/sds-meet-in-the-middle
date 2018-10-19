#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>


#ifdef __PROFILE__

#include <cstdio>
#include <fstream>

#endif

using namespace std;

typedef long VALUE_TYPE;

struct Item {
    int id; // TODO: looks like we can remove id
    int weight;
    int cost;
};

struct Sack {
private:
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    vector<Item> &items; // TODO: check if we can get some memory leaks
public:

    Sack(int weight) : weight(weight), cost(0), items(*new vector<Item>()) {}
    Sack(): weight(0), cost(0), items(*new vector<Item>()) {}
    Sack& operator=(const Sack& a) {
        weight = a.weight;
        cost = a.cost;
        items = a.items;
        return *this;
    }

    inline void add_item(Item item) {
        weight += item.weight;
        cost += item.cost;
        items.push_back(item);
    }

    inline VALUE_TYPE get_weight() const {
        return weight;
    }

    inline VALUE_TYPE get_cost() const {
        return cost;
    }

    inline vector<Item>& get_items() const {
        return items;
    }

    inline void append(const Sack &s) {
        for (auto it = s.items.begin(); it != s.items.end(); ++it) {
            add_item(*it);
        }
//        sort(items.begin(), items.end());
    }

    struct less_than_by_weight {
        bool operator()(const Sack &a, const Sack &b) {
            return a.weight < b.weight;
        }
    };
};

struct better_sack : unary_function<Sack, bool> {
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    better_sack(VALUE_TYPE weight, VALUE_TYPE cost) : weight(weight), cost(cost) { }
    bool operator()(Sack const& sack) const {
        return sack.get_weight() < weight && sack.get_cost() > cost;
    }
};

Sack best_sack(vector<Item> &items, VALUE_TYPE weight) {
    // TODO: try to devide another way
    int left_size = items.size() / 2;
    int right_size = items.size() - left_size;

    vector<Sack> left(1 << left_size);

    // generate all the subsets
    for (int i = 0; i < 1 << left_size; ++i) {
        Sack sack;
        for (int j = 0; j < left_size ; ++j) {
            bool get_item = (i >> j) & 1;
            if (get_item) {
                sack.add_item(items[j]);
            }
        }
        left[i] = sack;
    }

    sort(left.begin(), left.end(), Sack::less_than_by_weight());

    // remove bad options
    for (auto iter = left.begin(); iter != left.end(); ) {

        auto better_one = find_if(iter, left.end(), better_sack(iter->get_weight(), iter->get_cost()));
        if (better_one != left.end()) {
            iter = left.erase(iter);
        } else {
            ++iter;
        }
    }

    // find the best pair
    VALUE_TYPE best_cost = 0;
    Sack best_left;
    Sack best_right;

    for (int i = 0; i < 1 << right_size; ++i) {
        Sack right;
        for (int j = 0; j < right_size ; ++j) {
            bool get_item = (i >> j) & 1;
            if (get_item) {
                right.add_item(items[j]);
            }
        }
        Sack max_weight(weight - right.get_weight());
        auto not_less_item = lower_bound(left.begin(), left.end(), max_weight, Sack::less_than_by_weight());

        if (not_less_item != left.end() && not_less_item != left.begin() ) {
            auto left = --not_less_item;
            if (best_cost < left->get_cost() + right.get_cost()) {
                best_cost = left->get_cost() + right.get_cost();
                best_left = *left;
                best_right = right;
            }
        }
    }

    best_left.append(best_right);
    return best_left;
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
    cout << result.get_items().size() << endl;
    for (auto it = result.get_items().begin(); it != result.get_items().end(); ++it)
        cout << it->id;

    cout << endl;

    return 0;
}
