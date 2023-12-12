//Art
//To Do
//1. Rewrite to Text User Interface (without cls)
//2. REFACTOR IT. New Architect. Need Classes, constructors, get, set...
//3. OPTIMIZE ALGOS.
//4. test bugs, look presentation mode
//5. Create Environment. (rewrite without dependencies)
//6. Create Readme. Add game decription, gif examples, howto install...
//7. think about scenario, levels...
/*
Новая концепция. 
оставить только пакман мод.
При запуске, presentation mode (можно рандом карты) и под ним команда start game
сделать лвлы. карты только с файлов. команда показать решение.
таблица топ игроков с именами.

Новый Сценарий игры 
сделать жизни
сделать еду за очки (которая со временем исчезает) 
сделать препятствия которые отнимают жизнь.
*/

//#include<bits/stdc++.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <ctime>
#include <string>
#include <algorithm>
#include <Windows.h>

// #include <chrono>
// #include <thread>
using namespace std;
 
const long long INF = 1e9;
const int coordi[4] = {-1, 0, 1, 0};
const int coordj[4] = {0, 1, 0, -1};
const char 
    ROBOT  = '0', 
    FINISH = 'x',
    WALL   = '#', 
    DELAY  = '*',
    EMPTY  = '.'; 
 
enum ConsoleColor {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    LightMagenta = 13,
    Yellow = 14,
    White = 15
};
 
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
 
const int CELL_H = 4;
const int CELL_W = 6;
const int MAX_HEIGHT = 10;
const int MAX_WIDTH = 30;
 
struct cell {
    int x, y;
};
 
cell make_cell(int x, int y) {
    cell s;
    s.x = x;
    s.y = y;
    return s;
}
 
inline void mySleep(clock_t sec) 
// clock_t is a like typedef unsigned int clock_t. Use clock_t instead of integer in this context
{
    // clock_t start_time = clock();
    // clock_t end_time = sec * 1000 + start_time;
    // while(clock() != end_time);
    // this_thread::sleep_for(chrono::milliseconds(sec*1000));
    Sleep(sec * 1000);
}
 
class Maze {
private:
    string name = "Human";
    cell start, finish;
    vector <vector <char> > beauty_map;
    bool isPackModeON = false;
    bool isAssigned = false;
    bool is_go_left = false;
    bool isNopath = false;
    long long Best_Score = -1;
    char global_prev_val = EMPTY;
    //From file
    int saved_maze_id = 1;
    int height, width;
    vector <vector <char> > maze, curr_maze_state; //map of maze
public:
    void init()
    {
        if (!isAssigned)
        {
            maze.assign(height, vector <char> (width));
            curr_maze_state.assign(height, vector <char> (width));
            beauty_map.assign(height*CELL_H, vector <char> (width*CELL_W));
            isAssigned = true;
        }
        else
        {
            maze.clear();
            curr_maze_state.clear();
            beauty_map.clear();
            maze.resize(height, vector <char> (width));
            curr_maze_state.resize(height, vector <char> (width));
            beauty_map.resize(height*CELL_H, vector <char> (width*CELL_W));
        }
        global_prev_val = EMPTY;
    }
 
    void settings()
    {
        ifstream fin("settings.txt");
        fin >> saved_maze_id;
        fin.close();
    }

    void update_settings()
    {
        ofstream fout("settings.txt");
        fout << saved_maze_id;
        fout.close();
    }
 
    void create_random_maze()
    {
        height = rand() % (MAX_HEIGHT-2)+2;
        width = rand() % (MAX_WIDTH-2)+2;
        init();
        string s = ".*#";
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                maze[i][j] = s[rand() % 3];
                curr_maze_state[i][j] = maze[i][j];
            }
 
        int stx, sty, fix, fiy;
        do
        {
            stx = rand() % height;
            sty = rand() % width;
            fix = rand() % height;
            fiy = rand() % width;
        } while (stx == fix && sty == fiy);
        maze[stx][sty] = ROBOT;
        maze[fix][fiy] = FINISH;
        curr_maze_state[stx][sty] = ROBOT;
        curr_maze_state[fix][fiy] = FINISH;
    }

    int choose_number() {
        bool gotIt = false;
        int id;
        for (int i = 0; i < 7; i++) {
            printf("Choose maze number from 1 to %d\n", saved_maze_id);
            string sid;
            cin >> sid;
            stringstream ss(sid);
            if ((ss >> id).fail()) {
                puts("Can not understand your number :(");
            } else if (id < 1) {
                puts("your number is too small. Minimal number - 1");
            } else if (id > saved_maze_id) {
                printf("your number is too big. Maximal number - %d\n", saved_maze_id);
            } else {
                gotIt = true;
                break;
            }
            puts("Try again");
        }

        if (!gotIt) {
            puts("OK. I'll choose file randomly");
            id = rand() % saved_maze_id+1; 
        }

        return id;
    }
 
    void read_maze_from_file(bool isReaden)
    {       
        int id = choose_number();
        stringstream ss;
        ss << id;
        string file_name = "map" + ss.str() + ".txt";
        string file_path = "maps/";
        string file = file_path+file_name; 
        ifstream fin(file);
        fin >> height >> width;
        init();
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) {
                fin >> maze[i][j];
                curr_maze_state[i][j] = maze[i][j];
            }

        fin.close();
    }
 
    void save_maze()
    {
        string file_path = "maps/";
        saved_maze_id++;
        stringstream ss; 
        ss << saved_maze_id;
        string file_name = "map" + ss.str() + ".txt";
        string file = file_path+file_name;
        ofstream fout(file);
        fout << height << " " << width << '\n';
        for (int i = 0; i < height; i++, fout << '\n')
            for (int j = 0; j < width; j++)
                fout << maze[i][j];
 
        fout.close();
        update_settings();
    }
 
    int edit_maze()
    {   
        int id;
        puts("Edit last seen maze? yes/no");
        string ans;
        cin >> ans;
        if (ans == "yes" || ans == "YES" || ans == "Yes") {
            save_maze();
            id = saved_maze_id;
        } else {
            puts("You can choose from old mazes one");
            id = choose_number();
        }

        stringstream ss; 
        ss << id;
        string file_path = "maps\\";
        string file_name = "map" + ss.str() + ".txt";
        string file = file_path + file_name;
        system(file.c_str());
        //edit with sublime
        //string cmd = "C:\\Program Files\\Sublime Text 3\\sublime_text.exe " + file;
        //system(cmd.c_str()); 
        system("pause");
        return id;
    }
 
    void start_maze()
    {
        srand(int(time(NULL)));
        setlocale(0, "");
        //setlocale(LC_CTYPE, "rus");
        //system("Color XY");https://www.geeksforgeeks.org/how-to-print-colored-text-in-c/
        //system("color 67"); 
        //system("mode con cols=1024 lines=720");
        //system("mode con cols=100 lines=30");
        puts("Welcome to...\n");
        mySleep(1);
        puts("   #       #                                       ");
        puts("   ##     ##      ##       ##########  ##########  ");
        puts("   # #   # #     #  #              #   ##          ");
        puts("   #  # #  #    #    #           #     ########    ");
        puts("   #   #   #   ########        #       ##          ");
        puts("   #       #  #        #     #         ##          ");
        puts("   #       # #          #  ##########  ##########  \n");
        
        mySleep(1);
        puts("Please type your name: ");
        cin >> name;
        mySleep(1);
 
        settings();
        string _switch = "on ";
        bool isReaden = false;
        string action_string;
        char action = '#';
        while (true)
        {
            if (action == '1')
            {
                create_random_maze();
                printf("%d %d\n\n", height, width);
                // isAssigned = true;
                if (isPackModeON)
                    print_beauty_map();
                else
                    print_maze();
            }
            else if (action == '2')
            {
                read_maze_from_file(isReaden);
                printf("%d %d\n\n", height, width);
                isReaden = true;
                // isAssigned = true;
                if (isPackModeON)
                    print_beauty_map();
                else
                    print_maze();
            }
            else if (action == '3')
            {
                cout << "Pacman mode is turned " << _switch << "\n\n";
                mySleep(2);
                if (!isPackModeON)
                {
                    isPackModeON = true;
                    _switch = "off";
                    if (isAssigned)
                        print_beauty_map();
                } else {
                    isPackModeON = false;
                    _switch = "on ";
                    if (isAssigned)
                        print_maze();
                }
            } 
            else if (action == '4')
            {
                puts("Presentation mode is turned on\n");
                mySleep(2);
                offline_mode();
            }
            else if (action == '5')
            {
                save_maze();
                cout << "The last maze was saved in file: 'map" << saved_maze_id << ".txt'\n";
                mySleep(2);
            } 
            else if (action == '6')
            {
                int id = edit_maze();
                cout << "The maze was edited in file: 'map" << id << ".txt'\n";
                mySleep(2);
            }
            else if (action == '7')
            {
                cout << "OK " << name << " let's play!\n\n";
                //system("pause");
                mySleep(2);
                curr_maze_state = maze;
                global_prev_val = EMPTY;
                vector <cell> path = bfs();
                if (!isNopath)
                    print_path(path, 0);
            } 
            else if (action == '8')
            {
                cout << "OK " << name << " let's play!\n\n";
                //system("pause");
                mySleep(2);
                curr_maze_state = maze;
                global_prev_val = EMPTY;
                long long Score = solve();
                cout << name << ", Your Score is: " << Score << '\n';
                if (Score < Best_Score || Best_Score == -1)
                    Best_Score = Score;

                printf("Best Score: %d\n\n", Best_Score);
            }
            else if (action == 'q')
            {
                finish_view();
                mySleep(2);
                break;
            } else if (action != '#') {
                puts("Unknown command");
            }

            puts("\nGenerate new maze      - press 1");
            puts("Read maze from file    - press 2");
            cout << "Turn " << _switch << " 'Pacman Mode' - press 3\n";
            puts("Presentation mode      - press 4");
            if (isAssigned) 
            {
                cout << "Save this maze to file - press 5  (map" << saved_maze_id+1 << ".txt)\n";
                puts("Edit maze              - press 6");
                puts("Show the solve         - press 7");
                puts("Start solving maze     - press 8");
            }
            puts("Exit                   - press q");
            puts("Force Exit - Ctrl + C");
            
            cin >> action_string;
            action = action_string[0];
            puts("");
            mySleep(1);         
        }
        //cout.flush();
        //mySleep(2);
        //sleep(2);
        //delay(2);
        //char ch = getchar();
    }
 
    void find_start_finish()
    {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++) 
            {
                if (maze[i][j] == ROBOT)
                {
                    start.x = i;
                    start.y = j;
                } 
                else if (maze[i][j] == FINISH) 
                {
                    finish.x = i;
                    finish.y = j;
                }
            }
        //cout << start.x << " " << start.y << '\n';
        //cout << finish.x << " " << finish.y << '\n';
    }
 
    vector <cell> bfs()
    { 
        find_start_finish();
        queue <cell> q;
        vector <vector <bool> > used(height, vector <bool> (width, false));
        vector <vector <cell> > p(height, vector <cell> (width));
        vector <vector <long long> > d(height, vector <long long> (width, INF));
     
        q.push(start); 
        d[start.x][start.y] = 0;
        used[start.x][start.y] = true;
        p[start.x][start.y] = make_cell(-1, -1);
     
        while (!q.empty()) 
        {
            cell current = q.front();
            q.pop();
     
            for (int t = 0; t < 4; t++) 
            {
                int i = current.x + coordi[t]; 
                int j = current.y + coordj[t];
                
                if (0 <= i && i < height &&
                    0 <= j && j < width &&
                    /*!used[i][j] &&*/ maze[i][j] != WALL)
                {
                    long long dist = d[current.x][current.y] + 1 + (maze[i][j] == DELAY);
                    if (dist < d[i][j])
                    {                      
                        used[i][j] = true;
                        d[i][j] = dist;
                        q.push(make_cell(i, j));
                        p[i][j] = current;
                    }
                }
            }
        }
 
        vector <cell> path;
        if (!used[finish.x][finish.y])
        {
            puts("No path!");
            isNopath = true;
            path.push_back(start); // fictive data 
            //system("pause");
            //exit(0);
        }
        else 
        {
            isNopath = false;
            for (cell v = finish; v.x != -1 && v.y != -1; v = p[v.x][v.y])
                path.push_back(v);
            reverse(path.begin(), path.end());
            /*
            puts("Path: ");
            for (size_t i=0; i<int(path.size()); ++i)
                cout << path[i].x + 1 << " " << path[i].y + 1 << '\n';
            puts("");
            system("pause");
            */
        }

        return path;
    }
 
    void print_path(vector <cell> path, long long iter_id)
    { 
        char prev_val = global_prev_val; // start position value without ROBOT
        cell current = start;
        for (long long it = iter_id; it < (long long)(path.size()); it++)
        {
            if (iter_id == 0)
                mySleep(1);
            system("cls");//
 
            cell prev = path[it-1*(it>0)]; //current;
            current = path[it];
            if (current.x >= 0 && current.x < height &&
                current.y >= 0 && current.y < width)
            {
                curr_maze_state[prev.x][prev.y] = prev_val;
                prev_val = curr_maze_state[current.x][current.y];
                curr_maze_state[current.x][current.y] = ROBOT;
                if (prev.y > current.y) {
                    is_go_left = true;
                } else if (prev.y < current.y) {
                    is_go_left = false;
                }
                
                //optimize_print_map(prev, current);//

                /*
                if (it + 1 < long long(path.size())) {
                    cell next_step = path[it+1];
                    if (next_step.y < current.y) {
                        is_go_left = true;
                    } else if (next_step.y > current.y) {
                        is_go_left = false;
                    }
                }  */
            } 
            else
            {
                puts("Error! You are out of maze!");
                //cerr << "Error! You are out of maze!\n";
                //system("pause");
                //exit(0);
                return;
            }

            global_prev_val = prev_val;
 
            if (isPackModeON)
                print_beauty_map();
            else
                print_maze();
        }
        puts("");
    }


 ////////////////////////////////////////////////////////
    void optimize_print_map(cell prev, cell current) {
        CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
        // GetConsoleScreenBufferInfo(h, &bufferInfo);
        COORD curr_place;
        curr_place.X = current.x;
        curr_place.Y = current.y;
        COORD prev_place;
        prev_place.X = prev.x;
        prev_place.Y = prev.y;
        // update loop
        //while (updating)
        //{
        SetConsoleCursorPosition(hConsole, curr_place);
        putchar(ROBOT);
        SetConsoleCursorPosition(hConsole, prev_place);
        putchar(global_prev_val);
            //...
            // insert combinations of sprintf, printf, etc. here
            //...
        //}
    }

    void print_maze()
    {
        for (int i = 0; i < height; i++, puts(""))
            for (int j = 0; j < width; j++)
            {
                if (curr_maze_state[i][j] == WALL)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightRed));
                else if (curr_maze_state[i][j] == DELAY)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightCyan));
                else if (curr_maze_state[i][j] == EMPTY)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightGreen));
                else
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | White));
                
                putchar(curr_maze_state[i][j]);
            }
        SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | White));
        puts("");
    }
 
    void create_empty(int i1, int j1)
    {
        int vars = 6;
        vector <vector <string> > myEmpty(vars, vector <string> (CELL_H));
        myEmpty[0][0] = "      ";
        myEmpty[0][1] = " .-.  ";
        myEmpty[0][2] = " '-'  ";
        myEmpty[0][3] = "      ";
 
        myEmpty[1][0] = "      ";
        myEmpty[1][1] = "  .-. ";
        myEmpty[1][2] = "  '-' ";
        myEmpty[1][3] = "      ";
 
        myEmpty[2][0] = "      ";
        myEmpty[2][1] = "      ";
        myEmpty[2][2] = " .-.  ";
        myEmpty[2][3] = " '-'  ";
 
        myEmpty[3][0] = "      ";
        myEmpty[3][1] = "      ";
        myEmpty[3][2] = "  .-. ";
        myEmpty[3][3] = "  '-' ";
 
        myEmpty[4][0] = " .-.  ";
        myEmpty[4][1] = " '-'  ";
        myEmpty[4][2] = "      ";
        myEmpty[4][3] = "      ";
 
        myEmpty[5][0] = "  .-. ";
        myEmpty[5][1] = "  '-' ";
        myEmpty[5][2] = "      ";
        myEmpty[5][3] = "      ";
 
        int rnd = rand() % vars;
        for (int i = 0; i < CELL_H; i++)
        {
            for (int j = 0; j < CELL_W; j++)
            {
                beauty_map[i1 * CELL_H + i][j1 * CELL_W + j] = myEmpty[rnd][i][j];
            }
        }
    }
 
    void create_delay(int i1, int j1)
    {
        int vars = 2;
        vector <vector <string> > myDelay(vars, vector <string> (CELL_H));
        myDelay[0][0] = " ____ ";
        myDelay[0][1] = "|.-. |";
        myDelay[0][2] = "|'-' |";
        myDelay[0][3] = "|____|";
 
        myDelay[1][0] = " ____ ";
        myDelay[1][1] = "| .-.|";
        myDelay[1][2] = "| '-'|";
        myDelay[1][3] = "|____|";
 
        int rnd = rand() % vars;
        for (int i = 0; i < CELL_H; i++)
        {
            for (int j = 0; j < CELL_W; j++)
            {
                beauty_map[i1 * CELL_H + i][j1 * CELL_W + j] = myDelay[rnd][i][j];
            }
        }
    }
 
    void create_wall(int i1, int j1)
    {
        vector <string> wall(CELL_H);
        wall[0] = "######";
        wall[1] = "######";
        wall[2] = "######";
        wall[3] = "######";
 
        for (int i = 0; i < CELL_H; i++)
        {
            for (int j = 0; j < CELL_W; j++)
            {
                beauty_map[i1 * CELL_H + i][j1 * CELL_W + j] = wall[i][j];
            }
        }
    }
 
    void create_finish(int i1, int j1)
    {
        vector <string> finish(CELL_H);
        finish[0] = "\\\\  //";
        finish[1] = " \\\\// ";
        finish[2] = " //\\\\ ";
        finish[3] = "//  \\\\";
 
        for (int i = 0; i < CELL_H; i++)
        {
            for (int j = 0; j < CELL_W; j++)
            {
                beauty_map[i1 * CELL_H + i][j1 * CELL_W + j] = finish[i][j];
            }
        }
    }
    void create_robot(int i1, int j1)
    {
        //4*6 + space = 5 * 7
        /*
        ,==.
        \ o ',
         \    \
         /    ;
        /   .'
        '=='   
        */
 
        vector <string> right_robot(CELL_H);
        right_robot[0] = " .--. ";
        right_robot[1] = "/ o.-'";
        right_robot[2] = "\\  '-.";
        right_robot[3] = " '--' ";
 
        vector <string> left_robot(CELL_H);
        left_robot[0] = " .--. ";
        left_robot[1] = "'-.o \\";
        left_robot[2] = ".-'  /";
        left_robot[3] = " '--' ";

        vector <string> cur_robot(CELL_H);
        if (is_go_left) {
            cur_robot = left_robot;
        } else {
            cur_robot = right_robot;
        }
 
        for (int i = 0; i < CELL_H; i++)
        {
            for (int j = 0; j < CELL_W; j++)
            {
                beauty_map[i1 * CELL_H + i][j1 * CELL_W + j] = cur_robot[i][j];
            }
        }
    }
 
    void create_beauty_map()
    {
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                if (curr_maze_state[i][j] == ROBOT)
                    create_robot(i, j);
                else if (curr_maze_state[i][j] == EMPTY)
                    create_empty(i, j);
                else if (curr_maze_state[i][j] == DELAY)
                    create_delay(i, j);
                else if (curr_maze_state[i][j] == WALL)
                    create_wall(i, j);
                else //if (curr_maze_state[i][j] == FINISH)
                    create_finish(i, j);
    }
 
    void print_beauty_map()
    {
        create_beauty_map();
        for (int i = 0; i < height * CELL_H; i++, puts(""))
            for (int j = 0; j < width * CELL_W; j++) 
            {
                if (curr_maze_state[i / CELL_H][j / CELL_W] == WALL)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightRed));
                else if (curr_maze_state[i / CELL_H][j / CELL_W] == DELAY)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightCyan));
                else if (curr_maze_state[i / CELL_H][j / CELL_W] == EMPTY)
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | LightGreen));
                else
                    SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | White));
                    
                putchar(beauty_map[i][j]);
            }
        SetConsoleTextAttribute(hConsole, (WORD) ((Black << 4) | White));
        puts("");
    }
 
    void finish_view()
    {
        puts("It's done!");
        cout << "Good bye " << name << "!\n";
    }
 
    void offline_mode() // 
    {
        while (true)
        {
            create_random_maze();
            printf("%d %d\n\n", height, width);
            //isAssigned = true;
            if (isPackModeON)
                print_beauty_map();
            else
                print_maze();
            curr_maze_state = maze;
            global_prev_val = EMPTY;
            vector <cell> path = bfs();
            if (!isNopath)
                print_path(path, 0);
            puts("Save current maze       - press s");
            puts("Exit                   - press q or CTRL+C");
            mySleep(2);
            if (GetKeyState('Q') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                break;
            } 
            else if (GetKeyState('S') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                save_maze();
            }
        }
    }
 
    long long solve()
    {
        find_start_finish();
        cell current = start;
        long long it = 0, Score = 0;
        vector <cell> path;
        path.push_back(start);
        print_path(path, it);
        puts("Exit                   - press q");
        bool problem = false;
        while (current.x != finish.x || current.y != finish.y)
        {
            cell next_step = current;
            if (GetKeyState('q') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                break;
            }
            else if(GetKeyState(VK_LEFT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                mySleep(0.5);
                next_step.y -= 1;
            }
            else if(GetKeyState(VK_RIGHT) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                mySleep(0.5);
                next_step.y += 1;
            }
            else if(GetKeyState(VK_UP) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                mySleep(0.5);
                next_step.x -= 1;
            } 
            else if(GetKeyState(VK_DOWN) & 0x8000/*Check if high-order bit is set (1 << 15)*/)
            {
                mySleep(0.5);
                next_step.x += 1;
            }
            
            if (next_step.x != current.x || next_step.y != current.y) {
                if (next_step.x >= 0 && next_step.x < height && 
                    next_step.y >= 0 && next_step.y < width &&
                    maze[next_step.x][next_step.y] != WALL) 
                {
                    current = next_step;
                    path.push_back(current);
                    it++; Score++;
                    if (maze[next_step.x][next_step.y] == DELAY)
                        Score++;

                    print_path(path, it);
                    puts("Exit                   - press q");
                }
            }
        }
        return it;
    }
};
 
int main()
{
    Maze new_maze;
    new_maze.start_maze();
    return 0;
}