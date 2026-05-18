#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

void line()
{
    cout << "================================================\n";
}

void header(string title)
{
    line();
    cout << "     >>> " << title << " <<<\n";
    line();
}

void successMsg(string msg)
{
    cout << "\n[✅] SUCCESS: " << msg << "\n";
}

void errorMsg(string msg)
{
    cout << "\n[❌] ERROR: " << msg << "\n";
}

class user
{
protected:
    string name;
    int id;
    string password;

public:
    void set_name(string n)
    {
        name = n;
    }
    void set_password(string pass)
    {
        password = pass;
    }
    void set_id(int i)
    {
        id = i;
    }
    int getId()
    {
        return id;
    }
    string getName()
    {
        return name;
    }
    string getPass()
    {
        return password;
    }
    bool checkPassword(string pass)
    {
        return password == pass;
    }
};

class MedicalRecord
{
protected:
    int key[2][2] = {{1, 2}, {3, 5}};
    int inv_key[2][2] = {{-5, 2}, {3, -1}};
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
        for (size_t j = 0; j < word.size() / 2; j++)
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
            cout << "⚠️ No medical records found for this patient.\n";
            return;
        }
        cout << "📜 Medical History: ";
        for (size_t j = 0; j < encryptedData.size() / 2; j++)
        {
            int e1 = encryptedData[2 * j];
            int e2 = encryptedData[2 * j + 1];
            int r1 = inv_key[0][0] * e1 + inv_key[0][1] * e2;
            int r2 = inv_key[1][0] * e1 + inv_key[1][1] * e2;
            cout << numToChar(r1) << numToChar(r2);
        }
        cout << endl;
    }

    vector<int> &getEncryptedData()
    {
        return encryptedData;
    }
    void setEncryptedData(vector<int> data)
    {
        encryptedData = data;
    }
};

class Patient : public user, public MedicalRecord
{
private:
    string upcoming_visit = "None";
    int appointmentBalance = 0;
    string accountType = "medical";

public:
    void setAccountType(string t) { accountType = t; }
    string getAccountType()
    {
        return accountType;
    }
    void set_visit(string v)
    {
        upcoming_visit = v;
    }
    void set_balance(int b)
    {
        appointmentBalance = b;
    }
    string getVisit()
    {
        return upcoming_visit;
    }
    int getBalance()
    {
        return appointmentBalance;
    }

    void changePassword()
    {
        string newPass;
        cout << "🔐 Enter your new password: ";
        cin >> newPass;
        set_password(newPass);
        successMsg("Password updated successfully!");
    }

    void view_medical_history()
    {
        decryptAndShow();
    }

    void check_balance()
    {
        line();
        cout << "👤 Patient: " << name << endl;
        cout << "📅 Upcoming Visit: " << upcoming_visit << endl;
        cout << "💳 Total Appointments: " << appointmentBalance << endl;
        line();
    }

    void book_visit(string slot)
    {
        if (accountType == "inquiry")
        {
            errorMsg("Inquiry accounts are not authorized to book appointments.");
            return;
        }
        upcoming_visit = slot;
        appointmentBalance++;
        successMsg("Your visit has been scheduled for " + slot);
    }
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
        errorMsg("Access Denied! Attempt " + to_string(loginAttempts) + "/3");
        if (loginAttempts >= 3)
        {
            errorMsg("Critical Security Lockout. Terminating...");
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
            if (patients[i].getId() == id)
                return &patients[i];
        return nullptr;
    }

    void saveToFile()
    {
        ofstream outFile("database.txt");
        outFile << patientCount << endl;
        for (int i = 0; i < patientCount; i++)
        {
            outFile << patients[i].getName() << " " << patients[i].getId() << " "
                    << patients[i].getPass() << " " << patients[i].getVisit() << " "
                    << patients[i].getBalance() << " " << patients[i].getAccountType() << " ";
            vector<int> data = patients[i].getEncryptedData();
            outFile << data.size() << " ";
            for (int val : data)
                outFile << val << " ";
            outFile << endl;
        }
    }

    void loadFromFile()
    {
        ifstream inFile("database.txt");
        if (!(inFile >> patientCount))
            return;
        for (int i = 0; i < patientCount; i++)
        {
            string n, p, v, type;
            int id, bal, size;
            inFile >> n >> id >> p >> v >> bal >> type >> size;
            patients[i].set_name(n);
            patients[i].set_id(id);
            patients[i].set_password(p);
            patients[i].set_visit(v);
            patients[i].set_balance(bal);
            patients[i].setAccountType(type);
            vector<int> data(size);
            for (int j = 0; j < size; j++)
                inFile >> data[j];
            patients[i].setEncryptedData(data);
        }
    }

public:
    portal_system()
    {
        loadFromFile();
    }

    void run()
    {
        int id;
        string pass;
        header("🏥 SMART MEDICAL PORTAL SYSTEM");

        while (true)
        {
            cout << "\n🔑 LOGIN SECTION\n";
            cout << "Enter ID (-1 to exit): ";
            if (!(cin >> id))
            { // Handle non-numeric input
                cin.clear();
                cin.ignore(1000, '\n');
                errorMsg("Invalid input. Please enter a number.");
                continue;
            }

            if (id == -1)
            {
                saveToFile();
                cout << "\nSaving secure data... Goodbye 👋\n";
                break;
            }

            cout << "Enter Password: ";
            cin >> pass;

            if (id == 0)
            {
                if (admin.login(pass))
                {
                    doctorActions();
                }
            }
            else
            {
                Patient *p = findPatient(id);
                if (!p)
                {
                    errorMsg("Patient record not found!");
                    continue;
                }
                if (p->checkPassword(pass))
                {
                    patientActions(p);
                }
                else
                {
                    errorMsg("Invalid patient password.");
                }
            }
        }
    }

    void doctorActions()
    {
        int ch;
        while (true)
        {
            header("👨‍⚕️ DOCTOR CONTROL PANEL");
            cout << "1. ➕ Add Patient\n";
            cout << "2. 🗑 Delete Patient\n";
            cout << "3. 📝 Update Medical History\n";
            cout << "4. 🔍 View Medical Records\n";
            cout << "5. 🚪 Logout\n";
            cout << "Choice: ";
            cin >> ch;

            if (ch == 1)
            {
                string n, ps, type;
                int id;
                cout << "Name : ";
                cin.ignore();
                cin >> n;
                cout << "New ID: ";
                cin >> id;
                if (findPatient(id))
                {
                    errorMsg("ID already exists!");
                    continue;
                }
                cout << "Password: ";
                cin >> ps;
                cout << "Type (medical/inquiry): ";
                cin >> type;

                Patient p;
                p.set_name(n);
                p.set_id(id);
                p.set_password(ps);
                p.setAccountType(type);
                patients[patientCount++] = p;
                saveToFile();
                successMsg("Patient profile created.");
            }
            else if (ch == 2)
            {
                int id;
                cout << "Enter ID to delete: ";
                cin >> id;
                bool found = false;
                for (int i = 0; i < patientCount; i++)
                {
                    if (patients[i].getId() == id)
                    {
                        for (int j = i; j < patientCount - 1; j++)
                            patients[j] = patients[j + 1];
                        patientCount--;
                        found = true;
                        successMsg("Record purged.");
                        break;
                    }
                }
                if (!found)
                    errorMsg("ID not found.");
            }
            else if (ch == 3)
            {
                int id;
                cout << "Enter Patient ID: ";
                cin >> id;
                Patient *p = findPatient(id);
                if (p)
                {
                    string diag;
                    cout << "Enter New Diagnosis: ";
                    cin.ignore();
                    getline(cin, diag);
                    p->encrypt(diag);
                    successMsg("Medical record updated and encrypted.");
                    saveToFile();
                }
                else
                    errorMsg("Patient not found.");
            }
            else if (ch == 4)
            {
                int id;
                cout << "Enter Patient ID: ";
                cin >> id;
                Patient *p = findPatient(id);
                if (p)
                   (*p).view_medical_history();
                else
                    errorMsg("Patient not found.");
            }
            else
                break;
        }
    }

    void patientActions(Patient *p)
    {
        int ch;
        while (true)
        {
            header("👤 PATIENT DASHBOARD: " + p->getName());
            cout << "1. 📜 View My Medical History\n";
            cout << "2. 📅 Book Appointment\n";
            cout << "3. 💰 Check My Balance/Status\n";
            cout << "4. 🔐 Change Password\n";
            cout << "5. 🚪 Logout\n";
            cout << "Choice: ";
            cin >> ch;

            if (ch == 1)
                p->view_medical_history();
            else if (ch == 2)
            {
                if (p->getAccountType() == "inquiry")
                {
                    errorMsg("Account type 'inquiry' cannot book visits.");
                    continue;
                }
                cout << "\nAvailable Slots:\n";
                cout << "[1] 09:00 AM   [2] 10:00 AM   [3] 11:00 AM\n";
                cout << "[4] 12:00 PM   [5] 01:00 PM   [6] 02:00 PM\n";
                cout << "Select Slot Number: ";
                int slot;
                cin >> slot;
                string times[] = {"0", "9AM", "10AM", "11AM", "12PM", "1PM", "2PM"};
                if (slot >= 1 && slot <= 6)
                {
                    p->book_visit(times[slot]);
                    saveToFile();
                }
                else
                    errorMsg("Invalid slot selection.");
            }
            else if (ch == 3)
                p->check_balance();
            else if (ch == 4)
            {
                p->changePassword();
                saveToFile();
            }
            else
                break;
        }
    }
};

int main()
{
    portal_system system;
    system.run();
    return 0;
}
