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
    int id;
    int weight;
    int cost;
};

inline bool operator<(const Item& lhs, const Item& rhs) {
    return lhs.id < rhs.id;
}

struct Sack {
private:
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    vector<Item> &items; // TODO: check if we can get some memory leaks
public:

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
        sort(items.begin(), items.end());
    }
};

bool compareByWeight(const Sack &a, const Sack &b) {
    return a.get_weight() < b.get_weight();
}

struct better_sack : std::unary_function<Sack, bool> {
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    better_sack(VALUE_TYPE weight, VALUE_TYPE cost) : weight(weight), cost(cost) { }
    bool operator()(Sack const& sack) const {
        return sack.get_weight() < weight && sack.get_cost() > cost;
    }
};

struct max_weight_sack : std::unary_function<Sack, bool> {
    VALUE_TYPE weight;
    max_weight_sack(VALUE_TYPE weight) : weight(weight) { }
    bool operator()(Sack const& sack) const {
        return sack.get_weight() < weight;
    }
};



Sack best_sack(vector<Item> &items, int weight) {
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

    sort(left.begin(), left.end(), compareByWeight);

    // remove bad options
    for (auto iter = left.begin(); iter != left.end(); ) {

        auto better_one = find_if(iter, left.end(),
                                                    better_sack(iter->get_weight(), iter->get_cost()));
        if (better_one == left.end()) {
            iter = left.erase(iter);
        } else {
            ++iter;
        }
    }

    // find best pair
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
        auto not_less_item = lower_bound(left.begin(), left.end(), max_weight_sack(weight - right.get_weight()));

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
