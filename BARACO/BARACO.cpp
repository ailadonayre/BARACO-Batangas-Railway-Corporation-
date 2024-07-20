#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <numeric>

using namespace std;

// Global variables
string ref_no = "";
string date_time = "";
string cond_name = "";
string orig_dest = "";
int orig_pt = 0;
int dest_pt = 0;
double baraco_tix = 0;
double baraco_disc = 0;
double baraco_tix_disc = 0;

// Data structures
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

map<string, int> routes_qty = {
    {"1-2", 0}, {"1-3", 0}, {"1-4", 0}, {"1-5", 0},
    {"2-1", 0}, {"2-3", 0}, {"2-4", 0}, {"2-5", 0},
    {"3-1", 0}, {"3-2", 0}, {"3-4", 0}, {"3-5", 0},
    {"4-1", 0}, {"4-2", 0}, {"4-3", 0}, {"4-5", 0},
    {"5-1", 0}, {"5-2", 0}, {"5-3", 0}, {"5-4", 0}
};

map<string, map<string, string>> baraco_rcp;
vector<double> total_routes;
vector<double> total_sales;

// Function declarations
void main_menu();
void view_stations();
string generate_ref_no();
string get_current_datetime();
void print_receipt(const string& ref_no, const map<string, string>& receipt);
void select_route();
void view_sales();

// View BARACO stations
void view_stations() {
    cout << string(50, '_') << "\n\n";
    cout << setw(60 + 10) << "BARACO Stations\n\n";

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    while (true) {
        cout << "\n> Would you like to go back to the main menu? (Y/N) ";
        string choice;
        cin >> choice;
        if (choice == "Y" || choice == "y") {
            main_menu();
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

// Generate random reference number
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

// Get current date and time
string get_current_datetime() {
    time_t now = time(0);
    struct tm ltm;
    localtime_s(&ltm, &now);

    char buf[80];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &ltm);
    return string(buf);
}

// Print receipt based on provided data
void print_receipt(const string& ref_no, const map<string, string>& receipt) {
    cout << string(75, '_') << "\n\n";
    cout << setw(37 + 24) << "BARACO Official Receipt\n";
    cout << setw(37 + 27) << "Batangas Railway Corporation\n";
    cout << setw(37 + ("Reference No.: " + ref_no).size() / 2) << "Reference No.: " + ref_no + "\n";
    cout << setw(37 + ("Date and Time: " + receipt.at("rcp_datetime")).size() / 2) << "Date and Time: " + receipt.at("rcp_datetime") + "\n";
    cout << setw(37 + ("Conductor: " + receipt.at("rcp_cond")).size() / 2) << "Conductor: " + receipt.at("rcp_cond") + "\n";
    cout << setw(37 + ("From: " + baraco_stations.at(receipt.at("rcp_orig"))).size() / 2) << "From: " + baraco_stations.at(receipt.at("rcp_orig")) + "\n";
    cout << setw(37 + ("To: " + baraco_stations.at(receipt.at("rcp_dest"))).size() / 2) << "To: " + baraco_stations.at(receipt.at("rcp_dest")) + "\n";
    cout << setw(37 + ("Regular: Php " + receipt.at("rcp_tix")).size() / 2) << "Regular: Php " + receipt.at("rcp_tix") + "\n";
    cout << setw(37 + ("Discount: Php " + receipt.at("rcp_disc")).size() / 2) << "Discount: Php " + receipt.at("rcp_disc") + "\n";
    cout << setw(37 + ("Total Amount: Php " + receipt.at("rcp_tix_disc")).size() / 2) << "Total Amount: Php " + receipt.at("rcp_tix_disc") + "\n";
    cout << string(75, '_') << "\n";
}

// Select BARACO route and process ticketing
void select_route() {
    baraco_tix = 0;
    baraco_disc = 0;
    baraco_tix_disc = 0;

    cout << string(150, '_') << "\n\n";
    cout << setw(75 + 19) << "Select BARACO Route\n\n";

    int i = 1;
    for (const auto& station : baraco_stations) {
        cout << i << ". " << station.second << "\n";
        i++;
    }

    cout << "\n> Kindly enter your name: ";
    cin.ignore();
    getline(cin, cond_name);
    transform(cond_name.begin(), cond_name.end(), cond_name.begin(), ::toupper);

    while (true) {
        cout << "\n> Kindly select the numerical input corresponding to your origin point: ";
        cin >> orig_pt;
        cout << "> Kindly select the numerical input corresponding to your destination point: ";
        cin >> dest_pt;
        orig_dest = to_string(orig_pt) + "-" + to_string(dest_pt);

        if (routes_km.find(orig_dest) != routes_km.end()) {
            baraco_tix = 5 * abs(routes_km[orig_dest] - 7);
            baraco_tix = static_cast<int>(baraco_tix) + (baraco_tix - static_cast<int>(baraco_tix) >= 0.5 ? 1 : 0);
            total_routes.push_back(routes_km[orig_dest]);
            routes_qty[orig_dest]++;

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
                            main_menu();
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
                            main_menu();
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

/*void view_sales() {
    cout << string(150, '_') << "\n\n";
    cout << setw(75 + 29) << "BARACO Total Sales and Routes\n\n";

    cout << "Summary of Sales by Routes:\n";
    for (const auto& route_qty : routes_qty) {
        if (route_qty.second != 0) {
            cout << "Route " << route_qty.first << ": " << route_qty.second << " trip(s)\n";
        }
    }

    cout << "\nTotal Sales Amount: Php " << fixed << setprecision(2) << accumulate(total_sales.begin(), total_sales.end(), 0.0) << "\n";
    cout << "Total Distance Travelled: " << fixed << setprecision(2) << accumulate(total_routes.begin(), total_routes.end(), 0.0) << " kilometers\n";

    while (true) {
        cout << "\n> Would you like to go back to the main menu? (Y/N) ";
        string choice;
        cin >> choice;
        if (choice == "Y" || choice == "y") {
            main_menu();
            return;
        }
        else if (choice == "N" || choice == "n") {
            return;
        }
        else {
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}*/

// Main menu function
void main_menu() {
    cout << string(100, '_') << "\n\n";
    cout << setw(55) << "BARACO\n\n";
    cout << setw(66) << "Batangas Railway Corporation\n\n";

    vector<string> menu_options = {
        "View BARACO stations",
        "Select BARACO route",
        "View BARACO total sales and routes",
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
            view_stations();
            return;
        case 2:
            select_route();
            return;
        case 3:
            view_sales();
            return;
        case 4:
            cout << "\t>> Exiting BARACO System...\n";
            exit(0);
        default:
            cout << "\t>> Invalid input. Please try again.\n";
        }
    }
}

// Main function
int main() {
    main_menu();
    return 0;
}
