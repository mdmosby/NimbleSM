#include "CollisionManager.h"
#include "kokkos_mock.hpp"

#include <mpi.h>
#include <iostream>

auto to_string(int i) -> std::string { return std::to_string(i); }
template <class T>
auto to_string(std::vector<T> const& v) -> std::string
{
    if (v.size() == 0)
        return "[]";
    std::string s = "[";
    for (auto& t : v)
    {
        s += to_string(t);
        s += ", ";
    }
    s.pop_back();
    s.back() = ']';
    return s;
}
// Has even ranks exchange data and odd ranks exchange data
void test_exchange(DataChannel channel, bool exclude_self)
{
    int my_rank = channel.commRank();
    int size    = channel.commSize();

    std::vector<int> ranks;
    for (int i = my_rank % 2; i < size; i += 2)
    {
        if (i == my_rank && exclude_self)
            continue;
        ranks.push_back(i);
    }

    std::vector<int> my_data;
    for (int i = 1; i < (my_rank + 1); i++)
    {
        my_data.push_back(my_rank + i * 1000);
    }

    auto all_data = channel.exchange(my_data, ranks);

    std::cerr << ("Rank " + to_string(my_rank) + " recieved "
                  + to_string(all_data) + '\n');
}
int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    auto channel = DataChannel{MPI_COMM_WORLD, 0};

    KokkosMock       coord_d_;
    double           background_grid_cell_size = 1.0;
    std::vector<int> exchange_members          = getExchangeMembers(
        coord_d_, background_grid_cell_size, MPI_COMM_WORLD, 0, 1, 2);

    for (int i = 0; i < exchange_members.size(); i++)
    {
        std::cout << i << std::endl;
    }

    MPI_Finalize();
}
