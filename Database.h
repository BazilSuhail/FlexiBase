#include "Helper.h" 
using namespace std;

struct Record {
    my_hashmap<string, string> record;
    my_hashmap<int, Record*> Objects;
};

class Database {
private:
    my_hashmap<int, Record> object_record;
    my_hashmap<int, Record*> id_object;
    B_tree btree;
    string current_file = "";
    string current_btree_file = "";

    int gen_ID() {
        return rand() % 10000 + 1;
    }

    string trim_whitespace(const string& str) {
        size_t first = str.find_first_not_of(" \n\r\t");
        size_t last = str.find_last_not_of(" \n\r\t");
        if (first == string::npos || last == string::npos) {
            return "";
        }
        return str.substr(first, last - first + 1);
    }

    void read_data() {
        ifstream rdr(current_file);
        if (rdr.is_open()) {
            string line;
            while (getline(rdr, line)) {
                if (line.find("_id:") != string::npos) {
                    int id = stoi(line.substr(line.find(":") + 1));
                    long pos = rdr.tellg();

                    Record rec;
                    rec.record.clear();

                    while (getline(rdr, line)) {
                        if (line.find("}") != string::npos) {
                            break;
                        }

                        size_t find_colon = line.find(":");
                        if (find_colon != string::npos) {
                            string key = trim_whitespace(line.substr(0, find_colon));
                            string value = trim_whitespace(line.substr(find_colon + 1));
                            rec.record[key] = value;
                        }
                    }
                    object_record[id] = rec;
                    id_object[id] = &object_record[id];
                    btree.insert(make_pair(id, pos));
                }
            }
            rdr.close();
        }
        else {
            cout << "Error opening the file for reading.\n";
        }
    }

    void write_data(int id, my_hashmap<string, string>& data) {
        ofstream wrtr(current_file, ios::app);
        if (wrtr.is_open()) {
            long pos = wrtr.tellp();
            wrtr << "{\n";
            wrtr << "  _id: " << id << "\n";
            for (auto i = data.begin(); i != data.end(); ++i) {
                const string& key = i->key;
                const string& value = i->data;
                wrtr << "  " << key << ": " << value << "\n";
            }
            wrtr << "}\n\n";

            wrtr.close();
            btree.insert(make_pair(id, pos));
            btree.serializeToFile(current_btree_file);
        }
        else {
            cerr << "Error opening the file for writing.\n";
        }
    }

    void update_data() {
        ofstream wrtr(current_file);
        if (wrtr.is_open()) {
            btree = B_tree(btree.getDegree());
            for (auto it = object_record.begin(); it != object_record.end(); ++it) {
                const auto& P = *it;
                const auto& rec = P.data;
                long pos = wrtr.tellp();
                wrtr << "{\n";
                wrtr << "  _id: " << P.key << "\n";

                for (auto propIt = rec.record.begin(); propIt != rec.record.end(); ++propIt) {
                    const string& key = propIt->key;
                    const string& value = propIt->data;
                    wrtr << "  " << key << ": " << value << "\n";
                }

                wrtr << "}\n\n";
                btree.insert(make_pair(P.key, pos));
            }
            wrtr.close();
            btree.serializeToFile(current_btree_file);
        }
        else {
            cerr << "Error opening the file for writing.\n";
        }
    }

public:
    Database() : btree(3) {
        if (current_file.empty()) {
            cout << "No database in use. Please use 'create' or 'use' command first.\n";
            return;
        }
    }

    void SetClr(int clr) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
    }

    bool create_database(const string& filename) {
        ofstream wrtr(filename);
        if (wrtr.is_open()) {
            wrtr.close();
            current_file = filename;
            current_btree_file = filename + "-btree.txt";
            object_record.clear();
            id_object.clear();
            btree = B_tree(3); // Assuming degree of 3 for B-tree
            SetClr(11);
            cout << "Database \"" << current_file << "\" created and in use.\n\n";
            SetClr(15);
            return true;
        }
        else {
            SetClr(4);
            cerr << " Error creating the database file.\n\n";
            SetClr(15);
            return false;
        }
    }

    bool use_database(const string filename) {
        ifstream rdr(filename);
        if (rdr.is_open()) {
            rdr.close();
            current_file = filename;
            current_btree_file = filename + "-btree.txt";
            btree.deserializeFromFile(current_btree_file);
            read_data();
            SetClr(6);
            string databse_name;
            databse_name = current_file.substr(0, filename.size() - 5);

            cout << " Using database \"" << databse_name << "\".\n\n";
            SetClr(15);
            return true;
        }
        else {
            SetClr(4);
            cerr << "Error opening the Database.\n\n";
            SetClr(15);
            return false;
        }
    }

    void exit_database() {
        SetClr(12);
        cout << "Exited the current \"" << current_file << "\" database.\n\n";
        SetClr(15);

        current_file = "";
        object_record.clear();
        id_object.clear();
    }

    void Insert_object(my_hashmap<string, string>& data) {
        if (current_file.empty()) {
            cout << "No database in use. Please use 'create' or 'use' command first.\n";
            return;
        }

        int id = gen_ID();
        Record to_insert;
        to_insert.record = data;

        object_record[id] = to_insert;
        id_object[id] = &object_record[id];
        write_data(id, data);
        update_data();
        SetClr(2);
        cout << "{ nInserted , Assigned ==> \"_id\": " << id << " , Evaluation = True }\n\n";
        SetClr(15);
    }

    void delete_object(int id) {
        if (object_record.find(id) != nullptr) {
            object_record.erase(id);
            id_object.erase(id);
            update_data();
            SetClr(2);
            cout << "\n{\"_id\": " << id << " , Deletion-Evaluation = True }\n\n";
        }
        else {
            SetClr(4);
            cout << "\n{\"_id\": " << id << " , Deletion-Evaluation = True }\n\n";
        }
        SetClr(15);
    }

    void insert_a_key(int id, const string& key, const string& value) {
        if (object_record.find(id) != nullptr) {
            object_record[id].record[key] = value;
            update_data();
            SetClr(2);
            cout << "\n{\"_id\": " << id << " , InsertionEvaluation = True , KeyStatus = Updated }\n\n";
        }
        else {
            SetClr(4);
            cout << "\n{\"_id\": " << id << " , InsertionEvaluation = False , \"_id\" = False }\n\n";
        }
        SetClr(15);
    }

    void delete_a_key(int id, const string& key) {
        if (object_record.find(id) != nullptr) {
            if (object_record[id].record.find(key) != nullptr) {
                object_record[id].record.erase(key);
                update_data();
                SetClr(2);
                cout << "\n{\"_id\": " << id << " , Deletion-Evaluation = True }\n\n";
            }
            else {
                SetClr(4);
                cout << "\n{\"_id\": " << id << " , Deletion-Evaluation = False , key not FOUND}\n\n";
            }
        }
        else {
            SetClr(4);
            cout << "\n{\"_id\": " << id << " , Deletion-Evaluation = False , _id not FOUND}\n\n";
        }
        SetClr(15);
    }

    void update_a_key(int id, const string& key, const string& new_value) {
        if (object_record.find(id) != nullptr) {
            object_record[id].record[key] = new_value;
            update_data();
            SetClr(2);
            cout << "\n{\"_id\": " << id << " , UpdationEvaluation = True }\n\n";
            SetClr(15);
        }
        else {
            SetClr(4);
            cout << "\n{\"_id\": " << id << " , UpdationEvaluation = False }\n\n";
            SetClr(15);
        }
    }
    // Function to find the object based on ID and return the record
    Record* find_the_object(int id, const string& current_file, B_tree& btree) {
        long pos = btree.Return_position(id);
        if (pos == -1) {
            return nullptr;  // Object with the given ID not found
        }

        ifstream rdr(current_file);
        rdr.seekg(pos);
        string line;
        Record* rec = new Record();
        rec->record.clear();

        while (getline(rdr, line)) {
            if (line.find("}") != string::npos) {
                break;
            }

            size_t find_colon = line.find(":");
            if (find_colon != string::npos) {
                string key = trim_whitespace(line.substr(0, find_colon));
                string value = trim_whitespace(line.substr(find_colon + 1));
                rec->record[key] = value;
            }
        }

        rdr.close();
        return rec;
    }

    void find_object_with_key(int id) {
        Record* rec = find_the_object(id, current_file, btree);
        SetClr(2);
        cout << "\n{\"_id\": " << id << " , Search-Evaluation = True }\n\n";
        SetClr(11);
        if (rec) {
            cout << "{\n";
            cout << "\t\"_id\": \"" << id << "\"\n";
            for (auto it = rec->record.begin(); it != rec->record.end(); ++it) {
                if (it->key != "_id") {
                    cout << "\t" << it->key << ": " << it->data << "\n";
                }
            }
            cout << "}\n\n";
            delete rec;  // Clean up allocated memory
        }
        else {
            SetClr(4);
            cout << "\n{\"_id\": " << id << " , Search-Evaluation = False }\n\n";
        }
        SetClr(15);
    }
    void find_key_value(const string& key, const string& value) {
        ifstream rdr(current_file);
        if (!rdr.is_open()) {
            SetClr(12);
            cerr << "Error opening the database file.\n";
            SetClr(15);
            return;
        }

        string line;
        bool found = false;
        bool in_object = false;
        bool match_found = false;
        string current_object;

        cout << "\n[\n";
        SetClr(8);

        while (getline(rdr, line)) {
            if (line.find("{") != string::npos) {
                current_object = "";
                in_object = true;
            }

            if (in_object) {
                current_object += line + "\n";

                if (line.find("}") != string::npos) {
                    in_object = false;
                    Record rec;
                    rec.record.clear();

                    // Parse current_object to extract key-value pairs
                    istringstream ss(current_object);
                    string obj_line;
                    string obj_key, obj_value;
                    bool found_in_object = false;

                    while (getline(ss, obj_line)) {
                        size_t find_colon = obj_line.find(":");
                        if (find_colon != string::npos) {
                            obj_key = trim_whitespace(obj_line.substr(0, find_colon));
                            obj_value = trim_whitespace(obj_line.substr(find_colon + 1));

                            if (obj_key == key && obj_value == value) {
                                found_in_object = true;
                                match_found = true;
                            }
                            if (obj_key == "_id") {
                                rec.record[obj_key] = obj_value;
                            }
                            else {
                                rec.record[obj_key] = obj_value;
                            }
                        }
                    }

                    if (found_in_object) {
                        if (found) {
                            cout << "\n";
                        }
                        cout << "    {\n";
                        // Print the object with color coding
                        istringstream ss_print(current_object);
                        string print_line;
                        while (getline(ss_print, print_line)) {
                            size_t find_colon = print_line.find(":");
                            if (find_colon != string::npos) {
                                string print_key = trim_whitespace(print_line.substr(0, find_colon));
                                string print_value = trim_whitespace(print_line.substr(find_colon + 1));
                                if (print_key == key && print_value == value) {
                                    SetClr(10);  // Green
                                }
                                else {
                                    SetClr(8);  // Gray
                                }
                                cout << "\t    " << print_key << ": " << print_value << "\n";
                            }
                        }
                        cout << "    }\n";
                        found = true;
                    }
                }
            }
        }

        if (!match_found) {
            SetClr(12);
            cout << "\n{ Search-Evaluation = False } ==> No object found with key: \"" << key << "\" and value: \"" << value << "\".}";
        }

        SetClr(15);  // Reset to default color
        cout << "\n\n]\n\n";
        rdr.close();
    }

    // Function to print the entire file as it is on screen with "_id" in quotes
    void print_entire_file() {
        ifstream rdr(current_file);
        if (rdr.is_open()) {
            string line;
            SetClr(7);
            while (getline(rdr, line)) {
                if (line.find("_id:") != string::npos) {
                    size_t find_colon = line.find(":");
                    string key = trim_whitespace(line.substr(0, find_colon));
                    string value = trim_whitespace(line.substr(find_colon + 1));
                    cout << "\t" << "\"" << key << "\": \"" << value << "\"" << endl;
                }
                else {
                    cout << "\t" << line << endl;
                }
            }
            rdr.close();
        }
        else {
            SetClr(12);
            cout << "\n{ Evaluation = False==> Database Not found }\n\n";
        }
        SetClr(15);
    }

    // Function to print the entire file in a single line per object with "_id" in quotes
    void print_entire_file_single_line() {
        SetClr(7);
        ifstream rdr(current_file);
        if (rdr.is_open()) {
            string line;
            while (getline(rdr, line)) {
                if (line.find("{") != string::npos) {
                    cout << "    { ";
                }
                else if (line.find("}") != string::npos) {
                    cout << "}\n";
                }
                else {
                    size_t find_colon = line.find(":");
                    if (find_colon != string::npos) {
                        string key = trim_whitespace(line.substr(0, find_colon));
                        string value = trim_whitespace(line.substr(find_colon + 1));
                        if (key == "_id") {
                            cout << "\"" << key << "\": \"" << value << "\" ";
                        }
                        else {
                            cout << key << ": " << value << " ";
                        }
                    }
                }
            }
            rdr.close();
        }
        else {
            SetClr(12);
            cout << "\n{ Evaluation = False==> Database Not found }\n\n";
        }
        SetClr(15);
    }
    void display_help() {
        SetClr(3);
        cout << "FlexiBase Shell Commands:\n\n";
        cout << "1. create <database_name>\n   - Creates a new database file with the specified name.\n\n";
        cout << "2. use <database_name>\n   - Switches to the specified database file.\n\n";
        cout << "3. exit db\n   - Exits the current database.\n\n";
        cout << "4. db.insert([key1: value1], [key2: value2], ...)\n   - Inserts a new object with the specified key-value pairs into the database.\n\n";
        cout << "5. db.insertkey(<id>, <key>, <value>)\n   - Inserts or updates a key-value pair in the object with the specified ID.\n\n";
        cout << "6. db.deletekey(<id>, <key>)\n   - Deletes the key-value pair from the object with the specified ID.\n\n";
        cout << "7. db.delete(<id>)\n   - Deletes the object with the specified ID from the database.\n\n";
        cout << "8. db.update(<id>, <key>, <value>)\n   - Updates the value of the specified key in the object with the specified ID.\n\n";
        cout << "9. db.find(<id>)\n    - Finds and displays the object with the specified ID.\n\n10. db.show()\n     - Displays all objects in the database in a compact format.\n\n";
        cout << "11. db.show.pretty()\n    - Displays all objects in the database in a detailed format.\n\n";
        cout << "12. db.findbyvalue(<key>, <value>)\n    - Finds and displays all objects that contain the specified key-value pair.\n\n";
        cout << "13. clear\n    - Clears the console screen.\n\n";
        cout << "14. exit\n    - Exits the shell.\n\n";
        cout << "15. help\n    - Displays this help menu.\n\n";
        SetClr(15);
    }

};
