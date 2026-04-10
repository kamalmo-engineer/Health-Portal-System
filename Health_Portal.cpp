#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class user
{
protected:
    string name;
    int id;
    string password;

public:
    void set_name(string n) { name = n; }
    void set_password(string pass) { password = pass; }
    void set_id(int i) { id = i; }
    int getId() { return id; }
    string getName() { return name; }
    string getPass() { return password; }
    bool checkPassword(string pass) { return password == pass; }
};

class MedicalRecord
{
protected:
    int key[2][2] = {{1, 2}, {3, 5}};
    int invkey[2][2] = {{-5, 2}, {3, -1}};
    vector<int> encryptedData;

    int charToNum(char c)
    {
        if (c == ' ')
            return 27;
        if (c >= 'A' && c <= 'Z')
            return c - 'A' + 1;
        if (c >= 'a' && c <= 'z')
            return c - 'a' + 28;
        return 0;
    }

    char numToChar(int n)
    {
        if (n == 27)
            return ' ';
        if (n >= 1 && n <= 26)
            return 'A' + n - 1;
        if (n >= 28 && n <= 53)
            return 'a' + (n - 28);
        return '?';
    }

public:
    void encrypt(string word)
    {
        encryptedData.clear();
        if (word.size() % 2 != 0)
            word += ' ';
        int y = word.size() / 2;
        for (int j = 0; j < y; j++)
        {
            int p1 = charToNum(word[2 * j]);
            int p2 = charToNum(word[2 * j + 1]);
            encryptedData.push_back(key[0][0] * p1 + key[0][1] * p2);
            encryptedData.push_back(key[1][0] * p1 + key[1][1] * p2);
        }
    }

    void decryptAndShow()
    {
        if (encryptedData.empty())
        {
            cout << "No records yet." << endl;
            return;
        }
        cout << "Medical History: ";
        int y = encryptedData.size() / 2;
        for (int j = 0; j < y; j++)
        {
            int e1 = encryptedData[2 * j];
            int e2 = encryptedData[2 * j + 1];
            int r1 = invkey[0][0] * e1 + invkey[0][1] * e2;
            int r2 = invkey[1][0] * e1 + invkey[1][1] * e2;
            cout << numToChar(r1) << numToChar(r2);
        }
        cout << endl;
    }

    vector<int> &getEncryptedData() { return encryptedData; }
    void setEncryptedData(vector<int> data) { encryptedData = data; }
};

class Patient : public user, public MedicalRecord
{
private:
    string upcoming_visit = "None";
    int appointmentBalance = 0;

public:
    void view_medical_history() { decryptAndShow(); }
    void check_balance()
    {
        cout << "Appointments: " << appointmentBalance << " | Next: " << upcoming_visit << endl;
    }
    void book_visit(string slot)
    {
        upcoming_visit = slot;
        appointmentBalance++;
        cout << "Booked!" << endl;
    }
    string getVisit() { return upcoming_visit; }
    int getBalance() { return appointmentBalance; }
    void set_visit(string v) { upcoming_visit = v; }
    void set_balance(int b) { appointmentBalance = b; }
};

class Dr : public user
{
private:
    string adminPassword = "123";
    int loginAttempts = 0;

public:
    bool login(string pass)
    {
        if (pass == adminPassword)
        {
            loginAttempts = 0;
            return true;
        }
        loginAttempts++;
        cout << "Wrong! Attempt " << loginAttempts << "/3" << endl;
        if (loginAttempts >= 3)
        {
            cout << "System Terminating..." << endl;
            exit(0);
        }
        return false;
    }
};

class portal_system
{
private:
    Patient patients[100];
    int patientCount = 0;
    Dr admin;

    Patient *findPatient(int id)
    {
        for (int i = 0; i < patientCount; i++)
        {
            if (patients[i].getId() == id)
                return &patients[i];
        }
        return nullptr;
    }

    void saveToFile()
    {
        ofstream outFile("database.txt");
        if (outFile.is_open())
        {
            outFile << patientCount << endl;
            for (int i = 0; i < patientCount; i++)
            {
                outFile << patients[i].getName() << " "
                        << patients[i].getId() << " "
                        << patients[i].getPass() << " "
                        << patients[i].getVisit() << " "
                        << patients[i].getBalance() << " ";

                vector<int> data = patients[i].getEncryptedData();
                outFile << data.size() << " ";
                for (int val : data)
                    outFile << val << " ";
                outFile << endl;
            }
            outFile.close();
        }
    }

    void loadFromFile()
    {
        ifstream inFile("database.txt");
        if (inFile.is_open())
        {
            if (!(inFile >> patientCount))
                return;
            for (int i = 0; i < patientCount; i++)
            {
                string n, p, v;
                int id, bal, dataSize;
                inFile >> n >> id >> p >> v >> bal >> dataSize;
                patients[i].set_name(n);
                patients[i].set_id(id);
                patients[i].set_password(p);
                patients[i].set_visit(v);
                patients[i].set_balance(bal);

                vector<int> data;
                for (int j = 0; j < dataSize; j++)
                {
                    int temp;
                    inFile >> temp;
                    data.push_back(temp);
                }
                patients[i].setEncryptedData(data);
            }
            inFile.close();
        }
    }

public:
    portal_system() { loadFromFile(); }

    void run()
    {
        int choice;
        while (true)
        {
            cout << "\n1. Doctor Mode\n2. Patient Mode\n3. EXIT\nChoice: ";
            cin >> choice;
            if (choice == 1)
                doctorMenu();
            else if (choice == 2)
                patientMenu();
            else if (choice == 3)
            {
                saveToFile();
                break;
            }
        }
    }

    void doctorMenu()
    {
        string pass;
        cout << "Admin Password: ";
        cin >> pass;
        if (admin.login(pass))
        {
            int drChoice;
            while (true)
            {
                cout << "\n1. Add Patient\n2. Delete\n3. Update History\n4. Logout\nChoice: ";
                cin >> drChoice;
                if (drChoice == 1 && patientCount < 100)
                {
                    Patient p;
                    string n, ps;
                    int id;
                    cout << "Name: ";
                    cin >> n;
                    cout << "ID: ";
                    cin >> id;
                    cout << "Pass: ";
                    cin >> ps;
                    p.set_name(n);
                    p.set_id(id);
                    p.set_password(ps);
                    patients[patientCount++] = p;
                    saveToFile();
                }
                else if (drChoice == 2)
                {
                    int id;
                    cout << "ID to delete: ";
                    cin >> id;
                    for (int i = 0; i < patientCount; i++)
                    {
                        if (patients[i].getId() == id)
                        {
                            for (int j = i; j < patientCount - 1; j++)
                                patients[j] = patients[j + 1];
                            patientCount--;
                            saveToFile();
                            break;
                        }
                    }
                }
                else if (drChoice == 3)
                {
                    int id;
                    cout << "Patient ID: ";
                    cin >> id;
                    Patient *p = findPatient(id);
                    if (p)
                    {
                        string d;
                        cout << "Enter Diagnosis: ";
                        cin.ignore();
                        getline(cin, d);
                        p->encrypt(d);
                        saveToFile();
                        cout << "Record updated securely." << endl;
                    }
                }
                else
                    break;
            }
        }
    }

    void patientMenu()
    {
        int id;
        string pass;
        cout << "ID: ";
        cin >> id;
        cout << "Pass: ";
        cin >> pass;
        Patient *p = findPatient(id);
        if (p && p->checkPassword(pass))
        {
            int ptChoice;
            while (true)
            {
                cout << "\n1. View History\n2. Book Visit\n3. Check Balance\n4. Logout\nChoice: ";
                cin >> ptChoice;
                if (ptChoice == 1)
                    p->view_medical_history();
                else if (ptChoice == 2)
                {
                    string s;
                    cout << "Slot: ";
                    cin >> s;
                    p->book_visit(s);
                    saveToFile();
                }
                else if (ptChoice == 3)
                    p->check_balance();
                else
                    break;
            }
        }
        else
            cout << "Failed login!" << endl;
    }
};

int main()
{
    portal_system healthPortal;
    healthPortal.run();
    return 0;
}
