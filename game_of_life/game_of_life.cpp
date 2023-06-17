#include <iostream>
#include <string>
#include <vector>
#include "game_of_life.h"
using namespace std;

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif


Grid::Grid(int g_width, int g_height, vector<int> g_start_array_xs, vector<int> g_start_array_ys, bool g_loop_around) {
    // start arrays contain the coordinates of the initial alive cells

    width = g_width;
    height = g_height;
    start_array_xs = g_start_array_xs;
    start_array_ys = g_start_array_ys;
    loop_around = g_loop_around;

    grid.resize(height, vector<int>(width, 0));
    
    for (int i = 0; i < start_array_xs.size(); i++) {
        grid[start_array_ys[i]][start_array_xs[i]] = 1;
    }
}

void Grid::print_grid() {
    char printout;
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            if (grid[i][j] == 1) {
                printout = 'o';
            }
            else {
                printout = '-';
            }
            cout << printout << " ";
        }   
        cout << endl;
    }
    cout << '\n';
}

int Grid::get_n_neighbours(int x, int y) {
    int n_neighbours = 0 - grid[y][x];

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {

            if (!loop_around) {
                if (((x+j)>=0 && (y+i)>=0) && ((x+j-width)<0 && (y+i-height)<0)) {
                    n_neighbours += grid[y+i][x+j];
                }
            }

            else {
                int r_i = i;
                int r_j = j;
                if (x+j < 0) {
                    r_j= width+j;
                }
                if (y+i < 0) {
                    r_i= height+i;
                }
                if ((x+j-width)>=0) {
                    r_j = j-width;
                }
                if ((y+i-height)>=0) {
                    r_i = i-height;
                }

                n_neighbours += grid[y+r_i][x+r_j];
            }
        }
    }

    return n_neighbours;
}


void Grid::evolve_one_step() {
    vector<vector<int>> cgrid(height, vector<int>(width, 0));
    for (int i = 0; i < cgrid.size(); i++) {
        for (int j = 0; j < cgrid[i].size(); j++) {
            int n_neighbours = get_n_neighbours(j,i);

            if (n_neighbours <= 1) {
                cgrid[i][j] = 0;
            }
            else if (n_neighbours == 3) {
                cgrid[i][j] = 1;
            }
            else if (n_neighbours >= 4) {
                cgrid[i][j] = 0;
            }
            else {
                cgrid[i][j] = grid[i][j];
            }
        }
    }

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            grid[i][j] = cgrid[i][j];
        }
    }
}


void Grid::evolve_n_steps(int n_steps, int sleep_ms) {
    for (int i = 0; i < n_steps; i++) {

        // this is not the prettiest, it just scrolls the terminal up at each print
        // but finding a more elegant solution seems more trouble than worth
        printf("\033[2J");
        printf("\033[%d;%dH", 0, 0);

        evolve_one_step();
        print_grid();
        usleep(sleep_ms*1000);
    }
}



int main() {
    Grid myGrid(51, 53, vector<int>{5,6,7,5,6}, vector<int>{5,5,5,6,7}, true);
    myGrid.print_grid();
    myGrid.evolve_n_steps(200, 50);
}