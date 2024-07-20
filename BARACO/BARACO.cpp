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

unordered_set<string> existing_usernames;
unordered_set<string> existing_passwords;
unordered_set<string> existing_ids;

void print_receipt();
void create_src();
string generate_unique_id();
string trim();
void card_gen();
void load_users();
void save_users();
void save_username();
void save_password();
void activate_card();
void open_card();
void print_centered();
void user_menu();
void view_stations();
string generate_ref_no();
string get_current_datetime();
void select_route();
void main_menu();

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

            getline(iss, temp);
            string id_number = trim(temp.substr(temp.find(":") + 1));

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
        users_file << "Username: " << username << ", Password: " << password << ", ID Number: " << id_number << endl;
        users_file.close();
    }
    else {
        cerr << "\tUnable to open 'users.txt'." << endl;
    }
}

void save_username(const string& username) {
    existing_usernames.insert(username);
}

void save_password(const string& password) {
    existing_passwords.insert(password);
}

void activate_card() {
    string id_number, username, password;

    cout << string(100, '_') << endl << endl;
    print_centered("BARACO", 100);
    print_centered("Batangas Railway Corporation", 100);
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
    save_users(username, password, id_number);

    ofstream card_file_out("src/card_numbers.txt");
    if (card_file_out.is_open()) {
        for (const string& card : card_numbers) {
            card_file_out << card << endl;
        }
        card_file_out.close();
    }
    else {
        cerr << "\t>> Unable to open card_numbers.txt" << endl;
    }

    cout << "\t>> Card activated successfully!" << endl;
    main_menu();
}

void open_card() {
    string username, password;

    cout << string(100, '_') << endl << endl;
    print_centered("BARACO", 100);
    print_centered("Batangas Railway Corporation", 100);
    print_centered("Open BARACO Card", 100);
    cout << endl;

    cout << "> Enter your username: ";
    cin >> username;
    cout << "> Enter your password: ";
    cin >> password;

    if (existing_usernames.find(username) != existing_usernames.end() &&
        existing_passwords.find(password) != existing_passwords.end()) {
        cout << "\t>> Sign-in successful!" << endl;
        user_menu();
    }
    else {
        cerr << "\t>> Invalid username or password. Please try again." << endl;
    }
}

void user_menu() {
    cout << string(100, '_') << endl << endl;
    print_centered("BARACO", 100);
    print_centered("Batangas Railway Corporation", 100);
    print_centered("User Menu", 100);
    cout << endl;

    vector<string> menu_options = {
        "View Stations",
        "Select Route",
        "Exit"
    };

    for (size_t i = 0; i < menu_options.size(); ++i) {
        cout << i + 1 << ". " << menu_options[i] << "\n";
    }

    while (true) {
        cout << "\n> Kindly select an option: ";
        int choice;
        cin >> choice;
        switch (choice) {
        case 1:
            view_stations();
            break;
        case 2:
            select_route();
            break;
        case 3:
            cout << "\t>> Exiting...\n";
            exit(0);
        default:
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}

void view_stations() {
    cout << string(100, '_') << endl << endl;
    print_centered("View BARACO Stations", 100);
    cout << endl;

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    while (true) {
        cout << "\n> Would you like to go back to the User Menu? (Y/N) ";
        string choice;
        cin >> choice;
        if (choice == "Y" || choice == "y") {
            user_menu();
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
    cout << string(100, '_') << endl << endl;
    print_centered("BARACO Official Receipt", 100);
    cout << endl;
    cout << setw(37 + ("Reference No.: " + ref_no).size() / 2) << "Reference No.: " + ref_no + "\n";
    cout << setw(37 + ("Date and Time: " + receipt.at("rcp_datetime")).size() / 2) << "Date and Time: " + receipt.at("rcp_datetime") + "\n";
    cout << setw(37 + ("From: " + baraco_stations.at(receipt.at("rcp_orig"))).size() / 2) << "From: " + baraco_stations.at(receipt.at("rcp_orig")) + "\n";
    cout << setw(37 + ("To: " + baraco_stations.at(receipt.at("rcp_dest"))).size() / 2) << "To: " + baraco_stations.at(receipt.at("rcp_dest")) + "\n";
    cout << setw(37 + ("Regular: Php " + receipt.at("rcp_tix")).size() / 2) << "Regular: Php " + receipt.at("rcp_tix") + "\n";
    cout << setw(37 + ("Discount: Php " + receipt.at("rcp_disc")).size() / 2) << "Discount: Php " + receipt.at("rcp_disc") + "\n";
    cout << setw(37 + ("Total Amount: Php " + receipt.at("rcp_tix_disc")).size() / 2) << "Total Amount: Php " + receipt.at("rcp_tix_disc") + "\n";
    cout << string(75, '_') << "\n";
}

void select_route() {
    baraco_tix = 0;
    baraco_disc = 0;
    baraco_tix_disc = 0;

    cout << string(100, '_') << endl << endl;
    print_centered("Select BARACO Route", 100);
    cout << endl;

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    while (true) {
        cout << "> Kindly select the numerical input corresponding to your origin point: ";
        cin >> orig_pt;
        cout << "> Kindly select the numerical input corresponding to your destination point: ";
        cin >> dest_pt;
        orig_dest = to_string(orig_pt) + "-" + to_string(dest_pt);

        if (routes_km.find(orig_dest) != routes_km.end()) {
            baraco_tix = 5 * abs(routes_km[orig_dest] - 7);
            baraco_tix = static_cast<int>(baraco_tix) + (baraco_tix - static_cast<int>(baraco_tix) >= 0.5 ? 1 : 0);
            total_routes.push_back(routes_km[orig_dest]);

            cout << "\t>> The calculated distance from " << baraco_stations[to_string(orig_pt)] << " to " << baraco_stations[to_string(dest_pt)] << " is " << routes_km[orig_dest] << " kilometers.\n";

            while (true) {
                cout << "> Are you a senior citizen, PWD, or a student? (Y/N) ";
                string elig_disc;
                cin >> elig_disc;
                if (elig_disc == "Y" || elig_disc == "y") {
                    baraco_disc = baraco_tix * 0.2;
                    baraco_tix_disc = baraco_tix - baraco_disc;
                    total_sales.push_back(baraco_tix_disc);
                    cout << "\t>> Thank you for riding with BARACO. Your total ticketing fare with the 20% discount applied is Php " << fixed << setprecision(2) << baraco_tix_disc << ".\n";

                    ref_no = generate_ref_no();
                    date_time = get_current_datetime();

                    baraco_rcp[ref_no] = {
                        {"rcp_datetime", date_time},
                        {"rcp_cond", cond_name},
                        {"rcp_orig", to_string(orig_pt)},
                        {"rcp_dest", to_string(dest_pt)},
                        {"rcp_tix", to_string(baraco_tix)},
                        {"rcp_disc", to_string(baraco_disc)},
                        {"rcp_tix_disc", to_string(baraco_tix_disc)}
                    };

                    print_receipt(ref_no, baraco_rcp[ref_no]);

                    while (true) {
                        cout << "> Would you like to ride again? (Y/N) ";
                        string again;
                        cin >> again;
                        if (again == "Y" || again == "y") {
                            select_route();
                            return;
                        }
                        else if (again == "N" || again == "n") {
                            user_menu();
                            return;
                        }
                        else {
                            cout << "\t>> Invalid input. Please try again.\n";
                        }
                    }
                }
                else if (elig_disc == "N" || elig_disc == "n") {
                    total_sales.push_back(baraco_tix);
                    cout << "\t>> Thank you for riding with BARACO. Your total ticketing fare is Php " << fixed << setprecision(2) << baraco_tix << ".\n";

                    ref_no = generate_ref_no();
                    date_time = get_current_datetime();

                    baraco_rcp[ref_no] = {
                        {"rcp_datetime", date_time},
                        {"rcp_cond", cond_name},
                        {"rcp_orig", to_string(orig_pt)},
                        {"rcp_dest", to_string(dest_pt)},
                        {"rcp_tix", to_string(baraco_tix)},
                        {"rcp_disc", "0"},
                        {"rcp_tix_disc", to_string(baraco_tix)}
                    };

                    print_receipt(ref_no, baraco_rcp[ref_no]);

                    while (true) {
                        cout << "> Would you like to ride again? (Y/N) ";
                        string again;
                        cin >> again;
                        if (again == "Y" || again == "y") {
                            select_route();
                            return;
                        }
                        else if (again == "N" || again == "n") {
                            user_menu();
                            return;
                        }
                        else {
                            cout << "\t>> Invalid input. Please try again.\n";
                        }
                    }
                }
                else {
                    cout << "\t>> Invalid input. Please try again.\n";
                }
            }
        }
        else {
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}

void main_menu() {
    cout << string(100, '_') << endl << endl;
    print_centered("BARACO", 100);
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
        cin >> choice;
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
