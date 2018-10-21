#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <math.h>


#define __PROFILE__

#ifdef __PROFILE__

#include <cstdio>
#include <fstream>

#endif

using namespace std;

typedef long long VALUE_TYPE;

struct Item {
    short id;
    int weight;
    int cost;
};

struct Sack {
private:
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    vector<Item> *items; // TODO: check if we can get some memory leaks
public:

    Sack(VALUE_TYPE weight) : weight(weight), cost(0), items(new vector<Item>()) {}
    Sack(): weight(0), cost(0), items(new vector<Item>()) {}
    Sack& operator=(const Sack& a) {
        weight = a.weight;
        cost = a.cost;
        items = a.items;
        return *this;
    }

    inline void add_item(Item item) {
        weight += item.weight;
        cost += item.cost;
        items->push_back(item);
    }

    inline VALUE_TYPE get_weight() const {
        return weight;
    }

    inline VALUE_TYPE get_cost() const {
        return cost;
    }

    inline vector<Item>& get_items() const {
        return *items;
    }

    inline void append(const Sack &s) {
        for (auto it = s.items->begin(); it != s.items->end(); ++it) {
            add_item(*it);
        }
    }

    struct less_than_by_weight {
        bool operator()(const Sack &a, const Sack &b) {
            return a.weight < b.weight;
        }
    };

    struct greater_than_by_weight {
        bool operator()(const Sack &a, const Sack &b) {
            return a.weight > b.weight;
        }
    };


};

struct better_sack : unary_function<Sack, bool> {
    VALUE_TYPE weight;
    VALUE_TYPE cost;
    better_sack(VALUE_TYPE weight, VALUE_TYPE cost) : weight(weight), cost(cost) { }
    bool operator()(Sack const& sack) const {
        return sack.get_weight() <= weight && sack.get_cost() > cost;
    }
};

Sack best_sack(vector<Item> &items, VALUE_TYPE weight) {
    // TODO: try to divide another way
    size_t left_part = items.size() / 2;
    size_t right_part = items.size() - left_part;
    size_t left_size = pow(2, left_part);
    size_t right_size = pow(2, right_part);
    size_t mask = 1;



    vector<Sack> left(left_size);

    // generate all the subsets
    for (size_t i = 0; i < left_size; ++i) {
        Sack sack;
        for (size_t j = 0; j < left_part ; ++j) {
            bool get_item = ((i >> j) & mask ) > 0;
            if (get_item) {
                sack.add_item(items[j]);
            }
        }
        left[i] = sack;
    }

    sort(left.begin(), left.end(), Sack::greater_than_by_weight());

    // remove bad options
    // TODO: we can find and remove all items where get_weight > weight first
    vector<Sack> optimal_left;
    for (auto iter = left.begin(); iter != left.end(); ++iter) {

        auto better_one = find_if(iter, left.end(), better_sack(iter->get_weight(), iter->get_cost()));
        if (better_one == left.end() && iter->get_weight() <= weight) {
            optimal_left.push_back(*iter);
        }
    }
    left = optimal_left;

    // find the best pair
    Sack best_left = left.empty() ? Sack() : left[0];
    Sack best_right;
    VALUE_TYPE best_cost = best_left.get_cost();

    for (size_t i = 0; i < right_size; ++i) {
        Sack right;
        for (size_t j = 0; j < right_part ; ++j) {
            bool get_item = ((i >> j) & mask) > 0;
            if (get_item) {
                right.add_item(items[left_part + j]);
            }
        }
        if (right.get_weight() > weight) {
            continue;
        }
        Sack max_weight(weight - right.get_weight() + 1);
        auto not_less_it = lower_bound(left.rbegin(), left.rend(), max_weight, Sack::less_than_by_weight());

        Sack current_best_left = not_less_it == left.rbegin() ? *not_less_it : *--not_less_it;
        if (best_cost < current_best_left.get_cost() + right.get_cost() && weight >= current_best_left.get_weight() + right.get_weight()) {
            best_cost = current_best_left.get_cost() + right.get_cost();
            best_left = current_best_left;
            best_right = right;

#ifdef __PROFILE__
            cout << "current best cost " << best_cost << endl;
#endif
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
        items[i].id = i + 1;
        cin >> items[i].weight >> items[i].cost;
    }

    Sack result = best_sack(items, w);
    cout << result.get_items().size() << endl;
    for (auto it = result.get_items().begin(); it != result.get_items().end(); ++it)
        cout << it->id << " ";

#ifdef __PROFILE__
    cout << endl << "weight " << result.get_weight() << " cost " << result.get_cost();
#endif

    cout << endl;

    return 0;
}
