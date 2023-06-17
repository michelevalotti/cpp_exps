#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Grid {
    bool loop_around;
    int width;
    int height;
    vector<int> start_array_xs;
    vector<int> start_array_ys;

    vector<vector<int>> grid;
    
    public:
        // constructor
        Grid(int g_width, int g_height, vector<int> g_start_array_xs, vector<int> g_start_array_ys, bool g_loop_around=false);
        
        // methods
        void print_grid();
        int get_n_neighbours(int x, int y);
        void evolve_one_step();
        void evolve_n_steps(int n_steps, int sleep_ms=200);
};