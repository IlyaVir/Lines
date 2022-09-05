/*
Это был мой самый первый проект с интерфейсом на С++.
Однако это не отменяет того, что объктивно здесь много плохих решений.
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <ctime>
#include <queue>
#include <stack>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>

int table[9][9];
int distances[9][9];
unsigned long long score = 0;

void print(const int *matrix) { // for debug
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            std::cerr << *(matrix + x + 9 * y) << ' ';
        }
        std::cerr << '\n';
    }
    std::cerr << '\n';
}

void print_pairs(std::stack <std::pair <int, int>> pairs) { // for debug
    while (!pairs.empty()) {
        std::cerr << '[' << pairs.top().first << ", " << pairs.top().second << "], ";
        pairs.pop();
    }
    std::cerr << '\n';
}

void reload_distances() {
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            distances[y][x] = 1e6;
        }
    }
}

sf::CircleShape create_circle(int x, int y) {
    sf::CircleShape circle(45);
    circle.setPosition(5 + x * 100,5 + y * 100);
    circle.setOutlineThickness(2);
    circle.setOutlineColor(sf::Color::Black);
    if (table[y][x] == 1) { // Red
        circle.setFillColor(sf::Color::Red);
    } else if (table[y][x] == 2) { // Orange
        circle.setFillColor(sf::Color(255, 150, 0));
    } else if (table[y][x] == 3) { // Yellow
        circle.setFillColor(sf::Color::Yellow);
    } else if (table[y][x] == 4) { // Green
        circle.setFillColor(sf::Color::Green);
    } else if (table[y][x] == 5) { // Dark green
        circle.setFillColor(sf::Color(0, 150, 0));
    } else if (table[y][x] == 6) { // Blue
        circle.setFillColor(sf::Color::Blue);
    } else { // Light blue
        circle.setFillColor(sf::Color(99, 255, 255));
    }
    return circle;
}

std::vector <std::pair <int, int>> new_free() {
    std::vector <std::pair <int, int>> free;
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            if (table[y][x] == 0) {
                free.push_back({y, x});
            }
        }
    }
    return free;
}

void gen() {
    std::vector <std::pair <int, int>> free = new_free();
    if (free.size() > 3) {
        for (int i = 0, pos; i < 3; ++i) {
            pos = rand() % free.size();
            table[free[pos].first][free[pos].second] = rand() % 7 + 1;
            free = new_free();
        }
    } else {
        for (int i = 0; i < free.size(); ++i) {
            table[free[i].first][free[i].second] = rand() % 7 + 1;
        }
    }
}

std::stack <std::pair <int, int>> BFS(int from_y, int from_x, int to_y, int to_x) {
    reload_distances();
    std::stack <std::pair <int, int>> way;

    std::queue <std::pair <int, int>> wave, new_wave;
    wave.push({from_y, from_x});
    int distance = 0;
    distances[from_y][from_x] = 0;

    std::pair <int, int> point;
    while (!wave.empty() && distances[to_y][to_x] == 1e6) {
        ++distance;
        new_wave = {};
        while (!wave.empty()) {
            point = wave.front();
            wave.pop();
            if (point.first + 1 <= 8 &&
            distances[point.first + 1][point.second] == 1e6 && table[point.first + 1][point.second] == 0) {
                distances[point.first + 1][point.second] = distance;
                new_wave.push({point.first + 1, point.second});
            }
            if (point.second + 1 <= 8 &&
            distances[point.first][point.second + 1] == 1e6 && table[point.first][point.second + 1] == 0) {
                distances[point.first][point.second + 1] = distance;
                new_wave.push({point.first, point.second + 1});
            }
            if (point.first - 1 >= 0 &&
            distances[point.first - 1][point.second] == 1e6 && table[point.first - 1][point.second] == 0) {
                distances[point.first - 1][point.second] = distance;
                new_wave.push({point.first - 1, point.second});
            }
            if (point.second - 1 >= 0 &&
            distances[point.first][point.second - 1] == 1e6 && table[point.first][point.second - 1] == 0) {
                distances[point.first][point.second - 1] = distance;
                new_wave.push({point.first, point.second - 1});
            }
        }
        std::swap(wave, new_wave);
    }

    if (distances[to_y][to_x] == 1e6) {
        //std::cout << "impossible\n";
    } else {
        way.push({to_y, to_x});
        point = {to_y, to_x};
        while (distance > 0) {
            --distance;
            if (point.first + 1 <= 8 && distances[point.first + 1][point.second] == distance) {
                point = {point.first + 1, point.second};
            } else if (point.second + 1 <= 8 && distances[point.first][point.second + 1] == distance) {
                point = {point.first, point.second + 1};
            } else if (point.first - 1 >= 0 && distances[point.first - 1][point.second] == distance) {
                point = {point.first - 1, point.second};
            } else if (point.second - 1 >= 0 && distances[point.first][point.second - 1] == distance) {
                point = {point.first, point.second - 1};
            }
            way.push(point);
        }
    }
    return way;
}

bool destroy(int same) {
    std::vector <std::pair <int, int>> to_destroy;
    unsigned long long add_score = 0, combo = 0;

    int color, sames;
    // check horizontal
    for (int y = 0; y < 9; ++y) {
        sames = color = 0;
        for (int x = 0; x < 9; ++x) {
            if (color == table[y][x] && table[y][x]) {
                ++sames;
            } else {
                if (sames >= same) {
                    for (int now = x - 1; now >= x - sames; --now) {
                        to_destroy.push_back({y, now});
                    }
                    ++combo;
                    add_score += sames;
                }
                color = table[y][x];
                sames = 1;
            }
        }

        if (sames >= same) {
            for (int now = 8; now >= 8 - sames; --now) {
                to_destroy.push_back({y, now});
            }
            ++combo;
            add_score += sames;
        }
    }
    // check vertical
    for (int x = 0; x < 9; ++x) {
        sames = color = 0;
        for (int y = 0; y < 9; ++y) {
            if (color == table[y][x] && table[y][x]) {
                ++sames;
            } else {
                if (sames >= same) {
                    for (int now = y - 1; now >= y - sames; --now) {
                        to_destroy.push_back({now, x});
                    }
                    ++combo;
                    add_score += sames;
                }
                color = table[y][x];
                sames = 1;
            }
        }

        if (sames >= same) {
            for (int now = 8; now > 8 - sames; --now) {
                to_destroy.push_back({now, x});
            }
            ++combo;
            add_score += sames;
        }
    }
    //check left diagonals (/)
    int start_x = 0, start_y = 0, x, y;
    while (start_x < 8 || start_y < 8) {
        sames = color = 0;
        x = start_x;
        y = start_y;
        while (y <= start_x) {
            if (color == table[y][x] && table[y][x]) {
                ++sames;
            } else {
                if (sames >= same) {
                    for (int now_x = x + 1, now_y = y - 1; now_y >= y - sames; ++now_x, --now_y) {
                        to_destroy.push_back({now_y, now_x});
                    }
                    ++combo;
                    add_score += sames;
                }
                color = table[y][x];
                sames = 1;
            }
            --x;
            ++y;
        }
        if (sames >= same) {
            for (int now_x = x, now_y = y; now_y >= y - sames; ++now_x, --now_y) {
                to_destroy.push_back({now_y, now_x});
            }
            ++combo;
            add_score += sames;
        }
        if (start_x < 8) {
            ++start_x;
        } else {
            ++start_y;
        }
    }
    //check right diagonals (\)
    start_x = 7, start_y = 0;
    while (start_x > 0 || start_y < 7) {
        sames = color = 0;
        x = start_x;
        y = start_y;
        while (y <= 8 - start_x) {
            if (color == table[y][x] && table[y][x]) {
                ++sames;
            } else {
                if (sames >= same) {
                    for (int now_x = x - 1, now_y = y - 1; now_y >= y - sames; --now_x, --now_y) {
                        to_destroy.push_back({now_y, now_x});
                    }
                    ++combo;
                    add_score += sames;
                }
                color = table[y][x];
                sames = 1;
            }
            ++x;
            ++y;
        }
        if (sames >= same) {
            for (int now_x = x, now_y = y; now_y >= y - sames; --now_x, --now_y) {
                to_destroy.push_back({now_y, now_x});
            }
            ++combo;
            add_score += sames;
        }
        if (start_x > 0) {
            --start_x;
        } else {
            ++start_y;
        }
    }

    bool ans = true;
    if (to_destroy.empty()) {
        ans = false;
    }

    while (!to_destroy.empty()) {
        table[to_destroy.back().first][to_destroy.back().second] = 0;
        to_destroy.pop_back();
    }

    score += (add_score * combo);
    std::cerr << score << '\n';
    return ans;
}

bool check_lose() {
    bool lose = true;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (table[i][j] == 0) {
                lose = false;
                break;
            }
        }
    }
    return lose;
}

int main()
{
    srand(std::time(0));
    for (int y = 0; y < 9; ++y) {
        for (int x = 0; x < 9; ++x) {
            table[y][x] = 0;
        }
    }

    // creation of grid
    sf::VertexArray grid(sf::Lines, 32);
    for (int i = 0, pos = 100; i < 16; i += 2, pos += 100) {
        grid[i].position = sf::Vector2f(pos, 0);
        grid[i].color = sf::Color::Black;
        grid[i + 1].position = sf::Vector2f(pos, 900);
        grid[i + 1].color = sf::Color::Black;
        grid[i + 16].position = sf::Vector2f(0, pos);
        grid[i + 16].color = sf::Color::Black;
        grid[i + 17].position = sf::Vector2f(900, pos);
        grid[i + 17].color = sf::Color::Black;
    }

    int select_x = -1, select_y = -1;
    sf::VertexArray select(sf::Quads, 4);
    std::stack <std::pair <int, int>> way;

    int same;
    std::cout << "Settings:\nSame[2; 9] = ";
    std::string s;
    std::getline(std::cin, s);
    while (s.size() != 1 || s[0] - '0' < 2 || s[0] - '0' > 9) {
        std::cout << "Wrong\nSame[2; 9] = ";
        std::getline(std::cin, s);
    }
    same = s[0] - '0';
    sf::RenderWindow game(sf::VideoMode(900, 900), "Lines");

    gen();

    sf::Music music;
    music.openFromFile("/home/ilya/CLionProjects/Lines/Music/inthestreets.ogg");
    music.play();
    music.setVolume(49);
    music.setLoop(true);

    sf::SoundBuffer buff;
    sf::Sound sound;

    while (game.isOpen()) {
        sf::Event xxx;
        while (game.pollEvent(xxx)) {
            if (xxx.type == sf::Event::Closed) {
                game.close();
            }
        }

        game.clear(sf::Color::White);
        if (way.empty() && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            int x = sf::Mouse::getPosition(game).x,
            y = sf::Mouse::getPosition(game).y;
            buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/impossible_move.wav");
            sound.setBuffer(buff);
            if (x > 0 && y > 0) {
                x /= 100;
                y /= 100;
                if (select_x == -1 && table[y][x]) {
                    select_x = x;
                    select_y = y;

                    select[0].position = sf::Vector2f(x * 100, y * 100);
                    select[1].position = sf::Vector2f(x * 100 + 100, y * 100);
                    select[2].position = sf::Vector2f(x * 100 + 100, y * 100 + 100);
                    select[3].position = sf::Vector2f(x * 100, y * 100 + 100);
                    for (int i = 0; i < 4; ++i) {
                        select[i].color = sf::Color(175, 175, 175, 200);
                    }
                    game.draw(select);
                    buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/select.wav");
                    sound.setBuffer(buff);
                } else if (select_x != -1) {
                    if (table[y][x] == 0) {
                        way = BFS(select_y, select_x, y, x);
                        if (!way.empty()) {
                            buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/give_army_move_order.wav");
                            sound.setBuffer(buff);
                        }
                    }
                    select_x = -1;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(180));
                sound.play();
            }
        } else if (way.empty() && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            select_x = -1;
        }

        if (select_x != -1) {
            game.draw(select);
        }
        if (way.size() > 1) {
            int from_y = way.top().first, from_x = way.top().second;
            way.pop();
            table[way.top().first][way.top().second] = table[from_y][from_x];
            table[from_y][from_x] = 0;
        } else if (way.size() == 1) {
            way = {};
            if (!destroy(same)) {
                gen();
                if (destroy(same)) {
                    buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/same.wav");
                    sound.setBuffer(buff);
                    sound.play();
                } else if (check_lose()) {
                    for (int y = 0; y < 9; ++y) {
                        for (int x = 0; x < 9; ++x) {
                            if (table[y][x]) {
                                game.draw(create_circle(x, y));
                            }
                        }
                    }

                    game.draw(grid);
                    game.display();

                    buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/lose.wav");
                    sound.setBuffer(buff);
                    music.stop();
                    sound.play();
                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    game.close();
                }
            } else {
                buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/same.wav");
                sound.setBuffer(buff);
                sound.play();
            }
        }
        for (int y = 0; y < 9; ++y) {
            for (int x = 0; x < 9; ++x) {
                if (table[y][x]) {
                    game.draw(create_circle(x, y));
                }
            }
        }

        game.draw(grid);
        game.display();
        if (!way.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
        }
    }

    music.stop();

    struct excel {
        int score;
        std::string name;
    };

    std::ifstream fin;
    fin.open("/home/ilya/CLionProjects/Lines/res");

    int people;
    fin >> people;
    excel res[people + 1];
    for (int i = 0; i < people; ++i) {
        fin >> res[i].name >> res[i].score;
    }
    fin.close();

    /*
    sf::Text text("xxx", 20);
    sf::RenderWindow chart(sf::VideoMode(900, 900), "Chart");
    while (chart.isOpen()) {
        sf::Event xxx;
        while (chart.pollEvent(xxx)) {
            if (xxx.type == sf::Event::Closed) {
                chart.close();
            }
        }

        chart.clear(sf::Color::White);
        chart.display();
    }
    */
    buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/Your_name.wav");
    sound.setBuffer(buff);
    sound.play();

    std::string name;
    std::cout << "Введите своё имя (без пробелов): ";
    std::cin >> name;
    res[people].name = name;
    res[people].score = score;
    for (int i = people - 1; i >= 0 && res[i].score < score; --i) {
        std::swap(res[i], res[i + 1]);
    }

    std::ofstream fout;
    fout.open("/home/ilya/CLionProjects/Lines/res");
    fout << people + 1;
    std::cout << "\nТаблица лидеров (топ 5)";
    for (int i = 0; i < people + 1; ++i) {
        if (i < 5) {
            std::cout << '\n' << res[i].name << ' ' << res[i].score;
        }
        fout << '\n' << res[i].name << ' ' << res[i].score;
    }
    std::cout.flush();

    buff.loadFromFile("/home/ilya/CLionProjects/Lines/SoundFX/res.wav");
    sound.setBuffer(buff);
    sound.play();
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    fout.close();

    return 0;
}
