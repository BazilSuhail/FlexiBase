#include "Database.h"

using namespace std;

int main() {
    HWND console = GetConsoleWindow();
    RECT rect;
    GetWindowRect(console, &rect);

    // Set new width and height (in pixels)
    MoveWindow(console, rect.left, rect.top, 1050, 590, TRUE);
    cout << "Welcome to FlexiBase Shell !!\n\n";
    bool database_in_use = false;
    string current_db;
    Database myDB;

    do {
        srand(static_cast<unsigned>(time(nullptr)));
        cout << "->";
        string inpt;
        getline(cin, inpt);

        // Create a new database
        if (inpt.find("create ") != string::npos) {
            string filename = inpt.substr(7) + ".json";
            database_in_use = myDB.create_database(filename);
            current_db = filename;
        }

        // Use an existing database
        else if (inpt.find("use ") != string::npos) {
            string filename = inpt.substr(4) + ".json";
            database_in_use = myDB.use_database(filename);
            current_db = filename;
        }

        // Exit the current database
        else if (inpt.find("exit db") != string::npos) {
            myDB.exit_database();
            database_in_use = false;
            current_db = "";
        }

        // db.insert
        else if (database_in_use && inpt.find("db.insert(") != string::npos) {
            size_t pos = inpt.find("db.insert(");
            if (pos != string::npos) {
                inpt.erase(pos, 10);
            }
            my_hashmap<string, string> key_data;
            istringstream process(inpt);

            while (process >> ws && process.peek() == '[') {
                process.ignore();

                string key, value;
                getline(process, key, ':');
                getline(process, value, ']');

                key.erase(key.find_last_not_of(" \n\r\t") + 1);
                value.erase(0, value.find_first_not_of(" \n\r\t"));
                value.erase(value.find_last_not_of(" \n\r\t") + 1);

                key_data[key] = value;

                if (process.peek() == ',') {
                    process.ignore();
                }
            }
            process.ignore();
            myDB.Insert_object(key_data);
        }

        // db.insertkey
        else if (database_in_use && inpt.find("db.insertkey(") != string::npos) {
            size_t pos = inpt.find("db.insertkey(");
            if (pos != string::npos) {
                inpt.erase(pos, 13);
            }
            string read_id, key, val;
            istringstream process_id(inpt);

            getline(process_id, read_id, ',');
            getline(process_id, key, ',');
            getline(process_id, val, ')');

            int int_id = stoi(read_id);
            myDB.insert_a_key(int_id, key, val);
        }

        // db.deletekey 
        else if (database_in_use && inpt.find("db.deletekey(") != string::npos) {
            size_t pos = inpt.find("db.deletekey(");
            if (pos != string::npos) {
                inpt.erase(pos, 13);
            }
            string read_id, key;
            istringstream idStream(inpt);

            getline(idStream, read_id, ',');
            getline(idStream >> std::ws, key, ')');

            int int_id = stoi(read_id);
            myDB.delete_a_key(int_id, key);
        }

        // db.delete
        else if (database_in_use && inpt.find("db.delete(") != string::npos) {
            size_t pos = inpt.find("db.delete(");
            if (pos != string::npos) {
                inpt.erase(pos, 10);
            }
            string read_id;
            istringstream idStream(inpt);

            getline(idStream, read_id, ')');
            int int_id = stoi(read_id);

            myDB.delete_object(int_id);
        }

        // db.update
        else if (database_in_use && inpt.find("db.update(") != string::npos) {
            size_t pos = inpt.find("db.update(");
            if (pos != string::npos) {
                inpt.erase(pos, 10);
            }

            string read_id, key, val;
            istringstream idStream(inpt);

            getline(idStream, read_id, ',');
            getline(idStream, key, ',');
            getline(idStream, val, ')');

            int int_id = stoi(read_id);
            myDB.update_a_key(int_id, key, val);
        }

        // db.find
        else if (database_in_use && inpt.find("db.find(") != string::npos) {
            size_t pos = inpt.find("db.find(");
            if (pos != string::npos) {
                inpt.erase(pos, 8);
            }

            string read_id;
            istringstream process(inpt);
            getline(process, read_id, ')');

            int int_id = stoi(read_id);
            myDB.find_object_with_key(int_id);
        }

        // db.show
        else if (database_in_use && inpt.find("db.show(") != string::npos) {
            size_t pos = inpt.find("db.show(");
            if (pos != string::npos) {
                inpt.erase(pos, 8);
            }
            string read_id;
            istringstream process(inpt);
            getline(process, read_id, ')');
            cout << "\n[\n";
            myDB.print_entire_file_single_line();
            cout << "]\n\n";
        }

        // db.show.pretty
        else if (database_in_use && inpt.find("db.show.pretty(") != string::npos) {
            size_t pos = inpt.find("db.show(");
            if (pos != string::npos) {
                inpt.erase(pos, 8);
            }
            string read_id;
            istringstream process(inpt);
            getline(process, read_id, ')');
            cout << "\n[\n";
            myDB.print_entire_file();
            cout << "]\n\n";
        }

        // db.findbyValue
        else if (database_in_use && inpt.find("db.findbyvalue(") != string::npos) {
            size_t pos = inpt.find("db.findbyvalue(");
            if (pos != string::npos) {
                inpt.erase(pos, 15);
            }
            string key, val;
            istringstream process_id(inpt);

            getline(process_id, key, ',');
            getline(process_id, val, ')');

            myDB.find_key_value(key, val);
        }

        // clear
        else if (inpt.find("clear") != string::npos) {
            system("cls");
        }
        // help
        else if (inpt.find("help") != string::npos) {
            myDB.display_help();
        }

        // exit
        else if (inpt.find("exit") != string::npos) {
            break;
        }

        else {
            myDB.SetClr(12);
            cout << "Invalid command !! || No Database in use !!\n\n";
            myDB.SetClr(15);
        }

    } while (true);

    return 0;
}
