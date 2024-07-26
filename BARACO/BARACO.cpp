#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

using namespace std;

string ref_no = "";
string date_time = "";
string cond_name = "";
string orig_dest = "";
int orig_pt = 0;
int dest_pt = 0;
double baraco_tix = 0;
double baraco_disc = 0;
double baraco_tix_disc = 0;

map<string, string> baraco_stations = {
    {"1", "Batangas City"},
    {"2", "Ibaan"},
    {"3", "Rosario"},
    {"4", "San Juan"},
    {"5", "Lobo"}
};

map<string, double> routes_km = {
    {"1-2", 16.3}, {"1-3", 23.6}, {"1-4", 35.3}, {"1-5", 39.9},
    {"2-1", 16.3}, {"2-3", 14.5}, {"2-4", 23.5}, {"2-5", 42.6},
    {"3-1", 23.6}, {"3-2", 14.5}, {"3-4", 27.7}, {"3-5", 41.8},
    {"4-1", 35.3}, {"4-2", 23.5}, {"4-3", 27.7}, {"4-5", 38.6},
    {"5-1", 39.9}, {"5-2", 42.6}, {"5-3", 41.8}, {"5-4", 38.6}
};

map<string, map<string, string>> baraco_rcp;
vector<double> total_routes;
vector<double> total_sales;

string format_currency(float amount);
void print_receipt(const string& ref_no, const map<string, string>& receipt);
void create_src();
string generate_unique_id(unordered_set<string>& existing_ids);
string trim(const string& str);
void card_gen();
void load_users();
void save_users(const string& username, const string& password, const string& id_number);
void save_username(const string& username);
void save_password(const string& password);
void save_coins(const string& username, float coins);
void activate_card();
void open_card();
void print_centered(const string& text, int width);
void user_menu(const string& username);
void view_stations(const string& username);
void topup_card(const string& username);
string generate_ref_no();
string get_current_datetime();
void select_route(const string& username);
void sign_out();
void main_menu();
void view_favorite(const string& username);
void add_favorites(const string& username);
void remove_favorites(const string& username);

unordered_set<string> existing_usernames;
unordered_set<string> existing_passwords;
unordered_set<string> existing_ids;

unordered_map<string, vector<string>> user_favorites;

string format_currency(float amount) {
    stringstream ss;
    ss << fixed << setprecision(2) << amount;
    return ss.str();
}

void print_centered(const string& text, int width) {
    int padding = (width - text.size()) / 2;
    cout << string(padding, ' ') << text << endl;
}

void create_src() {
    std::filesystem::create_directory("src");
    ofstream card_file("src/card_numbers.txt");
    if (card_file.is_open()) {
        card_file.close();
    }
    else {
        cerr << "\t>> Unable to 'create card_numbers.txt'." << endl;
    }
}

string generate_unique_id(unordered_set<string>& existing_ids) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 9);

    string id;
    do {
        id = "";
        for (int i = 0; i < 10; ++i) {
            id += to_string(dis(gen));
        }
    } while (existing_ids.find(id) != existing_ids.end());

    existing_ids.insert(id);
    return id;
}

string trim(const string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

void card_gen() {
    ifstream card_file_in("src/card_numbers.txt");
    string id;

    while (card_file_in >> id) {
        existing_ids.insert(id);
    }
    card_file_in.close();

    ofstream card_file_out("src/card_numbers.txt", ios::app);
    for (int i = 0; i < 10; ++i) {
        card_file_out << generate_unique_id(existing_ids) << endl;
    }
    card_file_out.close();
}

void load_users() {
    ifstream users_file("src/users.txt");
    string line;

    if (users_file.is_open()) {
        while (getline(users_file, line)) {
            size_t pos = 0;
            while ((pos = line.find(", ", pos)) != string::npos) {
                line.replace(pos, 2, ",");
                pos += 1;
            }

            istringstream iss(line);
            string temp;

            getline(iss, temp, ',');
            string username = trim(temp.substr(temp.find(":") + 1));

            getline(iss, temp, ',');
            string password = trim(temp.substr(temp.find(":") + 1));

            getline(iss, temp, ',');
            string id_number = trim(temp.substr(temp.find(":") + 1));

            getline(iss, temp);
            string coins_str = trim(temp.substr(temp.find(":") + 1));

            float coins = 0.0f;
            try {
                coins = stof(coins_str);
            }
            catch (const std::invalid_argument&) {
                cerr << "\t>> Invalid coins value: " << coins_str << endl;
                continue;
            }

            existing_usernames.insert(username);
            existing_passwords.insert(password);
            existing_ids.insert(id_number);
        }
        users_file.close();
    }
    else {
        cerr << "\t>> Unable to open 'users.txt'." << endl;
    }
}

void save_users(const string& username, const string& password, const string& id_number) {
    ofstream users_file("src/users.txt", ios::app);
    if (users_file.is_open()) {
        users_file << "Username: " << username
            << ", Password: " << password
            << ", ID Number: " << id_number
            << ", Coins: 0.00" << endl;
        users_file.close();
    }
    else {
        cerr << "\t>> Unable to open 'users.txt'." << endl;
    }
}

void save_username(const string& username) {
    existing_usernames.insert(username);
}

void save_password(const string& password) {
    existing_passwords.insert(password);
}

void save_coins(const string& username, float coins) {
    ifstream users_file_in("src/users.txt");
    vector<string> users_data;
    string line;

    while (getline(users_file_in, line)) {
        if (line.find("Username: " + username) != string::npos) {
            size_t pos = line.find("Coins: ");
            size_t end_pos = line.find(",", pos);
            if (pos != string::npos) {
                if (end_pos == string::npos) {
                    end_pos = line.length();
                }
                string coins_str = line.substr(pos + 6, end_pos - (pos + 6));
                float current_coins = 0.0f;
                try {
                    current_coins = stof(coins_str);
                }
                catch (const std::invalid_argument& e) {
                    cerr << "\t>> Invalid current coins value: " << coins_str << endl;
                    current_coins = 0.0f;
                }
                current_coins -= coins;
                stringstream coins_ss;
                coins_ss << fixed << setprecision(2) << current_coins;
                line.replace(pos + 6, end_pos - (pos + 6), " " + coins_ss.str());
            }
        }
        users_data.push_back(line);
    }
    users_file_in.close();

    ofstream users_file_out("src/users.txt");
    for (const string& data : users_data) {
        users_file_out << data << endl;
    }
    users_file_out.close();
}

void activate_card() {
    string id_number, username, password;

    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Activate BARACO Card", 100);
    cout << endl;

    cout << "> Enter your ID number: ";
    cin >> id_number;

    ifstream card_file_in("src/card_numbers.txt");
    if (!card_file_in.is_open()) {
        cerr << "\t>> Unable to open 'card_numbers.txt'." << endl;
        return;
    }

    vector<string> card_numbers;
    string card_number;
    bool id_found = false;

    while (getline(card_file_in, card_number)) {
        if (card_number == id_number) {
            id_found = true;
        }
        else {
            card_numbers.push_back(card_number);
        }
    }
    card_file_in.close();

    if (!id_found) {
        cerr << "\t>> ID Number does not exist. Please try again." << endl;
        return;
    }

    cout << "> Enter your username (at least 4 characters): ";
    cin >> username;
    if (username.length() < 4) {
        cerr << "\t>> Username must be at least 4 characters. Please try again." << endl;
        return;
    }

    if (existing_usernames.find(username) != existing_usernames.end()) {
        cerr << "\t>> Username already exists. Please try a new username." << endl;
        return;
    }

    cout << "> Enter your password (at least 8 characters): ";
    cin >> password;
    if (password.length() < 8) {
        cerr << "\t>> Password must be at least 8 characters. Please try again." << endl;
        return;
    }

    save_username(username);
    save_password(password);

    ofstream users_file("src/users.txt", ios::app);
    if (users_file.is_open()) {
        users_file << "ID Number: " << id_number
            << ", Username: " << username
            << ", Password: " << password
            << ", Coins: 0" << endl;
        users_file.close();
    }
    else {
        cerr << "\t>> Unable to open 'users.txt'." << endl;
    }

    ofstream card_file_out("src/card_numbers.txt");
    if (card_file_out.is_open()) {
        for (const string& card : card_numbers) {
            card_file_out << card << endl;
        }
        card_file_out.close();
    }
    else {
        cerr << "\t>> Unable to open 'card_numbers.txt'." << endl;
    }

    cout << "\t>> Card activated successfully!" << endl;
    main_menu();
}

void open_card() {
    string username, password;

    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Open BARACO Card", 100);
    cout << endl;

    cout << "> Enter your username: ";
    cin >> username;
    cout << "> Enter your password: ";
    cin >> password;

    ifstream users_file("src/users.txt");
    string line;
    bool user_found = false;

    while (getline(users_file, line)) {
        if (line.find("Username: " + username) != string::npos &&
            line.find("Password: " + password) != string::npos) {
            user_found = true;
            break;
        }
    }
    users_file.close();

    if (!user_found) {
        cerr << "\t>> Invalid username or password. Please try again." << endl;
        return;
    }

    user_menu(username);
}

void user_menu(const string& username) {
    ifstream users_file("src/users.txt");
    string line;
    float user_coins = 0.0f;
    string user_id;

    while (getline(users_file, line)) {
        if (line.find("Username: " + username) != string::npos) {
            size_t id_pos = line.find("ID Number: ");
            size_t id_end_pos = line.find(",", id_pos);
            if (id_pos != string::npos) {
                user_id = line.substr(id_pos + 11, id_end_pos - (id_pos + 11));
            }

            size_t coins_pos = line.find("Coins: ");
            size_t coins_end_pos = line.find(",", coins_pos);
            if (coins_pos != string::npos) {
                if (coins_end_pos == string::npos) {
                    coins_end_pos = line.length();
                }
                string coins_str = line.substr(coins_pos + 6, coins_end_pos - (coins_pos + 6));
                try {
                    user_coins = stof(coins_str);
                }
                catch (const std::invalid_argument& e) {
                    cerr << "\t>> Invalid coins value: " << coins_str << endl;
                    user_coins = 0.0f;
                }
            }
            break;
        }
    }
    users_file.close();

    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("   __      __      __ __  ", 100);
    print_centered("  |__) /\\ |__) /\\ /  /  \\ ", 100);
    print_centered("  |__)/--\\| \\ /--\\\\__\\__/ ", 100);
    print_centered("                          ", 100);
    print_centered("Batangas Railway Corporation", 100);
    print_centered("User Menu", 100);
    cout << endl;

    cout << "Username: " << username << endl;
    cout << "ID Number: " << user_id << endl;
    cout << "Coins: Php " << fixed << setprecision(2) << user_coins << endl << endl;

    vector<string> menu_options = {
        "View Stations",
        "Select Route",
        "View Favorite Routes",
        "Top Up Card",
        "Sign Out"
    };

    for (size_t i = 0; i < menu_options.size(); ++i) {
        cout << i + 1 << ". " << menu_options[i] << "\n";
    }

    while (true) {
        cout << "\n> Kindly select an option: ";
        int choice;
        while (!(cin >> choice)) {
            cout << "\t>> Invalid input. Please enter an integer: ";
            cin.clear();
            cin.ignore(1000000, '\n');
        }
        switch (choice) {
        case 1:
            view_stations(username);
            break;
        case 2:
            select_route(username);
            break;
        case 3:
            view_favorite(username);
            break;
        case 4:
            topup_card(username);
            break;
        case 5:
            sign_out();
            return;
        default:
            cout << "\t>> Invalid input. Please try again.\n";
            user_menu(username);
        }
    }
}

void view_favorite(const string& username) {
    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Favorite BARACO Routes", 100);
    cout << endl;

    auto it = user_favorites.find(username);
    if (it != user_favorites.end() && !it->second.empty()) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            cout << i + 1 << ". " << it->second[i] << " - " 
                 << baraco_stations[it->second[i].substr(0, 1)] << " to " 
                 << baraco_stations[it->second[i].substr(2, 1)] << "\n";
        }
    } else {
        cout << "No favorite routes found.\n";
    }

    cout << "\n[A] Add Route to Favorites\n";
    cout << "[R] Remove Route from Favorites\n";
    cout << "[X] Return\n";
    cout << "\n> Enter your choice: ";

    string choice;
    getline(cin >> ws, choice);

    if (choice.empty()) {
        user_menu(username);
        return;
    }

    switch (choice[0]) {
        case 'A':
        case 'a':
            add_favorites(username);
            break;
        case 'R':
        case 'r':
            remove_favorites(username);
            break;
        case 'X':
        case 'x':
            user_menu(username);
            break;
        default:
            cout << "\t>> Invalid input. Please try again.\n";
            view_favorite(username);
    }
}

void add_favorites(const string& username) {
    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Add Route to Favorites", 100);
    cout << endl;

    cout << "Available Routes:\n\n";
    for (const auto& route : routes_km) {
        cout << route.first << " - " 
             << baraco_stations[route.first.substr(0, 1)] << " to " 
             << baraco_stations[route.first.substr(2, 1)] << "\n";
    }
    cout << "\n> Enter the route you want to add (e.g., 1-2): ";
    string new_route;
    getline(cin >> ws, new_route);

    if (new_route.empty()) {
        user_menu(username);
        return;
    }

    auto it = routes_km.find(new_route);
    if (it != routes_km.end()) {
        if (find(user_favorites[username].begin(), user_favorites[username].end(), new_route) != user_favorites[username].end()) {
            cout << "\t>> Route has already been added to favorites.\n";
        } else {
            user_favorites[username].push_back(new_route);
            cout << "\t>> Route added to favorites.\n";
        }
    } else {
        cout << "\t>> Invalid route. Please try again.\n";
    }

    view_favorite(username);
}

void remove_favorites(const string& username) {
    cout << string(100, '-') << endl << endl;
    print_centered("Remove Route from Favorites", 100);
    cout << endl;

    auto it = user_favorites.find(username);
    if (it != user_favorites.end() && !it->second.empty()) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            cout << i + 1 << ". " << it->second[i] << " - " 
                 << baraco_stations[it->second[i].substr(0, 1)] << " to " 
                 << baraco_stations[it->second[i].substr(2, 1)] << "\n";
        }
        cout << "\n> Enter the number of the route that you want to remove: ";
        string input;
        getline(cin >> ws, input);

        if (input.empty()) {
            user_menu(username);
            return;
        }

        int route_num;
        try {
            route_num = stoi(input);
        } catch (const invalid_argument&) {
            cout << "\t>>Invalid input. Please try again.\n";
            remove_favorites(username);
            return;
        }

        if (route_num > 0 && route_num <= static_cast<int>(it->second.size())) {
            it->second.erase(it->second.begin() + (route_num - 1));
            cout << "\t>> Route removed from favorites.\n";
        } else {
            cout << "\t>> Invalid number. Please try again.\n";
        }
    } else {
        cout << "\nNo routes to remove.\n";
    }

    view_favorite(username);
}

void view_stations(const string& username) {
    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("View BARACO Stations", 100);
    cout << endl;

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    while (true) {
        cout << "\n> Would you like to go back to the user menu? (Y/N): ";
        string choice;
        cin >> choice;
        if (choice == "Y" || choice == "y") {
            user_menu(username);
            return;
        }
        else if (choice == "N" || choice == "n") {
            return;
        }
        else {
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}

void topup_card(const string& username) {
    float coins_to_add;

    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Top-up BARACO Card", 100);
    cout << endl;

    cout << "> Enter the amount you want to add (Php): ";
    cin >> coins_to_add;

    if (coins_to_add <= 0) {
        cerr << "\t>> Invalid amount. Please enter a positive number." << endl;
        return;
    }

    ifstream users_file_in("src/users.txt");
    vector<string> users_data;
    string line;
    float current_coins = 0.0f;

    while (getline(users_file_in, line)) {
        if (line.find("Username: " + username) != string::npos) {
            size_t pos = line.find("Coins: ");
            size_t end_pos = line.find(",", pos);

            if (pos != string::npos) {
                if (end_pos == string::npos) {
                    end_pos = line.length();
                }
                string coins_str = line.substr(pos + 6, end_pos - (pos + 6));
                try {
                    current_coins = stof(coins_str);
                }
                catch (const std::invalid_argument& e) {
                    cerr << "\t>> Invalid current coins value: " << coins_str << endl;
                    current_coins = 0.0f;
                }
                current_coins += coins_to_add;
                stringstream coins_ss;
                coins_ss << fixed << setprecision(2) << current_coins;
                line.replace(pos + 6, end_pos - (pos + 6), " " + coins_ss.str());
            }
        }
        users_data.push_back(line);
    }
    users_file_in.close();

    ofstream users_file_out("src/users.txt");
    for (const string& data : users_data) {
        users_file_out << data << endl;
    }
    users_file_out.close();

    cout << "\t>> Top-up successful! Your new coin balance is updated." << endl;
    user_menu(username);
}

string generate_ref_no() {
    string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, chars.size() - 1);

    string ref_no;
    for (int i = 0; i < 10; ++i) {
        ref_no += chars[distribution(generator)];
    }
    return ref_no;
}

string get_current_datetime() {
    time_t now = time(0);
    tm ltm;
#ifdef _WIN32
    localtime_s(&ltm, &now);
#else
    localtime_r(&now, &ltm);
#endif

    char buf[80];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &ltm);
    return string(buf);
}

void print_receipt(const string& ref_no, const map<string, string>& receipt) {
    float rcp_tix = stof(receipt.at("rcp_tix"));
    float rcp_disc = stof(receipt.at("rcp_disc"));
    float rcp_tix_disc = stof(receipt.at("rcp_tix_disc"));

    print_centered(string(50, '-'), 100);
    print_centered(string(50, '-'), 100);
    cout << endl;
    print_centered("   __      __      __ __  ", 100);
    print_centered("  |__) /\\ |__) /\\ /  /  \\ ", 100);
    print_centered("  |__)/--\\| \\ /--\\\\__\\__/ ", 100);
    print_centered("                          ", 100);
    print_centered("Official Receipt", 100);
    cout << endl;

    print_centered("Reference No.: " + ref_no, 100);
    print_centered("Date and Time: " + receipt.at("rcp_datetime"), 100);
    print_centered("From: " + baraco_stations.at(receipt.at("rcp_orig")), 100);
    print_centered("To: " + baraco_stations.at(receipt.at("rcp_dest")), 100);
    print_centered("Regular: Php " + format_currency(rcp_tix), 100);
    print_centered("Discount: Php " + format_currency(rcp_disc), 100);
    print_centered("Total Amount: Php " + format_currency(rcp_tix_disc), 100);
    cout << endl;
    print_centered(string(50, '-'), 100);
    print_centered(string(50, '-'), 100);
    cout << endl;
}

void select_route(const string& username) {
    baraco_tix = 0;
    baraco_disc = 0;
    baraco_tix_disc = 0;

    ifstream users_file("src/users.txt");
    string line;
    float user_coins = 0.0f;

    while (getline(users_file, line)) {
        if (line.find("Username: " + username) != string::npos) {
            size_t coins_pos = line.find("Coins: ");
            size_t coins_end_pos = line.find(",", coins_pos);
            if (coins_pos != string::npos) {
                if (coins_end_pos == string::npos) {
                    coins_end_pos = line.length();
                }
                string coins_str = line.substr(coins_pos + 7, coins_end_pos - (coins_pos + 7)); // Adjusted index
                try {
                    user_coins = stof(coins_str);
                }
                catch (const std::invalid_argument& e) {
                    cerr << "\t>> Invalid coins value: " << coins_str << endl;
                    user_coins = 0.0f;
                }
            }
            break;
        }
    }
    users_file.close();

    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("Select BARACO Route", 100);
    cout << endl;

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    int orig_pt = -1;
    int dest_pt = -1;
    string input;

    while (orig_pt < 1 || orig_pt >= i) {
        cout << endl << "> Kindly select the numerical input corresponding to your origin point (1 to " << i - 1 << ") ";
        cout << "Press 'X' to return: ";
        cin >> input;
        if (input == "X" || input == "x") {
            user_menu(username);
            return;
        }
        try {
            orig_pt = stoi(input);
        } catch (const std::invalid_argument& e) {
            orig_pt = -1;
        }
        if (orig_pt < 1 || orig_pt >= i) {
            cout << "Invalid input. Please enter a number between 1 and " << i - 1 << ": ";
        }
    }

    while (dest_pt < 1 || dest_pt >= i || dest_pt == orig_pt) {
        cout << "> Kindly select the numerical input corresponding to your destination point (1 to " << i - 1 << "): ";
        cin >> input;
        try {
            dest_pt = stoi(input);
        } catch (const std::invalid_argument& e) {
            dest_pt = -1;
        }
        if (dest_pt < 1 || dest_pt >= i || dest_pt == orig_pt) {
            if (dest_pt == orig_pt) {
                cout << "Destination point cannot be the same as origin point. ";
            }
            cout << "Invalid input. Please enter a number between 1 and " << i - 1 << ": ";
        }
    }

    orig_dest = to_string(orig_pt) + "-" + to_string(dest_pt);

    if (routes_km.find(orig_dest) != routes_km.end()) {
        baraco_tix = 5 * abs(routes_km[orig_dest] - 7);
        baraco_tix = static_cast<int>(baraco_tix) + (baraco_tix - static_cast<int>(baraco_tix) >= 0.5 ? 1 : 0);
        total_routes.push_back(routes_km[orig_dest]);

        cout << "\t>> The calculated distance from " << baraco_stations[to_string(orig_pt)] << " to " << baraco_stations[to_string(dest_pt)] << " is " << routes_km[orig_dest] << " kilometers.\n";

        while (true) {
            cout << "> Are you a senior citizen, PWD, or a student? (Y/N): ";
            string elig_disc;
            cin >> elig_disc;

            if (elig_disc == "Y" || elig_disc == "y") {
                baraco_disc = baraco_tix * 0.2;
                baraco_tix_disc = baraco_tix - baraco_disc;

                if (user_coins < baraco_tix_disc) {
                    cout << "\t>> Insufficient coins. Please top up your card.\n";
                    user_menu(username);
                    return;
                }

                total_sales.push_back(baraco_tix_disc);
                cout << "\t>> Thank you for riding with BARACO. Your total ticketing fare with the 20% discount applied is Php " << fixed << setprecision(2) << baraco_tix_disc << ".\n\n";

                string ref_no = generate_ref_no();
                string date_time = get_current_datetime();

                map<string, string> receipt = {
                    {"rcp_datetime", date_time},
                    {"rcp_cond", cond_name},
                    {"rcp_orig", to_string(orig_pt)},
                    {"rcp_dest", to_string(dest_pt)},
                    {"rcp_tix", to_string(baraco_tix)},
                    {"rcp_disc", to_string(baraco_disc)},
                    {"rcp_tix_disc", to_string(baraco_tix_disc)}
                };

                print_receipt(ref_no, receipt);

                save_coins(username, baraco_tix_disc);

                while (true) {
                    cout << "> Would you like to go back to the user menu? (Y/N): ";
                    string again;
                    cin >> again;
                    if (again == "Y" || again == "y") {
                        user_menu(username);
                        return;
                    } else if (again == "N" || again == "n") {
                        select_route(username);
                        return;
                    } else {
                        cout << "\t>> Invalid input. Please try again.\n";
                    }
                }
            } else if (elig_disc == "N" || elig_disc == "n") {
                if (user_coins < baraco_tix) {
                    cout << "\t>> Insufficient coins. Please top up your card.\n";
                    user_menu(username);
                    return;
                }

                total_sales.push_back(baraco_tix);
                cout << "\t>> Thank you for riding with BARACO. Your total ticketing fare is Php " << fixed << setprecision(2) << baraco_tix << ".\n\n";

                string ref_no = generate_ref_no();
                string date_time = get_current_datetime();

                map<string, string> receipt = {
                    {"rcp_datetime", date_time},
                    {"rcp_cond", cond_name},
                    {"rcp_orig", to_string(orig_pt)},
                    {"rcp_dest", to_string(dest_pt)},
                    {"rcp_tix", to_string(baraco_tix)},
                    {"rcp_disc", "0"},
                    {"rcp_tix_disc", to_string(baraco_tix)}
                };

                print_receipt(ref_no, receipt);

                save_coins(username, baraco_tix);

                while (true) {
                    cout << "> Would you like to go back to the user menu? (Y/N): ";
                    string again;
                    cin >> again;
                    if (again == "Y" || again == "y") {
                        user_menu(username);
                        return;
                    } else if (again == "N" || again == "n") {
                        select_route(username);
                        return;
                    } else {
                        cout << "\t>> Invalid input. Please try again.\n";
                    }
                }
            } else {
                cout << "\t>> Invalid input. Please try again.\n";
            }
        }
    } else {
        cout << "\t>> Invalid route. Please try again.\n";
    }
}
void sign_out() {
    cout << "\t>> Signing out...\n";
    main_menu();
}

void main_menu() {
    cout << endl;
    cout << string(100, '-') << endl << endl;
    print_centered("   __      __      __ __  ", 100);
    print_centered("  |__) /\\ |__) /\\ /  /  \\ ", 100);
    print_centered("  |__)/--\\| \\ /--\\\\__\\__/ ", 100);
    print_centered("                          ", 100); 
    print_centered("Batangas Railway Corporation", 100);
    print_centered("Main Menu", 100);
    cout << endl;

    vector<string> menu_options = {
        "Activate BARACO Card",
        "Open BARACO Card",
        "Exit"
    };

    for (size_t i = 0; i < menu_options.size(); ++i) {
        cout << i + 1 << ". " << menu_options[i] << "\n";
    }

    while (true) {
        cout << "\n> Kindly select an option from the BARACO Main Menu: ";
        int choice;
        while(!(cin >> choice)){
            cout << "Invalid input. Please enter an integer: ";
            cin.clear();
            cin.ignore(1000000, '\n');
        };
        switch (choice) {
        case 1:
            activate_card();
            break;
        case 2:
            open_card();
            break;
        case 3:
            cout << "\t>> Exiting BARACO System...\n";
            exit(0);
        default:
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}

int main() {
    create_src();
    card_gen();
    load_users();
    main_menu();
    return 0;
}