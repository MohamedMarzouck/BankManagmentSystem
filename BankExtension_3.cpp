#pragma warning(disable : 4996)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

using namespace std;

const string ClientsFile = "BankClient.txt";
const string UsersFile = "BankUsers.txt";
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

struct stUser
{
    string UserName;
    string Password;
    int permissions = 0;
    bool MarkToDelete = false;
};

struct stTransferLogRecord
{
    string DateTime;
    string SourceAccNumber;
    string DestinationAccNumber;
    string UserName;
    float  Amount = 0;
    float  SourceBalanceBefor = 0;
    float  SourceBalanceAfter = 0;
    float  DestinationBalanceBefor = 0;
    float  DestinationBalanceAfter = 0;
};

enum enMainMenuOption { eClientList = 1, eAddClient = 2, eFindClient = 3, eUpdateClient = 4, eDeleteClient = 5,
                        eTransactionScreen = 6,eManageUsers = 7, eLogOut = 8};

enum enTransactionMenuOption { Deposit = 1, Withdraw, TotalBalance, TransferBalance, TransferLog, MainMenu };

enum enUsersMenuOption { UsersList = 1, AddUser = 2, FindUser = 3, UpdateUser = 4, DeleteUsser = 5, pMainMenu = 6 };

enum enUserPermissions { All = -1, pUsersList = 1, pNewUser = 2, pFindUser = 4, pUpdateUser = 8,
                         pDeleteUser = 16, pUserTransaction = 32, pManageUsers = 64 };


stUser CurrentUser;

// --------------------;
// Forward Declarations:
// --------------------;
void LogIn();
void MainMenuScreen();
void UsersScreen();
void TransactionMenuScreen();
vector<stClient> LoadClientsDataFromFile(const string&);
vector<stUser> LoadUsersDataFromFile(const string& FileName);
bool ClientExistsByAccountNumber(const string& AccountNumber, const string& FileName);
void SaveTransferLogToFile(const stTransferLogRecord& , const string& );
string GetSystemDateTimeString();


vector<string> _Split(const string& str, const string& Sepetator = " || ")
{
    size_t pos = 0, prev = 0;
    vector<string> vData;
    string word = "";

    while ((pos = str.find(Seperator, prev)) != string::npos)
    {
        word = str.substr(prev, pos - prev);
        if (!word.empty())
            vData.push_back(word);
        prev = pos + Seperator.length();
    }

    string lastWord = str.substr(prev);
    if (!lastWord.empty())
        vData.push_back(lastWord);

    return vData;
}

short ReadOption(short From, short To)
{
    short Option;
    do
    {
        cout << "Enter Your Choice From [" << From << " ] To [" << To << "]:   ";
        cin >> Option;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            Option = From - 1; //******************************;
        }

    } while (Option < From || Option > To);
    return Option;
}

void AccessDeniedMsg()
{
    system("cls");
    cout << "\n-------------------------------------\n";
    cout << "Access Denied, \n";
    cout << "You Don't Have Permission To Do This!\n";
    cout << "Please Contact The Admin\n";
    cout << "-------------------------------------\n";
    system("pause>0");
}

bool CheckAccessPermissions(enUserPermissions Permission)
{
    if (CurrentUser.permissions == enUserPermissions::All)
        return true;

    if ((CurrentUser.permissions & Permission) == Permission)
        return true;

    return false;
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
    }

    cout << "Enter Pin Code       :   ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, Client.PinCode);

    cout << "Enter Client Name    :   ";
    getline(cin, Client.ClientName);

    cout << "Enter Phone          :   ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance:   ";
    cin >> Client.AccBalance;

    return Client;
}

void SaveClientDataToFile(const string& Line, const string& FileName)
{
    fstream myFile;
    myFile.open(ClientsFile, ios::out | ios::app);

    if (myFile.is_open())
    {
        myFile << Line << "\n";
        myFile.close();
    }
}

void AddNewClients()
{
    stClient Client;
    char Answer = 'N';

    do
    {
        cout << "\nAdd New Client:\n";
        Client = ReadNewClient();
        SaveClientDataToFile(ConvertClientDataToLine(Client), ClientsFile);

        cout << "\nClient Saved Successfully, Do You Want To Add More Clients? (Y/N): ";
        cin >> Answer;

    } while (toupper(Answer) == 'Y');
}

void AddNewClientsScreen()
{
    if (!CheckAccessPermissions(enUserPermissions::pNewUser))
    {
        AccessDeniedMsg();
        return;
    }

    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Add New Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    AddNewClients();
}

void PrintClientLine(const stClient& Client)
{
    cout << "| " << setw(11) << left << Client.AccNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(30) << left << Client.ClientName;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccBalance << " |\n";
}

void AllClientsScreen()
{
    if (!CheckAccessPermissions(enUserPermissions::pUsersList))
    {
        AccessDeniedMsg();
        return;
    }

    system("cls");
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    cout << "\n\t\t----------------------------------------------------------------------------------\n";
    cout << "\t\t\t\t\t Clients Screen ";
    cout << "\n\t\t----------------------------------------------------------------------------------\n";
    cout << "\t\t\tNumber Of Clients = " << vClients.size() << " Client(s)";
    cout << "\n--------------------------------------------------------------------------------------------------\n";
    cout << "| " << setw(11) << left << "Acc Number";
    cout << "| " << setw(10) << left << "Pin Code";
    cout << "| " << setw(30) << left << "Client Name";
    cout << "| " << setw(12) << left << "Phone";
    cout << "| " << setw(12) << left << "Balance" << " |\n";
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
    if (!CheckAccessPermissions(enUserPermissions::pFindUser))
    {
        AccessDeniedMsg();
        return;
    }

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
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\nEnter Pin Code       :   ";
    getline(cin, Client.PinCode);

    cout << "Enter Client Name    :   ";
    getline(cin, Client.ClientName);

    cout << "Enter Phone          :   ";
    getline(cin, Client.Phone);

    cout << "Enter Account Balance:   ";
    cin >> Client.AccBalance;
}

void UpdateClientScreenByAccNumber()
{
    if (!CheckAccessPermissions(enUserPermissions::pUpdateUser))
    {
        AccessDeniedMsg();
        return;
    }

    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Update Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string AccountNumber = ReadAccNumber();
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    stClient Client;

    for (stClient& C : vClients)
    {
        if (C.AccNumber == AccountNumber)
        {
            PrintClientRecord(Client);

            char Answer = 'N';
            cout << "\nAre You Sure You Want To Update This Client? (Y/N): ";
            cin >> Answer;

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
        else
            cout << "\nThe Client With Account Number [" << AccountNumber << "] Is Not Found!\n";
    }
}
            

void DeleteClientScreenByAccNumber()
{
    if (!CheckAccessPermissions(enUserPermissions::pDeleteUser))
    {
        AccessDeniedMsg();
        return;
    }

    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Delete Client Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();
    stClient Client;

    for (stClient& C : vClients)
    {
        if (C.AccNumber == AccountNumber)
        {
            PrintClientRecord(Client);

            char Answer = 'N';
            cout << "\nAre You Sure You Want To Delete This Client? (Y/N): ";
            cin >> Answer;

            if (toupper(Answer) == 'Y')
            {
                C.MarkToDelete = true;
                SaveAllClientsInFile(vClients, ClientsFile);
                cout << "\nThe client deleted successfully.\n";
            }
            else
                cout << "\nUpdate Operation Cancelled.\n";

            break;
        }
        else
            cout << "\nThe Client With Account Number [" << AccountNumber << "] Is Not Found!\n";
    }
}

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

void DepositScreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\t Deposit Screen";
    cout << "\n-----------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();
    stClient Client;

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

    char Answer = 'N';
    cout << "\nAre You Sure You Want To Perform This Transaction [Y/N]?   ";
    cin >> Answer;

    if (toupper(Answer) == 'Y')
        DepositBalanceToClientByAccountNumber(vClients, AccountNumber, Amount);
    else
        cout << "\nDeposit Operation Cancelled.\n";
}

void WithdrawScreen()
{
    system("cls");
    cout << "\n-----------------------------------\n";
    cout << "\t Withdraw Screen";
    cout << "\n-----------------------------------\n";

    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    string AccountNumber = ReadAccNumber();
    stClient Client;

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

    while (Amount > Client.AccBalance)
    {
        cout << "\nThe Withdraw Amount Exceeds Your Balance! Your Balance [ " << Client.AccBalance << " ]\n";
        cout << "Enter The Amount Withdraw:   ";
        cin >> Amount;
    }

    char Answer = 'N';
    cout << "\nAre You Sure You Want To Perform This Transaction [Y/N]?   ";
    cin >> Answer;

    if (toupper(Answer) == 'Y')
        DepositBalanceToClientByAccountNumber(vClients, AccountNumber, -Amount);
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

void AllClientBalanceScreen()
{
    system("cls");
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    cout << "\n\t\t-------------------------------------------------------------------\n";
    cout << "\t\t\t\t Total Balance Screen ";
    cout << "\n\t\t-------------------------------------------------------------------\n";
    cout << "\t\t\tNumber Of Clients = " << vClients.size() << " Client(s)";
    cout << "\n--------------------------------------------------------------------------------------\n";
    cout << "| " << setw(15) << left << "Acc Number";
    cout << "| " << setw(40) << left << "Client Name";
    cout << "| " << setw(15) << left << "Account Balance" << " |\n";
    cout << "--------------------------------------------------------------------------------------\n";

    if (vClients.empty())
        cout << "\t\t\tThe Clients System Is Empty!\n";
    else
    {
        for (const stClient& C : vClients)
        {
            cout << "| " << setw(15) << left << C.AccNumber;
            cout << "| " << setw(40) << left << C.ClientName;
            cout << "| " << setw(15) << left << C.AccBalance << " |\n";
        }
    }
    cout << "--------------------------------------------------------------------------------------\n";
    cout << "\t\t\t\t Total Balance: " << GetTotalBalance(vClients) << "\n";
    cout << "--------------------------------------------------------------------------------------\n";
}


//-------------------------------------------- Transfer ----------------------------------------------;
//----------------------------------------------------------------------------------------------------;
void TransferAccNumber(const vector<stClient>& vClients, stClient& FromClient, stClient& ToClient)
{
    cout << "\nEnter Account Number To Transfer From:   ";
    cin >> FromClient.AccNumber;
    while (!FindClientByAccNumber(vClients, FromClient.AccNumber, FromClient))
    {
        cout << "\nThe Client With Account Number [ " << FromClient.AccNumber << " ] Is Not Found.";
        cout << "\nEnter Account Number To Transfer From:   ";
        cin >> FromClient.AccNumber;
    }
    cout << "------------------------------------------";
    cout << "\nAccount Number :    " << FromClient.AccNumber;
    cout << "\nName           :    " << FromClient.ClientName;
    cout << "\nAccount Balance:    " << FromClient.AccBalance;
    cout << "\n------------------------------------------";


    cout << "\n\nEnter Account Number To Transfer To:   ";
    cin >> ToClient.AccNumber;
    while (!FindClientByAccNumber(vClients, ToClient.AccNumber, ToClient) || ToClient.AccNumber == FromClient.AccNumber)
    {
        if (ToClient.AccNumber == FromClient.AccNumber)
            cout << "\nYou Cannot Transfer To The Same Account.";
        else
            cout << "\nThe Client With Account Number [ " << ToClient.AccNumber << " ] Is Not Found.";

        cout << "\nEnter Account Number To Transfer To:   ";
        cin >> ToClient.AccNumber;
    }
    cout << "------------------------------------------";
    cout << "\nAccount Number :    " << ToClient.AccNumber;
    cout << "\nName           :    " << ToClient.ClientName;
    cout << "\n------------------------------------------";
}

float GetTransferBalance(const stClient& FromClient)
{
    if (FromClient.AccBalance < 10)
    {
        cout << "\nYour Available Balance Is: [ " << FromClient.AccBalance << " ]. \n";
        return 0;
    }

    float Amount;
    cout << "\n\nEnter Transfer Amount Geater Than 10:   ";
    cin >> Amount;
    while (Amount < 10 || Amount > FromClient.AccBalance)                   //! while (true);
    {
        cout << "\nInvalid Amount!, Your Available Balance Is: [ " << FromClient.AccBalance << " ]. \n";
        cout << "\nEnter Another Amount Between [ 10, " << FromClient.AccBalance << " ]:   ";
        cin >> Amount;
    }

    return Amount;
}

void TransferScreen()
{
    system("cls");
    cout << "\n\t\t----------------------------------------------";
    cout << "\n\t\t\t\t Transfer Screen";
    cout << "\n\t\t----------------------------------------------";

    stClient FromClient, ToClient;
    vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    TransferAccNumber(vClients, FromClient, ToClient);
    float Amount = GetTransferBalance(FromClient);

    if (Amount == 0) return;

    short UpdatedCount = 0;
    char Confirm = 'N';
    cout << "\nAre You Sure To Transfer This Amount?   ";
    cin >> Confirm;

    if (toupper(Confirm) == 'Y')
    {
        for (stClient& C : vClients)
        {
            if (C.AccNumber == FromClient.AccNumber)
            {
                C.AccBalance -= Amount;
                FromClient.AccBalance = C.AccBalance;
                UpdatedCount++;

            }
            else if (C.AccNumber == ToClient.AccNumber)
            {
                C.AccBalance += Amount;
                ToClient.AccBalance = C.AccBalance;
                UpdatedCount++;
            }

            if (UpdatedCount == 2)
                break;
        }

        SaveAllClientsInFile(vClients, ClientsFile);

        cout << "\nTransfer Done Successfully!\n";
        cout << "New Balance For [" << FromClient.AccNumber << "] Is: " << FromClient.AccBalance << "\n";
    }

    else
        cout << "\nThe Operation Cancelled.\n";


    if (UpdatedCount == 2)
    {
        stTransferLogRecord LogRecord;

        LogRecord.Amount = Amount;
        LogRecord.DateTime = GetSystemDateTimeString();
        LogRecord.SourceAccNumber = FromClient.AccNumber;
        LogRecord.SourceBalanceBefor = FromClient.AccBalance + Amount;
        LogRecord.SourceBalanceAfter = FromClient.AccBalance;

        LogRecord.DestinationAccNumber = ToClient.AccNumber;
        LogRecord.DestinationBalanceBefor = ToClient.AccBalance - Amount;
        LogRecord.DestinationBalanceAfter = ToClient.AccBalance;
        LogRecord.UserName = CurrentUser.UserName;

        SaveTransferLogToFile(LogRecord, "TransferLog.txt");
    }
}


//-------- TransferLog --------;
string GetSystemDateTimeString()
{
    time_t t = time(0);
    tm* now = localtime(&t);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y - %H:%M:%S", now);

    return string(buffer);
}

string ConvertTransferLogToLine(const stTransferLogRecord& LogRecord, const string& Seperator)
{
    string LogLine = "";

    LogLine += LogRecord.DateTime + Seperator;
    LogLine += LogRecord.SourceAccNumber + Seperator;
    LogLine += to_string(LogRecord.SourceBalanceBefor) + Seperator;
    LogLine += to_string(LogRecord.SourceBalanceAfter) + Seperator;

    LogLine += LogRecord.DestinationAccNumber + Seperator;
    LogLine += to_string(LogRecord.DestinationBalanceBefor) + Seperator;
    LogLine += to_string(LogRecord.DestinationBalanceAfter) + Seperator;

    LogLine += to_string(LogRecord.Amount) + Seperator;
    LogLine += LogRecord.UserName;

    return LogLine;
}

void SaveTransferLogToFile(const stTransferLogRecord& LogRecord, const string& LogFileName = "TransferLog.txt")
{
    fstream MyFile;
    MyFile.open(LogFileName, ios::out | ios::app);

    if (MyFile.is_open())
    {
        string DataLine = ConvertTransferLogToLine(LogRecord, Seperator);
        MyFile << DataLine << endl;

        MyFile.close();
    }
}

stTransferLogRecord ConvertLineToTransferLogRecord(const string& Line, const string& Seperator)

{
    stTransferLogRecord LogRecord;
    vector<string> vLogData = _Split(Line, Seperator);

    if (vLogData.size() < 9) return LogRecord;

    LogRecord.DateTime = vLogData[0];
    LogRecord.SourceAccNumber = vLogData[1];
    LogRecord.SourceBalanceBefor = stof(vLogData[2]);
    LogRecord.SourceBalanceAfter = stof(vLogData[3]);

    LogRecord.DestinationAccNumber = vLogData[4];
    LogRecord.DestinationBalanceBefor = stof(vLogData[5]);
    LogRecord.DestinationBalanceAfter = stof(vLogData[6]);

    LogRecord.Amount = stod(vLogData[7]);
    LogRecord.UserName = vLogData[8];

    return LogRecord;
}

vector<stTransferLogRecord> LoadTransferLogDataFromFile(const string& LogFileName = "TransferLog.txt")
{
    vector<stTransferLogRecord> vTransferLogRecords;
    fstream MyFile;
    MyFile.open(LogFileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        stTransferLogRecord LogRecord;

        while (getline(MyFile, Line))
            if (Line != "")
            {
                LogRecord = ConvertLineToTransferLogRecord(Line, Seperator);
                vTransferLogRecords.push_back(LogRecord);
            }

        MyFile.close();
    }

    return vTransferLogRecords;
}

void PrintTransferLogRecordLine(const stTransferLogRecord& Record) {
    cout << "| " << left << setw(23) << Record.DateTime;
    cout << "| " << left << setw(8) << Record.SourceAccNumber;
    cout << "| " << left << setw(10) << Record.SourceBalanceBefor;
    cout << "| " << left << setw(10) << Record.SourceBalanceAfter;

    cout << "| " << left << setw(8) << Record.DestinationAccNumber;
    cout << "| " << left << setw(10) << Record.DestinationBalanceBefor;
    cout << "| " << left << setw(10) << Record.DestinationBalanceAfter;
    cout << "| " << left << setw(10) << Record.Amount;
    cout << "| " << left << setw(10) << Record.UserName << " |\n";
}

//--- TrancferLogScreen ---;
void ShowTransferLogScreen() 
{
    system("cls");
    vector<stTransferLogRecord> vLogs = LoadTransferLogDataFromFile();
    string Title = "\t\t\t\t\tTransfer Log List (" + to_string(vLogs.size()) + ") Record(s).";

    cout << "\n\t\t\t-------------------------------------------------------";
    cout << Title << endl;
    cout << "\n\t\t\t-------------------------------------------------------\n";
    cout << "=======================================================================================================================\n";

    cout << "| " << left << setw(23) << "Date/Time";
    cout << "| " << left << setw(8) << "s.AccN";
    cout << "| " << left << setw(8) << "s.BalBefor";
    cout << "| " << left << setw(10) << "s.BalAfter";

    cout << "| " << left << setw(8) << "d.AccN";
    cout << "| " << left << setw(8) << "d.BalBefor";
    cout << "| " << left << setw(10) << "d.BalAfter";
    cout << "| " << left << setw(10) << "Amount";
    cout << "| " << left << setw(10) << "User" << " |\n";
    cout << "=======================================================================================================================\n";

    if (vLogs.empty())
        cout << "\t\t\t\tNo Transfers Available In System!\n";
    
    else 
        for (const stTransferLogRecord& Record : vLogs) 
            PrintTransferLogRecordLine(Record);

    cout << "=======================================================================================================================\n";
}//--------------------------------------------------------------------------------------------------------------------;
//---------------------------------------------------------------------------------------------------------------------;
//---------------------------------------------------------------------------------------------------------------------;

short ReadPermissions()
{
    short Permission = 0;
    char Answer = 'N';

    cout << "\nDo You Want To Give Him Full Access? (Y/N)   ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        return enUserPermissions::All;

    cout << "\n\nDo You Want To Give Him Access To:   ";
    cout << "\n Show Client List: (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pUsersList;

    cout << "\n Add New Client  : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pNewUser;

    cout << "\n Find Client     : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pFindUser;

    cout << "\n Update Client   : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pUpdateUser;

    cout << "\n Delete Client   : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pDeleteUser;

    cout << "\n Transactions    : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pUserTransaction;

    cout << "\n Manage Users    : (Y/N)  ";
    cin >> Answer;
    if (toupper(Answer) == 'Y')
        Permission += enUserPermissions::pManageUsers;

    return Permission;
}

string ConvertUserDataToLine(const stUser& User)
{
    string UserData = "";
    UserData += User.UserName + Seperator;
    UserData += User.Password + Seperator;
    UserData += to_string(User.permissions);
    return UserData;
}

stUser ConvertLineToUserRecord(const string& Line)
{
    stUser User;
    vector<string> vUser = _Split(Line);

    if (vUser.size() < 3) return User;

    User.UserName = vUser[0];
    User.Password = vUser[1];
    User.permissions = stoi(vUser[2]);

    return User;
}

void SaveUserDataToFile(const string& Line, const string& FileName)
{
    fstream myFile;
    myFile.open(FileName, ios::out | ios::app);

    if (myFile.is_open())
    {
        myFile << Line << endl;
        myFile.close();
    }
}

vector<stUser> LoadUsersDataFromFile(const string& FileName)
{
    fstream myFile;
    vector<stUser> vUsers;
    myFile.open(FileName, ios::in);

    if (myFile.is_open())
    {
        string Line;
        while (getline(myFile, Line))
        {
            stUser User = ConvertLineToUserRecord(Line);
            if (!User.UserName.empty())
                vUsers.push_back(User);
        }

        myFile.close();
    }

    return vUsers;
}

void SaveAllUsersInFile(const vector<stUser>& vUsers, const string& FileName)
{
    fstream myFile;
    myFile.open(FileName, ios::out);

    if (myFile.is_open())
    {
        for (const stUser& U : vUsers)
            if (!U.MarkToDelete)
                myFile << ConvertUserDataToLine(U) << endl;


        myFile.close();
    }
}

bool FindUserByUserName(const vector<stUser>& vUsers, const string& UserN, stUser& User)
{
    for (const stUser& U : vUsers)
        if (U.UserName == UserN)
        {
            User = U;
            return true;
        }
    return false;
}

bool UserExistsByUserName(const string& UserN, const string& FileName) {
    vector<stUser> vUsers = LoadUsersDataFromFile(FileName);
    stUser User;


    return FindUserByUserName(vUsers, UserN, User);//******;
}

stUser ReadNewUser()
{
    stUser User;
    cout << "\nEnter Username :   ";
    cin >> User.UserName;

    while (UserExistsByUserName(User.UserName, UsersFile))
    {
        cout << "\nUser with [" << User.UserName << "] already exists, Enter another UserName:   ";
        cin >> User.UserName;
    }

    cout << "Enter Password   :   ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, User.Password);

    User.permissions = ReadPermissions();

    return User;
}

void AddNewUsers()
{
    char Answer = 'N';
    do
    {
        system("cls");
        cout << "\n\t\t-----------------------------------------\n";
        cout << "\t\t Add New Users ";
        cout << "\n\t\t-----------------------------------------\n";

        stUser User = ReadNewUser();
        SaveUserDataToFile(ConvertUserDataToLine(User), UsersFile);

        cout << "The User Saved Successfully, Do You Want To Add More Users? (Y/N)   ";
        cin >> Answer;

    } while (toupper(Answer) == 'Y');
}

void PrintUserLine(const stUser& User)
{
    cout << "| " << setw(20) << left << User.UserName;
    cout << "| " << setw(20) << left << User.Password;
    cout << "| " << setw(10) << left << User.permissions << " |\n";
}

void AllUsersScreen()
{
    system("cls");
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFile);

    cout << "\n\t\t----------------------------------------------------------\n";
    cout << "\t\t\t\t Users List Screen ";
    cout << "\n\t\t----------------------------------------------------------\n";
    cout << "\t\t\tNumber Of Users = " << vUsers.size() << " User(s)";
    cout << "\n--------------------------------------------------------------------------\n";
    cout << "| " << setw(20) << left << "UserName";
    cout << "| " << setw(20) << left << "Password";
    cout << "| " << setw(10) << left << "Permission" << " |\n";
    cout << "--------------------------------------------------------------------------\n";

    if (vUsers.empty())
        cout << "\t\t\tThe Users System Is Empty!\n";
    else
    {
        for (const stUser& U : vUsers)
            PrintUserLine(U);
    }
    cout << "--------------------------------------------------------------------------\n";
}

void PrintUserRecord(const stUser& User)
{
    cout << "\n---------------------------------------------";
    cout << "\n Username :   " << User.UserName;
    cout << "\n Password :   " << User.Password;
    cout << "\n Permissions: " << User.permissions;
    cout << "\n---------------------------------------------\n";
}

string ReadUserName()
{
    string UserN;
    cout << "\nEnter The Username:   ";
    cin >> UserN;
    return UserN;
}

void FindUserScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Find User Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string UserN = ReadUserName();
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFile);
    stUser User;

    if (FindUserByUserName(vUsers, UserN, User))
        PrintUserRecord(User);
    else
        cout << "\nThe User With Username [" << UserN << "] Is Not Found!\n";
}

void UpdateUserRecord(const string& UserN, stUser& User)
{
    User.UserName = UserN;

    cout << "Enter Password   :   ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, User.Password);

    User.permissions = ReadPermissions();
}

void UpdateUserScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Update User Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string UserN = ReadUserName();
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFile);
    stUser User;

    char Answer = 'N';
        
    for (stUser& U : vUsers)
        if (U.UserName == UserN)
        {
            PrintUserRecord(User);

            cout << "\nAre You Sure You Want To Update This User? (Y/N):   ";
            cin >> Answer;

            if (toupper(Answer) == 'Y')
            {
                UpdateUserRecord(U.UserName, U);
                SaveAllUsersInFile(vUsers, UsersFile);
                cout << "\nThe User updated successfully.\n";
            }
            else
                cout << "\nUpdate Operation Cancelled.\n";

            break;
        }
        else
            cout << "\nThe User With Username [" << UserN << "] Is Not Found!\n";
}

void DeleteUserScreen()
{
    system("cls");
    cout << "\n\t\t-----------------------------------------\n";
    cout << "\t\t\t Delete User Screen ";
    cout << "\n\t\t-----------------------------------------\n";

    string UserN = ReadUserName();
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFile);
    stUser User;

    if (UserN == "Admin")
    {
        cout << "\nYou Cannot Delete The System Admin Account!\n";
        return;
    }

    char Answer = 'N';

    for (stUser& U : vUsers)
        if (U.UserName == UserN)
        {
            PrintUserRecord(User);

            cout << "\nAre You Sure You Want To Delete This User? (Y/N):   ";
            cin >> Answer;

            if (toupper(Answer) == 'Y')
            {
                U.MarkToDelete = true;
                SaveAllUsersInFile(vUsers, UsersFile);
                cout << "\nThe User Deleted successfully.\n";
            }
            else
                cout << "\nDeletion Operation Cancelled.\n";

            break;
        }
        else
            cout << "\nThe User With Username [" << UserN << "] Is Not Found!\n";
}


void GoBackMainMenu()
{
    cout << "\nEnter Any Key To Go To Main Menu... ";
    system("pause>0");
}

void GoBackToTransactionMenu()
{
    cout << "\nEnter Any Key To Go To Transaction Menu... \n";
    system("pause>0");
}

void GoBackToUserMenuScreen()
{
    cout << "\nEnter Any Key To Go to User Menu Screen...  \n";
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
    case enTransactionMenuOption::TransferBalance:
        TransferScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionMenuOption::TransferLog:
        ShowTransferLogScreen();
        GoBackToTransactionMenu();
        break;
    case enTransactionMenuOption::MainMenu:
        break;
    }
}

void TransactionMenuScreen()
{
    if (!CheckAccessPermissions(enUserPermissions::pUserTransaction))
    {
        AccessDeniedMsg();
        return;
    }

    enTransactionMenuOption Option;
    do
    {
        system("cls");
        cout << "\n====================================================\n";
        cout << "\t\t Transaction Menu Screen";
        cout << "\n====================================================\n";
        cout << "\t [1] Deposit!\n";
        cout << "\t [2] Withdraw!\n";
        cout << "\t [3] Total Balance!\n";
        cout << "\t [4] Transfer Balances!\n";
        cout << "\t [5] Transfer Log!\n";
        cout << "\t [6] Main Menu Screen!\n";
        cout << "====================================================\n";

        Option = enTransactionMenuOption(ReadOption(1, 6));
        PerformTransactionMenuOption(Option);

    } while (Option != enTransactionMenuOption::MainMenu);
}

void PerformUserMenuOpton(enUsersMenuOption UserOption)
{
    switch (UserOption)
    {
    case enUsersMenuOption::UsersList:
        AllUsersScreen();
        GoBackToUserMenuScreen();
        break;
    case enUsersMenuOption::AddUser:
        AddNewUsers();
        GoBackToUserMenuScreen();
        break;
    case enUsersMenuOption::FindUser:
        FindUserScreen();
        GoBackToUserMenuScreen();
        break;
    case enUsersMenuOption::UpdateUser:
        UpdateUserScreen();
        GoBackToUserMenuScreen();
        break;
    case enUsersMenuOption::DeleteUsser:
        DeleteUserScreen();
        GoBackToUserMenuScreen();
        break;
    case enUsersMenuOption::pMainMenu:
        break;
    }
}

void UsersScreen()
{
    if (!CheckAccessPermissions(enUserPermissions::pManageUsers))
    {
        AccessDeniedMsg();
        return;
    }

    enUsersMenuOption Option;
    do
    {
        system("cls");
        cout << "\n====================================================";
        cout << "\n\t\t Users Menu Screen";
        cout << "\n====================================================";
        cout << "\n\t [1] Show Users List!";
        cout << "\n\t [2] Add New User!";
        cout << "\n\t [3] Find User!";
        cout << "\n\t [4] Update User!";
        cout << "\n\t [5] Delete User!";
        cout << "\n\t [6] Main Menu Screen!";
        cout << "\n====================================================\n";

        Option = enUsersMenuOption(ReadOption(1, 6));
        PerformUserMenuOpton(Option);

    } while (Option != enUsersMenuOption::pMainMenu);
}

//---- Main Menu ----;
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
    cout << "\t [6] Transaction Menu!\n";
    cout << "\t [7] Manage Users!\n";
    cout << "\t [8] Logout!\n";
    cout << "====================================================\n";
}

void PerformMainMenuOption(enMainMenuOption Option)
{
    switch (Option)
    {
    case enMainMenuOption::eClientList:
        AllClientsScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::eAddClient:
        AddNewClientsScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::eFindClient:
        FindClientScreen();
        GoBackMainMenu();
        break;
    case enMainMenuOption::eUpdateClient:
        UpdateClientScreenByAccNumber();
        GoBackMainMenu();
        break;
    case enMainMenuOption::eDeleteClient:
        DeleteClientScreenByAccNumber();
        GoBackMainMenu();
        break;
    case enMainMenuOption::eTransactionScreen:
        TransactionMenuScreen();
        break;
    case enMainMenuOption::eManageUsers:
        UsersScreen();
        break;
    case enMainMenuOption::eLogOut:
        break;
    }
}

bool LoadUserInfo(string UserName, string Password) 
{
    vector<stUser> vUsers = LoadUsersDataFromFile(UsersFile);

    for (stUser& U : vUsers) 
        if (U.UserName == UserName && U.Password == Password) 
        {
            CurrentUser = U;
            return true;
        }
    
    return false;
}

void LogIn()
{
    bool LoginFailed = false;
    string UserName, Password;

    do
    {
        system("cls");
        cout << "\n------------------------------\n";
        cout << "\tLogin Screen";
        cout << "\n------------------------------\n";

        if (LoginFailed)
            cout << "Invalid Username/Password!\n";

        cout << "Enter Username? ";
        cin >> UserName;
        cout << "Enter Password? ";
        cin >> Password;

        LoginFailed = !LoadUserInfo(UserName, Password);

    } while (LoginFailed);
}

int main()
{
    while (true)   // Infinite loop to keep the program running even after logout;
    {
        LogIn();

        enMainMenuOption Option;
        do
        {
            MainMenuScreen();
            Option = enMainMenuOption(ReadOption(1, 8));
            PerformMainMenuOption(Option);

        } while (Option != enMainMenuOption::eLogOut);
    }

    return 0;
}



