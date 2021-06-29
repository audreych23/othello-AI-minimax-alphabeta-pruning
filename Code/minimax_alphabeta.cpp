#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(int x, int y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

struct Node {
    Point point;
    //int hvalue;
    double hvalue;
};


//what you are can be black = 1, white = 2
int player;
const int SIZE = 8;

// the board that u took from the main.cpp
std::array<std::array<int, SIZE>, SIZE> board;

// valid spots right now that u got from the game 
std::vector<Point> next_valid_spots;

// disc count I added from the main, it's a bit useless but I try to keep track of the disc count 
std::array<int, 3> disc_count;



//directions useful on the flip function (copied frm main
const std::array<Point, 8> directions{ {
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    } };

// get who is the next turn plater
int get_next_player(int cur_player) {
    return 3 - cur_player;
}

// border of the board
bool is_spot_on_board(Point p) {
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}

// get_disc if it's black or white at point P
int get_disc(Point p, std::array<std::array<int, SIZE>, SIZE> currStateBoard) {
    return currStateBoard[p.x][p.y];
}

// set the disc blakc or white on point p
void set_disc(Point p, int disc, std::array<std::array<int, SIZE>, SIZE> &currStateBoard) {
    currStateBoard[p.x][p.y] = disc;
}

// is disc at point p connected to is spot valid and flip disc 
bool is_disc_at(Point p, int disc, std::array<std::array<int, SIZE>, SIZE> currStateBoard) {
    //if out or border then u quickly return false
    if (!is_spot_on_board(p))
        return false;
    
    if (get_disc(p, currStateBoard) != disc)
        return false;
    // e.g black turn if this point point is not white then return false, if it is white thentrue
   
    return true;
}
bool is_spot_valid(Point center, int cur_player, std::array<std::array<int, SIZE>, SIZE> currStateBoard) {
    if (get_disc(center, currStateBoard) != 0)
        return false;
    for (Point dir : directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player), currStateBoard))
            continue;
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p, currStateBoard) != 0) {
            if (is_disc_at(p, cur_player, currStateBoard))
                return true;
            p = p + dir;
        }
    }
    return false;
}
// for flipping disc and update board state
void flip_discs(Point center, int cur_player, std::array<std::array<int, SIZE>, SIZE> &currStateBoard) {
    for (Point dir : directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(p, get_next_player(cur_player), currStateBoard))
            continue;
        std::vector<Point> discs({ p });
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(p, currStateBoard) != 0) {
            if (is_disc_at(p, cur_player, currStateBoard)) {
                for (Point s : discs) {
                    set_disc(s, cur_player, currStateBoard);
                }
                disc_count[cur_player] += discs.size();
                disc_count[get_next_player(cur_player)] -= discs.size();
                break;
            }
            discs.push_back(p);
            p = p + dir;
        }
    }
}

// get all the valid spots for the current player
std::vector<Point> get_valid_spots(int cur_player, std::array<std::array<int, SIZE>, SIZE> currStateBoard){
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            // if there's a disc already then continue to the next point
            if (currStateBoard[i][j] != 0)
                continue;
            // else check if the ' u can put there as in the rule of othello ' if yes push_back
            if (is_spot_valid(p, cur_player, currStateBoard))
                valid_spots.push_back(p);
        }
    }
    // return whole vector
    return valid_spots;
}


std::array<std::array<int, SIZE>, SIZE> copy_board(std::array<std::array<int, SIZE>, SIZE> currStateBoard) {
    std::array<std::array<int, SIZE>, SIZE> temp;
    for (long long unsigned int i = 0; i < SIZE; i++) {
        for (long long unsigned int j = 0; j < SIZE; j++) {
            temp[i][j] = currStateBoard[i][j];
        }
    }
    return temp;
}

double greedy_heuristic_evaluation_function(std::array<std::array<int, SIZE>, SIZE> currStateBoard) {
    std::array<int, 3> currStateDiskCount = {};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (currStateBoard[i][j] == 1) {
                currStateDiskCount[1]++;
            }
            else if (currStateBoard[i][j] == 2) {
                currStateDiskCount[2]++;
            }
            else {
                currStateDiskCount[0]++;
            }
        }
    }
    double piece_diff = currStateDiskCount[player] - currStateDiskCount[get_next_player(player)];
    return piece_diff;
}
double mobility_heuristic_evaluation_function(std::array<std::array<int, SIZE>, SIZE> currStateBoard/*, int currPlayer*/) {
    int my_tiles = 0, opp_tiles = 0;
    double m_player = 0, m_opp = 0;
    //mobility
    if (currStateBoard[0][0] == player) my_tiles++;
    if (currStateBoard[0][7] == player) my_tiles++;
    if (currStateBoard[7][0] == player) my_tiles++;
    if (currStateBoard[7][7] == player) my_tiles++;
    if (currStateBoard[0][0] == get_next_player(player)) opp_tiles++;
    if (currStateBoard[0][7] == get_next_player(player)) opp_tiles++;
    if (currStateBoard[7][0] == get_next_player(player)) opp_tiles++;
    if (currStateBoard[7][7] == get_next_player(player)) opp_tiles++;

    
    //get the valid spot of the player right now
    std::vector<Point> temp = get_valid_spots(player, currStateBoard);
    m_player = temp.size();
    temp = get_valid_spots(get_next_player(player), currStateBoard);
    m_opp = temp.size();

    double heuristic = 10 * (my_tiles - opp_tiles) + ((m_player - m_opp) / (m_player + m_opp));
    return heuristic;

}
Node minimax(std::array<std::array<int, SIZE>, SIZE> currStateBoard, int currPlayer, int depth , double alpha, double beta/*, std::array<int, 3> disc_count_state*/){
    // valid move of every player with different board state every function call
    std::vector<Point> availablePoint = get_valid_spots(currPlayer, currStateBoard);

    if (disc_count[0] > 14) {
        if (depth == 6 ) {
            Node temp = { {0, 0}, mobility_heuristic_evaluation_function(currStateBoard) };
            return temp;   
        }
        else if (availablePoint.empty() && depth != 6) {
            Node temp = { {0, 0}, mobility_heuristic_evaluation_function(currStateBoard) };
            return temp;
        }
    }
    else if(disc_count[0] <= 14) {
        //change heuristic when empty <= 14
        if (depth == 14) {
                Node temp = { {0, 0}, greedy_heuristic_evaluation_function(currStateBoard};
                return temp;

        }
        else if (availablePoint.empty() && depth != 14) {
            Node temp = { {0, 0}, greedy_heuristic_evaluation_function(currStateBoard)};
            return temp;
        }
    }


    if (currPlayer == player) {
        double maxScore = -1e9;
        Point maxPoint = {-1, -1};
        for (unsigned long long int i = 0; i < availablePoint.size(); i++) {
            Point avaPoint = availablePoint[i];
            std::array<std::array<int, SIZE>, SIZE> temp = copy_board(currStateBoard);

            // update board
            set_disc(avaPoint, currPlayer, currStateBoard/*, disc_count_state*/);
            flip_discs(avaPoint, currPlayer, currStateBoard);

            Node result = minimax(currStateBoard, get_next_player(currPlayer), depth + 1, alpha, beta);
            currStateBoard = temp;
            if (maxScore < result.hvalue) {
                maxScore = result.hvalue;
                maxPoint = avaPoint;
            }
            alpha = std::max(alpha, maxScore);

            if (beta <= alpha) break;
        }
        return {maxPoint, maxScore};

    }
    else {
        // opponent plays optimally takes the minimum score
        double minScore = 1e9;
        Point minPoint = { -1, -1 };
        for (unsigned long long int i = 0; i < availablePoint.size(); i++) {
            Point avaPoint = availablePoint[i];
            std::array<std::array<int, SIZE>, SIZE> temp = copy_board(currStateBoard);

            //update board
            set_disc(avaPoint, currPlayer, currStateBoard);
            flip_discs(avaPoint, currPlayer, currStateBoard);            

            Node result = minimax(currStateBoard, get_next_player(currPlayer), depth + 1, alpha, beta);
            currStateBoard = temp;
            if (minScore > result.hvalue) {
                minScore = result.hvalue;
                minPoint = avaPoint;
            }
            beta = std::min(beta, minScore);
            
            if (beta <= alpha)break;
        }
        return { minPoint, minScore };
    }
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if (board[i][j] == 1) {
                // 1 is black
                disc_count[1]++;
            }
            else if (board[i][j] == 2) {
                // 2 is white
                disc_count[2]++;
            }
            else {
                // no black/ white
                disc_count[0]++;
            }
        }
    }
}


void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    // read valid spots
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    Node bestMove = minimax(board, player, 0, -1e9, 1e9);
    Point p = bestMove.point;
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

