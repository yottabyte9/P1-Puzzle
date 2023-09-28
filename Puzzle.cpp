// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725


#include <iostream>
#include <vector>
#include <fstream>
#include <deque>
#include <sstream>
#include <getopt.h>
#include <ctype.h>
#include <stack>
#include <queue>
#include <list>
#include <algorithm>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <cstring>


using namespace std;


struct Position {
    char color;
    int row;
    int col;
};


string output_str;
int numcolors = 0;
int cols = 0;
int rows = 0;
bool queue_b = false;
bool stack_b = false;
bool solved_bool = false;


vector<vector<vector<char>>> threeDmap;
vector<vector<char>> twoDmap;


pair<int,int> start_loc(-1,-1);
pair<int,int> end_tar(-1,-1);
char end_color;




deque<Position> search_container;


string cl_outputs(int argc, char **argv);
bool input_fail();
vector<vector<char>> create_map();
int color_to_int(char color);
void solve();
bool validInput(char symbol);
void output_both(vector<vector<vector<char>>> &threeDmap, vector<vector<char>> &twoDmap, string output_str);
//void noSolution(vector<vector<vector<char>>> &threeDmap, vector<vector<char>> &twoDmap);


int main(int argc, char **argv){
    ios_base::sync_with_stdio(false);


    output_str = cl_outputs(argc, argv);
    //cout << "finished cl_outputs\n";
    solve();
}


string cl_outputs(int argc, char** argv){
    int gotopt;
    int option_index = 0;
    option long_opts[]={
        {"help", no_argument, nullptr, 'h'},
        {"queue", no_argument, nullptr, 'q'},
        {"stack", no_argument, nullptr, 's'},
        {"output", required_argument, nullptr, 'o'},
        {0, 0, 0, 0}
    };


    bool flagged = false;
    string optargs;
    string flag;
    opterr = false;
    while((gotopt = getopt_long(argc, argv, "hqso:", long_opts, &option_index)) != -1){
        //cout << gotopt << "\n";
        switch(gotopt){
            case 'h':
                cout << "help";
                exit(0);
            case 'q':
                if(flagged == true){
                    cout << "only one flag allowed";
                    exit(1);
                }
                queue_b = true;
                flagged = true;
                break;
            case 's':
                if(flagged == true){
                    cout << "only one flag allowed";
                    exit(1);
                }
                stack_b = true;
                flagged = true;
                break;
            case 'o':
                output_str = optarg;
                if (!strcmp(optarg, "map") && !strcmp(optarg, "list") && !strcmp(optarg, ""))
                {
                    cout << "no output\n";
                    exit(1);
                }
                //cout << "output\n";
                break;
            default:
                cout << "default error 123\n";
                exit(1);
        }
    }
    //cout << "finished cmd line processing\n";
    if (!queue_b && !stack_b){
        cout << "No queue and no stack";
        exit(1);
    }
    return output_str;
}


bool input_fail(){
    return numcolors < 0 || numcolors > 26 || cols < 1 || rows < 1;
    exit(1);
}


vector<vector<char>> create_map(){
    cin >> numcolors >> rows >> cols;
    if(input_fail()){
        cout << "input_fail";
        exit(1);
    }
    string line;
    getline(cin, line);


    int rowcounter = 0;
    vector<vector<char>> twoDmap(rows, vector<char>(cols, '.'));
    //cout << "initialized create_map\n";


    while(getline(cin,line) && !line.empty()){//if there's something on the string
        //cout << rowcounter << "\n";
        if (line[0] == '/' && line[1] == '/'){
            continue;
        }
        //cout << line << "\n";
        for (int j = 0; j < cols; j++){
            if(validInput(line[j])){
                twoDmap[rowcounter][j] = line[j];
                //cout << line[j] << endl;
                if(twoDmap[rowcounter][j] == '@'){
                    if(start_loc != pair{-1,-1}){ //we already have a start
                        cout << "more than one start";
                        exit(1);
                    }
                    start_loc = pair{rowcounter,j};
                }
                if(twoDmap[rowcounter][j] == '?'){
                    if(end_tar != pair{-1,-1}){ //we already have an end
                        cout << "more than one end";
                        exit(1);
                    }
                    end_tar = pair{rowcounter,j};
                }
            }
            else{
                exit(1);
            }
        }
        ++rowcounter;
    }


    return twoDmap; //return the map completed
}


bool validInput(char symbol){
    //cout << "inside valid input\n";
    if( symbol == ' '){
        return false;
    }
    if (isalpha(symbol) && int(symbol) > numcolors + 96){
        cout << "wrong letter\n";
        exit(1);
    }
    if (!(symbol == '@' || symbol == '?' || symbol == '.' || symbol == '#' || symbol == '^' || isalpha(symbol))){
        cout << "invalid input\n";
        exit(1);
    }
    else {
        if (symbol == '@' && start_loc != pair{-1,-1}){
            cout << "already a start";
            exit(1);
        }
        if (symbol == '?' && end_tar != pair{-1,-1}){
            cout << "already an end";
            exit(1);
        }
        return true;
    }
    exit(1);
    return false;
}


int color_to_int(char color){
    if (color == '^'){
        return 0;
    }
    else{
        return color-96;
    }
}


bool has_seen(vector<vector<vector<char>>> &threeDmap, char color, int row, int col){
    return threeDmap[color_to_int(color)][row][col] != '-';
}


bool press_button(vector<vector<char>> &twoDmap, vector<vector<vector<char>>> &threeDmap, Position &curr){
    char char2d = twoDmap[curr.row][curr.col];
    if(islower(char2d) || char2d == '^'){ //if its actually a button
        if(curr.color != char2d){//its not on the same plane
            if(!has_seen(threeDmap, char2d, curr.row, curr.col)){//if destination not seen yet
                Position tar = {char2d, curr.row, curr.col};
                search_container.push_back(tar);
                threeDmap[color_to_int(char2d)][curr.row][curr.col] = curr.color;
                return true;
            }
            else{
                return true;
            }
        }
    }
    return false;
}


bool treat_as_tile(vector<vector<char>> &twoDmap, char color, int row, int col){
    char temp = twoDmap[row][col];
    if(isupper(temp) && toupper(color) == temp){ //open door
        return true;
    }
    switch(temp){
        case '.':
            return true;
            break;
        case '@':
            return true;
            break;
        case '?':
            return true;
            break;
        case '^':
            if(islower(temp)){
                return true;
            }
            break;
    }
    if(islower(temp) || temp == '^'){
        return true;
    }
    return false;
}

/* bool treat_as_tile(vector<vector<char>> &twoDmap, char color, int row, int col){
    char temp = twoDmap[row][col];
    if(isupper(temp) && toupper(color) == temp){ //open door
        return true;
    }
    if(temp == '.' || temp == '@' || temp == '?'){
        return true;
    }
    if(color == temp){ //same color button
        return true;
    }
    if(islower(temp) || temp == '^'){
        return true;
    }
    return false;
} */


void move_North(vector<vector<char>> &twoDmap, vector<vector<vector<char>>> &threeDmap, Position &curr){
    if(curr.row == 0){ //already on North boarder
        return;
    }
    if(threeDmap[color_to_int(curr.color)][curr.row-1][curr.col] != '-'){ //already explored
        return;
    }
    char temp = twoDmap[curr.row-1][curr.col];
    if(temp == '#'){ //if wall
        return;
    }
    else if(isupper(temp) && toupper(curr.color) != temp){ //if door but not open
        return;
    }
    else if(treat_as_tile(twoDmap, curr.color, curr.row-1, curr.col)){
        Position next_pos = {curr.color, curr.row-1, curr.col};
        threeDmap[color_to_int(curr.color)][curr.row-1][curr.col] = 'S';
        search_container.push_back(next_pos);
    }
    if(temp == '?'){
        solved_bool = true;
        end_color = curr.color;
    }
}


void move_South(vector<vector<char>> &twoDmap, vector<vector<vector<char>>> &threeDmap, Position &curr){
    if(curr.row == rows-1){ //already on South boarder
        return;
    }
    if(threeDmap[color_to_int(curr.color)][curr.row+1][curr.col] != '-'){ //already explored
        return;
    }
    char temp = twoDmap[curr.row+1][curr.col];
    if(temp == '#'){ //if wall
        return;
    }
    else if(isupper(temp) && toupper(curr.color) != temp){ //if door but not open
        return;
    }
    if(treat_as_tile(twoDmap, curr.color, curr.row+1, curr.col)){
        Position next_pos = {curr.color, curr.row+1, curr.col};
        threeDmap[color_to_int(curr.color)][curr.row+1][curr.col] = 'N';
        search_container.push_back(next_pos);
    }
    if(temp == '?'){
        solved_bool = true;
        end_color = curr.color;
    }
}


void move_West(vector<vector<char>> &twoDmap, vector<vector<vector<char>>> &threeDmap, Position &curr){
    if(curr.col == 0){ //already on West boarder
        return;
    }
    if(threeDmap[color_to_int(curr.color)][curr.row][curr.col-1] != '-'){ //already explored
        return;
    }
    char temp = twoDmap[curr.row][curr.col-1];
    if(temp == '#'){ //if wall
        return;
    }
    else if(isupper(temp) && toupper(curr.color) != temp){ //if door but not open
        return;
    }
    if(treat_as_tile(twoDmap, curr.color, curr.row, curr.col-1)){
        Position next_pos = {curr.color, curr.row, curr.col-1};
        threeDmap[color_to_int(curr.color)][curr.row][curr.col-1] = 'E';
        search_container.push_back(next_pos);
    }
    if(temp == '?'){
        solved_bool = true;
        end_color = curr.color;
    }
}


void move_East(vector<vector<char>> &twoDmap, vector<vector<vector<char>>> &threeDmap, Position &curr){
    if(curr.col == cols-1){ //already on East boarder
        return;
    }
    if(threeDmap[color_to_int(curr.color)][curr.row][curr.col+1] != '-'){ //already explored
        return;
    }
    int temp = twoDmap[curr.row][curr.col+1];
    if(temp == '#'){ //if wall
        return;
    }
    else if(isupper(temp) && toupper(curr.color) != temp){ //if door but not open
        return;
    }
    if(treat_as_tile(twoDmap, curr.color, curr.row, curr.col+1)){
        Position next_pos = {curr.color, curr.row, curr.col+1};
        threeDmap[color_to_int(curr.color)][curr.row][curr.col+1] = 'W';
        search_container.push_back(next_pos);
    }
    if(temp == '?'){
        solved_bool = true;
        end_color = curr.color;
    }
}


void solve(){    


    vector<vector<char>> twoDmap = create_map();
    
    vector<vector<vector<char>>> threeDmap(numcolors+1, vector<vector<char>>(rows, vector<char>(cols, '-')));


    Position curr = {'^', start_loc.first, start_loc.second};
    search_container.push_back(curr);
    


    threeDmap[0][start_loc.first][start_loc.second] = '@';


    search_container.front() = curr;
    Position temp;


    while (!search_container.empty() && !solved_bool){
        if (stack_b){ //stack
            temp = search_container.back();
            search_container.pop_back();
        }
        else { //queue
            temp = search_container.front();
            search_container.pop_front();
        }
        //any of these return a '?', then i can't move on
        if(press_button(twoDmap, threeDmap, temp)){
            //cout << "pressed a button \n";
            continue;
        }
        else{
            if(!solved_bool){
                move_North(twoDmap, threeDmap, temp);
            }
            if(!solved_bool){
                move_East(twoDmap, threeDmap, temp);
            }
            if(!solved_bool){
                move_South(twoDmap, threeDmap, temp);
            }
            if(!solved_bool){
                move_West(twoDmap, threeDmap, temp);
            }
        }
       
    }
    output_both(threeDmap, twoDmap, output_str);
};


char int_to_char(int i){
    if(i == 0){
        return '^';
    }
    return char(i+96);
}


bool is_button(char c){
    if(islower(c)){
        return true;
    }
    if(c=='^'){
        return true;
    }
    return false;
}

bool same_dim(char a, char b){
    return a==b;
}


void output_both(vector<vector<vector<char>>> &threeDmap, vector<vector<char>> &twoDmap, string output_str){
    if(!solved_bool){
        cout << "No solution.\nDiscovered:\n";
        for(int row=0; row < rows; row++){
            for(int col=0; col<cols; col++){
                bool pos_found = false;
                for(int curr_color = 0; curr_color<=numcolors; curr_color++){
                    if(threeDmap[curr_color][row][col] != '-'){
                        pos_found = true;
                        break;
                    }
                }
                if(pos_found == false){
                    twoDmap[row][col] = '#';
                }
            }
        }
        for(int row=0; row < rows; row++){
            for(int col=0; col<cols; col++){
                cout << twoDmap[row][col];
            }
            cout << "\n";
        }
        cout << "\n";
        return;
    }
    
    if(output_str == "list"){
        Position trace = {end_color, end_tar.first, end_tar.second};
        vector<Position> output_vec;
        while(threeDmap[color_to_int(trace.color)][trace.row][trace.col] != '@'){
            //cout << "(" << trace.color << "," << "(" << trace.row <<  "," << trace.col << "))\n";
            output_vec.push_back(trace);
            char symbol = threeDmap[color_to_int(trace.color)][trace.row][trace.col];
            switch(symbol){
                case 'N':
                    trace.row--;
                    break;
                case 'S':
                    trace.row++;
                    break;
                case 'E':
                    trace.col++;
                    break;
                case 'W':
                    trace.col--;
                    break;
                case '^':
                    trace.color = symbol;
                    break;
            }
            if(islower(symbol)){
                trace.color = symbol;
            }
        }
        
        trace.color = '^';
        trace.row = start_loc.first;
        trace.col = start_loc.second;
        output_vec.push_back(trace);
        
        for (auto it = output_vec.rbegin(); it != output_vec.rend(); it++){
            Position tempTrace = *it;
            cout << "(" << tempTrace.color << ", " << "(" << tempTrace.row <<  ", " << tempTrace.col << "))\n";
        }
    }


    else{//map
        /* for(auto i: threeDmap){
            for(auto j: i){
                for(auto k: j){
                    k = '-';
                }
            }
        } */


        Position trace = {end_color, end_tar.first, end_tar.second};
        vector<Position> output_vec;
        while(threeDmap[color_to_int(trace.color)][trace.row][trace.col] != '@'){
            //cout << "(" << trace.color << "," << "(" << trace.row <<  "," << trace.col << "))\n";
            output_vec.push_back(trace);
            char symbol = threeDmap[color_to_int(trace.color)][trace.row][trace.col];
            if(symbol == 'N'){
                trace.row--;
            }
            else if(symbol == 'S'){
                trace.row++;
            }
            else if(symbol == 'E'){
                trace.col++;
            }
            else if(symbol == 'W'){
                trace.col--;
            }
            else if(symbol == '^' || islower(symbol)){
                trace.color = symbol;
            }
        }
       
        trace.color = '^';
        trace.row = start_loc.first;
        trace.col = start_loc.second;
        output_vec.push_back(trace);
       
        for(int curr_color = 0; curr_color<=numcolors; curr_color++){
            for(int row=0; row < rows; row++){
                for(int col=0; col<cols; col++){
                    threeDmap[curr_color][row][col] = twoDmap[row][col];
                }
            }
        }
        //vector<vector<vector<char>>> output3d(numcolors+1, twoDmap);

        /* for(int colorcounter = 1; colorcounter<numcolors; colorcounter++){
            output3d[colorcounter][start_loc.first][start_loc.second] = '.';
        } */

        while(!output_vec.empty()){
            Position tempTrace = output_vec.back();
            output_vec.pop_back();
            //cout << tempTrace.color << ", " << tempTrace.row << ", " << tempTrace.col << "\n";
            //cout << twoDmap[tempTrace.row][tempTrace.col];
            char symbol = twoDmap[tempTrace.row][tempTrace.col];
            if(symbol == '.' || (isupper(symbol) && tolower(symbol) == tempTrace.color) || symbol == '@'){ //if . or open door
                threeDmap[color_to_int(tempTrace.color)][tempTrace.row][tempTrace.col] = '+';
            }
            
            if(is_button(symbol) && !same_dim(symbol, tempTrace.color)){
                threeDmap[color_to_int(tempTrace.color)][tempTrace.row][tempTrace.col] = '%';
                threeDmap[color_to_int(symbol)][tempTrace.row][tempTrace.col] = '@';
                output_vec.pop_back();
                continue; 
            }
            
            if(same_dim(symbol, tempTrace.color)){
                threeDmap[color_to_int(tempTrace.color)][tempTrace.row][tempTrace.col] = '+';
            }
        }

        for(int curr_color = 0; curr_color<=numcolors; curr_color++){
            for(int row=0; row < rows; row++){
                for(int col=0; col<cols; col++){
                    char output_char = threeDmap[curr_color][row][col];
                    switch(output_char){
                        case '+':
                            continue;
                        case '%':
                            continue;
                        case '@':
                            continue;
                        case '#':
                            continue;
                    }
                    if(isupper(output_char)){
                        if(tolower(output_char) == int_to_char(curr_color)){
                            threeDmap[curr_color][row][col] = '.';
                        }
                        else{
                            continue;
                        }
                    }
                    if(output_char == '^' && curr_color == 0){
                        threeDmap[curr_color][row][col] = '.';
                    }

                    
                }
            }
        }
        
        for(int i = 1; i<=numcolors; i++){
            if(threeDmap[i][start_loc.first][start_loc.second] == '@'){
                threeDmap[i][start_loc.first][start_loc.second] = '.';
            }
        }
        threeDmap[0][start_loc.first][start_loc.second] = '@';

        for(int curr_color = 0; curr_color<=numcolors; curr_color++){
            cout << "// color " << int_to_char(curr_color) << "\n";
            for(int row=0; row < rows; row++){
                for(int col=0; col<cols; col++){
                    if(int_to_char(curr_color) == threeDmap[curr_color][row][col]){
                        cout << '.';
                    }
                    else{
                        cout << threeDmap[curr_color][row][col];
                    }
                }
                cout << "\n";
            }
        }


        for(int curr_color = 0; curr_color<numcolors; curr_color++){
            threeDmap[color_to_int(int_to_char(curr_color))][end_tar.first][end_tar.second] = '?';
        }
    }
}





