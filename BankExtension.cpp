#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

const string ClientsFile = "BankClient.txt";
const string Seperator = " || ";

struct stClient
{
    string AccNumber;
    string PinCode;
    string ClientName;
    string Phone;
    float AccBalance = 0;

    bool MarkToDelete = false;
};

// ---------------------------------;
// Forward Declarations:
// ---------------------------------;
void TransactionMenuScreen();
void MainMenuScreen();
vector<stClient> LoadClientsDataFromFile(const string&);
bool ClientExistsByAccountNumber(const string& AccountNumber, const string& FileName);


vector<string> _Split(const string& str)
{
    size_t pos = 0, prev = 0;
    vector<string> vClient;
    string word = "";

    while ((pos = str.find(Seperator, prev)) != string::npos)
    {
        word = str.substr(prev, pos - prev);

        if (!word.empty())
            vClient.push_back(word);

        prev = pos + Seperator.length();
    }

    string lastWord = str.substr(prev);
    if (!lastWord.empty())
        vClient.push_back(lastWord);

    return vClient;
}

string ConvertClientDataToLine(const stClient& Client)
{
    string ClientData = "";

    ClientData += Client.AccNumber + Seperator;
    ClientData += Client.PinCode + Seperator;
    ClientData += Client.ClientName + Seperator;
    ClientData += Client.Phone + Seperator;
    ClientData += to_string(Client.AccBalance);

    return ClientData;
}

stClient ConvertLineToClientRecord(const string& Line)
{
    stClient Client;
    vector<string> vClient = _Split(Line);

    if (vClient.size() < 5) return Client;

    Client.AccNumber = vClient[0];
    Client.PinCode = vClient[1];
    Client.ClientName = vClient[2];
    Client.Phone = vClient[3];
    Client.AccBalance = stof(vClient[4]);

    return Client;
}

// ---------------;
// File Operations:
// ---------------;
bool ClientExistsByAccountNumber(const string& AccountNumber, const string& FileName)
{
    fstream myFile;
    myFile.open(FileName, ios::in);

    if (myFile.is_open())
    {
        string Line;
        stClient Client;
        while (getline(myFile, Line))
        {
            Client = ConvertLineToClientRecord(Line);
            if (Client.AccNumber == AccountNumber)
            {
                myFile.close();
                return true;
            }
        }
        myFile.close();
    }
    return false;
}

stClient ReadNewClient()
{
    stClient Client;

    cout << "\nEnter Account Number :   ";
    cin >> Client.AccNumber;

    while (ClientExistsByAccountNumber(Client.AccNumber, ClientsFile))
    {
        cout << "\nClient with [" << Client.AccNumber << "] already exists, Enter another Account Number:   ";
        cin >> Client.AccNumber;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }


    cout << "Enter Pin Code       :   ";
    getline(cin, Client.PinCode);

    cout << "Enter Client Name    :   ";
    getline(cin, Client.ClientName);

    cout << "Enter Phone          :   ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance:   ";
    cin >> Client.AccBalance;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return Client;
}

void SaveClientDataToFile(const string& Line, const string& FileName)
{
    fstream myFile;
    myFile.open(FileName, ios::out | ios::app);

    if (myFile.is_open())
    {
        myFile << Line << "\n";
        myFile.close();
    }
}

void AddNewClient()
{
    stClient Client = ReadNewClient();
    SaveClientDataToFile(ConvertClientDataToLine(Client), ClientsFile);
}

void AddNewClients()
{
    char Answer = 'N';

    do
    {
        cout << "\nAdd New Client:\n";
        AddNewClient();

        cout << "\nThe Client Saved Successfully, Do You Want To Add More Clients? (Y/N): ";
        cin >> Answer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

    } while (toupper(Answer) == 'Y');
}

void AddNewClientsScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Add New Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    AddNewClients();
}

vector<stClient> LoadClientsDataFromFile(const string& FileName)
{
    fstream myFile;
    vector<stClient> vClients;
    myFile.open(FileName, ios::in);

    if (myFile.is_open())
    {
        string Line;
        stClient Client;
        while (getline(myFile, Line))
        {
            Client = ConvertLineToClientRecord(Line);
            if (!Client.AccNumber.empty())
                vClients.push_back(Client);
        }
        myFile.close();
    }

    return vClients;
}

void SaveAllClientsInFile(const vector<stClient>& vClient, const string& FileName)
{
    fstream myFile;
    myFile.open(FileName, ios::out);

    if (myFile.is_open())
    {
        for (const stClient& C : vClient)
            if (!C.MarkToDelete)
                myFile << ConvertClientDataToLine(C) << endl;

        myFile.close();
    }
}

void PrintClientLine(const stClient& Client)
{
    cout << "| " << setw(11) << left << Client.AccNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(40) << left << Client.ClientName;
    cout << "| " << setw(15) << left << Client.Phone;
    cout << "| " << setw(17) << left << Client.AccBalance << " |\n";
}

void AllClientScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t\t Clients Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    cout << "\t\t\tNumber Of Clients = " << vClients.size() << " Client(s)";
    cout << "\n--------------------------------------------------------------------------------------------------\n";
    cout << "| " << setw(11) << left << "Acc Number";
    cout << "| " << setw(10) << left << "Pin Code";
    cout << "| " << setw(40) << left << "Client Name";
    cout << "| " << setw(15) << left << "Phone";
    cout << "| " << setw(17) << left << "Account Balance" << " |\n";
    cout << "--------------------------------------------------------------------------------------------------\n";

    if (vClients.empty())
        cout << "\t\t\tThe Clients System Is Empty!\n";
    else
    {
        for (const stClient& C : vClients)
            PrintClientLine(C);
    }
    cout << "--------------------------------------------------------------------------------------------------\n";
}

string ReadAccNumber()
{
    string AccNumber;
    cout << "Please Enter The Account Number: ";
    cin >> AccNumber;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return AccNumber;
}

void PrintClientRecord(const stClient& Client)
{
    cout << "\n---------------------------------------------";
    cout << "\n Account Number  :   " << Client.AccNumber;
    cout << "\n Pin Code        :   " << Client.PinCode;
    cout << "\n Client Name     :   " << Client.ClientName;
    cout << "\n Phone           :   " << Client.Phone;
    cout << "\n Account Balance :   " << Client.AccBalance;
    cout << "\n---------------------------------------------\n";
}

bool FindClientByAccNumber(const vector<stClient>& vClients, const string& AccountNumber, stClient& Client)
{
    for (const stClient& C : vClients)
        if (C.AccNumber == AccountNumber)
        {
            Client = C;
            return true;
        }

    return false;
}

void FindClientScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Find Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string AccNumber = ReadAccNumber();
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    stClient Client;

    if (FindClientByAccNumber(vClients, AccNumber, Client))
        PrintClientRecord(Client);
    else
        cout << "\nThe Client With Account Number [" << AccNumber << "] Is Not Found!\n";
}

void UpdateClientRecord(const string& AccountNumber, stClient& Client)
{
    Client.AccNumber = AccountNumber;

    cout << "\nEnter Pin Code     :   ";
    getline(cin, Client.PinCode);

    cout << "Enter Client Name    :   ";
    getline(cin, Client.ClientName);

    cout << "Enter Phone          :   ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance:   ";
    cin >> Client.AccBalance;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void UpdateClientScreenByAccNumber()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Update Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string AccountNumber = ReadAccNumber();
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    bool found = false;

    for (stClient& C : vClients)
    {
        if (C.AccNumber == AccountNumber)
        {
            found = true;
            PrintClientRecord(C);

            char Answer = 'N';
            cout << "\nAre You Sure You Want To Update This Client? (Y/N): ";
            cin >> Answer;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (toupper(Answer) == 'Y')
            {
                UpdateClientRecord(AccountNumber, C);
                SaveAllClientsInFile(vClients, ClientsFile);
                cout << "\nThe client updated successfully.\n";
            }
            else
                cout << "\nUpdate Operation Cancelled.\n";

            break;
        }
    }

    if (!found)
        cout << "\nThe Client With Account Number [" << AccountNumber << "] Is Not Found!\n";
}

void DeleteClientScreenByAccNumber()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Delete Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();
    bool found = false;

    for (stClient& C : vClients)
    {
        if (C.AccNumber == AccountNumber)
        {
            found = true;
            PrintClientRecord(C);

            char Answer = 'N';
            cout << "\nAre You Sure You Want To Delete This Client? (Y/N): ";
            cin >> Answer;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (toupper(Answer) == 'Y')
            {
                C.MarkToDelete = true;
                SaveAllClientsInFile(vClients, ClientsFile);
                cout << "\nThe Client Deleted Successfully!\n";
            }
            else
                cout << "\nDelete Operation Cancelled.\n";

            break;
        }
    }

    if (!found)
        cout << "\nThe Client With Account Number [" << AccountNumber << "] Is Not Found!\n";
}

void GoBackMainMenu()
{
    cout << "\nEnter Any Key To Go To Main Menu... ";
    system("pause>0");
}

void _Exit()
{
    system("cls");
    cout << "\n--------------------------------------------\n";
    cout << "\t\t Program Ends :-)";
    cout << "\n--------------------------------------------\n";
}

enum enMainMenuOption { ClientScreen = 1, AddClient, FindClient, UpdateClient, DeleteClient, TransactionScreen, eExit };
enMainMenuOption ReadMainMenuOption()
{
    short Option;
    cout << "Enter Your Choice [1 to 7]: ";
    cin >> Option;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return enMainMenuOption(Option);
}

void PerformMainMenuOption(enMainMenuOption Option)
{
    switch (Option)
    {
    case enMainMenuOption::ClientScreen:
        AllClientScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::AddClient:
        AddNewClientsScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::FindClient:
        FindClientScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::UpdateClient:
        UpdateClientScreenByAccNumber();
        GoBackMainMenu();
        break;
    case enMainMenuOption::DeleteClient:
        DeleteClientScreenByAccNumber();
        GoBackMainMenu();
        break;
    case enMainMenuOption::TransactionScreen:
        TransactionMenuScreen();
        break;
    case enMainMenuOption::eExit:
        _Exit();
        break;
    }
}

void MainMenuScreen()
{
    system("cls");
    cout << "\n====================================================\n";
    cout << "\t\t Main Menu Screen";
    cout << "\n====================================================\n";
    cout << "\t [1] Show Clients List!\n";
    cout << "\t [2] Add New Client!\n";
    cout << "\t [3] Find Client!\n";
    cout << "\t [4] Update Client!\n";
    cout << "\t [5] Delete Client!\n";
    cout << "\t [6] Transaction Menu Screen\n";
    cout << "\t [7] Exit!\n";
    cout << "====================================================\n";
}

// ------------------------------------------------------------------;
// Transactions Operations:
// ------------------------------------------------------------------;

bool DepositBalanceToClientByAccountNumber(vector<stClient>& vClients, const string& AccountNumber, double Amount)
{
    for (stClient& C : vClients)
    {
        if (C.AccNumber == AccountNumber)
        {
            C.AccBalance += Amount;
            SaveAllClientsInFile(vClients, ClientsFile);
            cout << "\n\nDone Successfully. New balance is: " << C.AccBalance;
            return true;
        }
    }
    return false;
}

//-----------------;
//- DepositScreen -;
//-----------------;
void DepositScreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\t Deposit Screen";
    cout << "\n-----------------------------------\n";

    stClient Client;
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();

    while (!FindClientByAccNumber(vClients, AccountNumber, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadAccNumber();
    }

    cout << "\nThe Following Are Client Details: \n";
    PrintClientRecord(Client);

    double Amount = 0;
    cout << "\nEnter The Amount Deposit:   ";
    cin >> Amount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    char Answer = 'N';
    cout << "\nAre You Sure You Want To Perform This Transaction [Y/N]?   ";
    cin >> Answer;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (toupper(Answer) == 'Y')
        DepositBalanceToClientByAccountNumber(vClients, AccountNumber, Amount);
    else
        cout << "\nDeposit Operation Cancelled.\n";
}

bool WithdrawBalanceFromClientByAccountNumber(vector<stClient>& vClients, const string& AccountNumber, double Amount)
{
    return DepositBalanceToClientByAccountNumber(vClients, AccountNumber, -Amount);
}

void WithdrawScreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\t Withdraw Screen";
    cout << "\n-----------------------------------\n";

    stClient Client;
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();

    while (!FindClientByAccNumber(vClients, AccountNumber, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadAccNumber();
    }

    cout << "\nThe Following Are Client Details: \n";
    PrintClientRecord(Client);

    double Amount = 0;
    cout << "\nEnter The Amount Withdraw:   ";
    cin >> Amount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (Amount > Client.AccBalance)
    {
        cout << "\nThe Withdraw Amount Exceeds Your Balance! Your Balance [ " << Client.AccBalance << " ]\n";
        cout << "Enter The Amount Withdraw:   ";
        cin >> Amount;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    char Answer = 'N';
    cout << "\nAre You Sure You Want To Perform This Transaction [Y/N]?   ";
    cin >> Answer;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (toupper(Answer) == 'Y')
        WithdrawBalanceFromClientByAccountNumber(vClients, AccountNumber, Amount);
    else
        cout << "\nWithdraw Operation Cancelled.\n";
}

double GetTotalBalance(const vector<stClient>& vClients)
{
    double TotalBalance = 0;
    for (const stClient& C : vClients)
        TotalBalance += C.AccBalance;

    return TotalBalance;
}

void ClientBalance(const stClient& Client)
{
    cout << "| " << setw(11) << left << Client.AccNumber;
    cout << "| " << setw(40) << left << Client.ClientName;
    cout << "| " << setw(17) << left << Client.AccBalance << " |\n";
}

void AllClientBalanceScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Total Balance Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    cout << "\t\t\tNumber Of Clients = " << vClients.size() << " Client(s)";
    cout << "\n-------------------------------------------------------------------------\n";
    cout << "| " << setw(11) << left << "Acc Number";
    cout << "| " << setw(40) << left << "Client Name";
    cout << "| " << setw(17) << left << "Account Balance" << " |\n";
    cout << "\n-------------------------------------------------------------------------\n";

    if (vClients.empty())
        cout << "\t\t\tThe Clients System Is Empty!\n";
    else
    {
        for (const stClient& C : vClients)
            ClientBalance(C);
    }
    cout << "\n-------------------------------------------------------------------------\n";
    cout << "\t\t\t\t Total Balance: " << GetTotalBalance(vClients) << "\n";
    cout << "-------------------------------------------------------------------------\n";
}

enum enTransactionMenuOption { Deposit = 1, Withdraw = 2, TotalBalance = 3, MainMenu = 4 };

enTransactionMenuOption ReadTransactionMenuOption()
{
    short Option;
    cout << "Enter Your Choice [1 to 4]: ";
    cin >> Option;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return enTransactionMenuOption(Option);
}

void GoBackToTransactionMenu()
{
    cout << "\nEnter Any Key To Go To Transaction Menu... \n";
    system("pause>0");
}

void PerformTransactionMenuOption(enTransactionMenuOption Option)
{
    switch (Option)
    {
    case enTransactionMenuOption::Deposit:
        DepositScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionMenuOption::Withdraw:
        WithdrawScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionMenuOption::TotalBalance:
        AllClientBalanceScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionMenuOption::MainMenu:
        break;
    }
}

void TransactionMenuScreen()
{
    enTransactionMenuOption Option;
    do
    {
        system("cls");
        cout << "\n====================================================\n";
        cout << "\t\t Transaction Menu Screen";
        cout << "\n====================================================\n";
        cout << "\t [1] Deposit\n";
        cout << "\t [2] Withdraw\n";
        cout << "\t [3] Total Balance\n";
        cout << "\t [4] Main Menu Screen\n";
        cout << "====================================================\n";

        Option = ReadTransactionMenuOption();
        PerformTransactionMenuOption(Option);

    } while (Option != enTransactionMenuOption::MainMenu);
}

int main()
{
    enMainMenuOption Option;

    do
    {
        MainMenuScreen();
        Option = ReadMainMenuOption();
        PerformMainMenuOption(Option);

    } while (Option != enMainMenuOption::eExit);

    return 0;
}







//! Cannot open visual studio code file --- (LNK1168);
// ctrl + shift + esc ---> fileName ---> rightClick -> end;
// Reset visual studio code -> BuildWindow ---> BuildSolution Then -> RebuildSolution;



