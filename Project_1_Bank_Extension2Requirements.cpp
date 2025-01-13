#include <iostream>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>
#include <iomanip>

using namespace std;
const string ClientsFileName = "/storage/emulated/0/OutputFilesCpp/Client.txt";
const string UsersFileName = "/storage/emulated/0/OutputFilesCpp/Users.txt";


void ShowMainMenue();
void ShowTransactionsMenue();


void GoBackToMainMenue()
{
    cout << "\n\nPress any key to go back to Main Menue...";
    system("read");
    ShowMainMenue();

}


struct stUser
{
  string UserName;
  string Password;
  int Permissions;
  bool MarkForDeleteUser = false;
  
};

stUser  currentUser;

struct sClient
{
    stUser User;
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double AccountBalance;
    bool MarkForDelete = false;
    
};
enum enTransactionsMenueOptions { eDeposit = 1, eWithdraw = 2, eShowTotalBalance = 3, eShowMainMenue = 4 };

enum enMainMenueOptions { eListClients = 1, eAddNewClient = 2, eDeleteClient = 3, eUpdateClient = 4, eFindClient = 5, eShowTransactionsMenue = 6, eMangeUsers = 7 ,eLogout = 8 };

enum enManageUsers {eListUser = 1 , eAddNewUser = 2 ,eDeleteUesrs = 3, eUpdateUsers = 4, eFindUser = 5, eMainMenue = 6};
enum enPermissions {eAll = -1 , eListClientsPermissions = 1 , eAddNewClientPermissions = 2 ,eDeleteClientPermissions = 4 ,eUpdateClientPermissions = 8 ,eFindClientPermissions = 16,
eTransactionsMenuePermissions = 32, eMangeUsersPermissions = 64};

bool checkPremissions(enPermissions Permissions );

int setPermissions() {
    char Answer = 'y';
    int Permissions = 0;

    // Check for full access
    cout << "\nDo you want to give full access? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        return -1; // Full access
    }

    // Ask for individual permissions
    cout << "\nDo you want to give access to the following features?";

    cout << "\nShow Client List? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eListClientsPermissions;
    }

    cout << "\nAdd New Client? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eAddNewClientPermissions;
    }

    cout << "\nDelete Client? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eDeleteClientPermissions;
    }

    cout << "\nUpdate Client? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eUpdateClientPermissions;
    }

    cout << "\nFind Client? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eFindClientPermissions;
    }
    
    cout << "\nTransactions? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eTransactionsMenuePermissions;
    }
    

    cout << "\nManage Users? (y/n): ";
    cin >> Answer;
    if (Answer == 'y') {
        Permissions += eMangeUsersPermissions;
    }

    return Permissions;
}

bool FindUserByUsername(string Username, vector <stUser> vUsers, stUser& User)
{

    for (stUser U : vUsers)
    {

        if (U.UserName == Username)
        {
            User = U;
            return true;
        }

    }
    return false;

}



vector<string> SplitString(string S1, string Delim)
{

    vector <string> vString;

    short pos = 0;
    string sWord; // define a string variable

    // use find() function to get the position of the delimiters
    while ((pos = S1.find(Delim)) != std::string::npos)
    {
        sWord = S1.substr(0, pos); // store the word
        if (sWord != "")
        {
            vString.push_back(sWord);
        }

        S1.erase(0, pos + Delim.length());  /* erase() until positon and move to next word. */
    }

    if (S1 != "")
    {
        vString.push_back(S1); // it adds last word of the string.
    }

    return vString;

}

sClient ConvertLinetoRecord(string Line, string Seperator = "#//#")
{

    
    sClient Client;
    vector<string> vClientData;

    vClientData = SplitString(Line, Seperator);

    // Ignore lines that do not have exactly 5 fields
    if (vClientData.size() < 5)
    {
        std::cerr << "Error: Insufficient fields in line: " << Line << std::endl;
        return Client; // Return a default-constructed client
    }

    Client.AccountNumber = vClientData[0];
    Client.PinCode = vClientData[1];
    Client.Name = vClientData[2];
    Client.Phone = vClientData[3];

    // Validate and convert the balance field
    try {
        Client.AccountBalance = stod(vClientData[4]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid balance value (" << vClientData[4] << ") in line: " << Line << std::endl;
        Client.AccountBalance = 0.0; // Set a default value in case of error
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Balance value out of range (" << vClientData[4] << ") in line: " << Line << std::endl;
        Client.AccountBalance = 0.0; // Set a default value in case of error
    }

    return Client;

}


string ConvertRecordToLine(sClient Client, string Seperator = "#//#")
{

    string stClientRecord = "";

    stClientRecord += Client.AccountNumber + Seperator;
    stClientRecord += Client.PinCode + Seperator;
    stClientRecord += Client.Name + Seperator;
    stClientRecord += Client.Phone + Seperator;
    stClientRecord += to_string(Client.AccountBalance);

    return stClientRecord;

}

string ConvertRecordToLineForUser(stUser User, string Seperator = "#//#")
{

    string stClientRecord = "";

    stClientRecord += User.UserName + Seperator;
    stClientRecord += User.Password + Seperator;
    stClientRecord += to_string(User.Permissions);

    return stClientRecord;

}

bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{

    vector <sClient> vClients;

    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open())
    {

        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {

            Client = ConvertLinetoRecord(Line);
            if (Client.AccountNumber == AccountNumber)
            {
                MyFile.close();
                return true;
            }


            vClients.push_back(Client);
        }

        MyFile.close();

    }

    return false;


}

sClient ReadNewClient()
{
    sClient Client;

    cout << "Enter Account Number? ";

    // Usage of std::ws will extract allthe whitespace character
    getline(cin >> ws, Client.AccountNumber);

    while (ClientExistsByAccountNumber(Client.AccountNumber, ClientsFileName))
    {
        cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
        getline(cin >> ws, Client.AccountNumber);
    }


    cout << "Enter PinCode? ";
    getline(cin, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;

}

vector <sClient> LoadCleintsDataFromFile(string FileName)
{

    vector <sClient> vClients;

    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open())
    {

        string Line;
        sClient Client;

        while (getline(MyFile, Line))
        {

            Client = ConvertLinetoRecord(Line);

            vClients.push_back(Client);
        }

        MyFile.close();

    }

    return vClients;

}

stUser ConvertLinetoRecordForUsers(string Line ,string Seperator = "#//#")
{
  
    
    stUser User;
    vector<string> vUserData;

    vUserData = SplitString(Line, Seperator);

    User.UserName = vUserData[0];
    User.Password = vUserData[1];

    try {
        User.Permissions = stoi(vUserData[2]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid permissions value (" << vUserData[2] << ")." << std::endl;
        User.Permissions = 0; // Default value
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Permissions value out of range (" << vUserData[2] << ")." << std::endl;
        User.Permissions = 0; // Default value
    }

    return User;
  
}

vector <stUser> LoadUsersDataFromFile(string FileName)
{
  
    vector <stUser> vUsers;

    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open())
    {

        string Line;
        stUser User;

        while (getline(MyFile, Line))
        {

          User = ConvertLinetoRecordForUsers(Line);

          vUsers.push_back(User);
        }

        MyFile.close();

    }
    
    return vUsers;
}


void PrintClientRecordLine(sClient Client)
{

    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(10) << left << Client.PinCode;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.Phone;
    cout << "| " << setw(12) << left << Client.AccountBalance;

}

void PrintUserRecordLine(stUser User)
{
  
    cout << "| " << setw(15) << left << User.UserName;
    cout << "| " << setw(10) << left << User.Password;
    cout << "| " << setw(40) << left << User.Permissions;
  
}

void PrintClientRecordBalanceLine(sClient Client)
{

    cout << "| " << setw(15) << left << Client.AccountNumber;
    cout << "| " << setw(40) << left << Client.Name;
    cout << "| " << setw(12) << left << Client.AccountBalance;

}

void ShowAccessDeniedMessage() {
    cout << string(23, '-') << endl;
    cout << "Access Denied," << endl; 
    cout << "You don't have permission to do this," << endl;
    cout << "Please contact your admin." << endl;
    cout << string(23, '-') << endl;

    GoBackToMainMenue(); // تأكد من أن هذه الدالة تعمل بشكل صحيح
}

void ShowAllClientsScreen()
{
    if(!checkPremissions(eListClientsPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(10) << "Pin Code";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Phone";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (sClient Client : vClients)
        {

            PrintClientRecordLine(Client);
            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

}

void ShowTotalBalances()
{

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);

    cout << "\n\t\t\t\t\tBalances List (" << vClients.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "Accout Number";
    cout << "| " << left << setw(40) << "Client Name";
    cout << "| " << left << setw(12) << "Balance";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    double TotalBalances = 0;

    if (vClients.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (sClient Client : vClients)
        {

            PrintClientRecordBalanceLine(Client);
            TotalBalances += Client.AccountBalance;

            cout << endl;
        }

    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;
    cout << "\t\t\t\t\t   Total Balances = " << TotalBalances;

}

void PrintClientCard(sClient Client)
{
    cout << "\nThe following are the client details:\n";
    cout << "-----------------------------------";
    cout << "\nAccout Number: " << Client.AccountNumber;
    cout << "\nPin Code     : " << Client.PinCode;
    cout << "\nName         : " << Client.Name;
    cout << "\nPhone        : " << Client.Phone;
    cout << "\nAccount Balance: " << Client.AccountBalance;
    cout << "\n-----------------------------------\n";

}

void PrintUserCard(stUser User)
{
    cout << "\nThe following are the user details:\n";
    cout << "-----------------------------------";
    cout << "\nUserame      : " << User.UserName;
    cout << "\nPassword     : " << User.Password;
    cout << "\nPermissions  : " << User.Permissions;
    cout << "\n-----------------------------------\n";

  
}

bool FindClientByAccountNumber(string AccountNumber, vector <sClient> vClients, sClient& Client)
{

    for (sClient C : vClients)
    {

        if (C.AccountNumber == AccountNumber)
        {
            Client = C;
            return true;
        }

    }
    return false;

}

bool FindUserByUserName(string userName, vector <stUser> vUsers, stUser& User)
{

    for (auto C : vUsers)
    {

        if (C.UserName == userName )
        {
            User = C;
            return true;
        }

    }
    return false;

}



sClient ChangeClientRecord(string AccountNumber)
{
    sClient Client;

    Client.AccountNumber = AccountNumber;

    cout << "\n\nEnter PinCode? ";
    getline(cin >> ws, Client.PinCode);

    cout << "Enter Name? ";
    getline(cin, Client.Name);

    cout << "Enter Phone? ";
    getline(cin, Client.Phone);

    cout << "Enter AccountBalance? ";
    cin >> Client.AccountBalance;

    return Client;

}

bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{

    for (sClient& C : vClients)
    {

        if (C.AccountNumber == AccountNumber)
        {
            C.MarkForDelete = true;
            return true;
        }

    }

    return false;

}

bool MarkUserForDeleteByUsername(string Username, vector <stUser>& vUsers)
{

    for (auto &  C : vUsers)
    {

        if (C.UserName == Username )
        {
            C.MarkForDeleteUser = true;
            return true;
        }

    }

    return false;

}



vector <sClient> SaveCleintsDataToFile(string FileName, vector <sClient> vClients)
{

    fstream MyFile;
    MyFile.open(FileName, ios::out);//overwrite

    string DataLine;

    if (MyFile.is_open())
    {

        for (sClient C : vClients)
        {

            if (C.MarkForDelete == false)
            {
                //we only write records that are not marked for delete.
                DataLine = ConvertRecordToLine(C);
                MyFile << DataLine << endl;

            }

        }

        MyFile.close();

    }

    return vClients;

}

vector <stUser> SaveUsersDataToFile(string FileName, vector <stUser> vUsers)
{

    fstream MyFile;
    MyFile.open(FileName, ios::out);//overwrite

    string DataLine;

    if (MyFile.is_open())
    {

        for (auto C : vUsers)
        {

            if (C.MarkForDeleteUser == false)
            {
                //we only write records that are not marked for delete.
                DataLine = ConvertRecordToLineForUser(C);
                MyFile << DataLine << endl;

            }

        }

        MyFile.close();

    }

    return vUsers;

}



void AddDataLineToFile(string FileName, string  stDataLine)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);

    if (MyFile.is_open())
    {

        MyFile << stDataLine << endl;

        MyFile.close();
    }

}

void AddNewClient()
{
    sClient Client;
    Client = ReadNewClient();
    AddDataLineToFile(ClientsFileName, ConvertRecordToLine(Client));

}

void AddNewClients()
{
    char AddMore = 'Y';
    do
    {
        //system("cls");
        cout << "Adding New Client:\n\n";

        AddNewClient();
        cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";


        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');

}

bool DeleteClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{

    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {

        PrintClientCard(Client);

        cout << "\n\nAre you sure you want delete this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {
            MarkClientForDeleteByAccountNumber(AccountNumber, vClients);
            SaveCleintsDataToFile(ClientsFileName, vClients);

            //Refresh Clients
            vClients = LoadCleintsDataFromFile(ClientsFileName);

            cout << "\n\nClient Deleted Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }

}




bool DeleteUserByUsername(string Username, vector <stUser>& vUsers)
{

    if (Username == "Admin")
    {
        cout << "\n\nYou cannot Delete This User.";
        return false;

    }

    stUser User;
    char Answer = 'n';

    if (FindUserByUserName(Username, vUsers, User))
    {

        PrintUserCard(User);

        cout << "\n\nAre you sure you want delete this User? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {

            MarkUserForDeleteByUsername(Username, vUsers);
            SaveUsersDataToFile(UsersFileName, vUsers);

            //Refresh Clients 
            vUsers = LoadUsersDataFromFile(UsersFileName);

            cout << "\n\nUser Deleted Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nUser with Username (" << Username << ") is Not Found!";
        return false;
    }

}

bool UpdateClientByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{

    sClient Client;
    char Answer = 'n';

    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
    {

        PrintClientCard(Client);
        cout << "\n\nAre you sure you want update this client? y/n ? ";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {

            for (sClient& C : vClients)
            {
                if (C.AccountNumber == AccountNumber)
                {
                    C = ChangeClientRecord(AccountNumber);
                    break;
                }

            }

            SaveCleintsDataToFile(ClientsFileName, vClients);

            cout << "\n\nClient Updated Successfully.";
            return true;
        }

    }
    else
    {
        cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
        return false;
    }

}

stUser ChangeUserRecord(string Username)
{
    stUser User;

    User.UserName = Username;

    cout << "\n\nEnter Password? ";
    getline(cin >> ws, User.Password);

    User.Permissions = setPermissions();

    return User;


}

bool UpdateUserByUsername(string Username, vector <stUser>& vUsers)
{

    stUser User;
    char Answer = 'y';

    if (FindUserByUsername(Username, vUsers, User))
    {

        PrintUserCard(User);
        cout << "\n\nAre you sure you want update this User? y/n ?";
        cin >> Answer;
        if (Answer == 'y' || Answer == 'Y')
        {

            for (stUser& U : vUsers)
            {
                if (U.UserName == Username)
                {
                    U = ChangeUserRecord(Username);
                    break;
                }

            }

            SaveUsersDataToFile(UsersFileName, vUsers);

            cout << "\n\nUser Updated Successfully.";
            return true;
        } else {
          
          return true;
        }

    }
    else
    {
        cout << "\nUser with Account Number (" << Username << ") is Not Found!";
        return false;
    }

}


bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <sClient>& vClients)
{


    char Answer = 'n';


    cout << "\n\nAre you sure you want perfrom this transaction? y/n ? ";
    cin >> Answer;
    if (Answer == 'y' || Answer == 'Y')
    {

        for (sClient& C : vClients)
        {
            if (C.AccountNumber == AccountNumber)
            {
                C.AccountBalance += Amount;
                SaveCleintsDataToFile(ClientsFileName, vClients);
                cout << "\n\nDone Successfully. New balance is: " << C.AccountBalance;

                return true;
            }

        }


        return false;
    }

}

string ReadClientAccountNumber()
{
    string AccountNumber = "";

    cout << "\nPlease enter AccountNumber? ";
    cin >> AccountNumber;
    return AccountNumber;

}

string ReadUserName()
{
    string AccountNumber = "";

    cout << "\nPlease enter UserName? ";
    cin >> AccountNumber;
    return AccountNumber;

}




void ShowDeleteClientScreen()
{
    if(!checkPremissions(eDeleteClientPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }

   
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber, vClients);

}

void ShowDeleteUsertScreen()
{
    
   
    cout << "\n-----------------------------------\n";
    cout << "\tDelete User Screen";
    cout << "\n-----------------------------------\n";

    vector <stUser> vUser = LoadUsersDataFromFile(UsersFileName);
    string UserName = ReadUserName();
    DeleteUserByUsername(UserName, vUser);

}

void ShowUpdateClientScreen()
{
  
   
    if(!checkPremissions(eUpdateClientPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }

    cout << "\n-----------------------------------\n";
    cout << "\tUpdate Client Info Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    UpdateClientByAccountNumber(AccountNumber, vClients);

}

void ShowUpdateUserScreen()
{
  

    cout << "\n-----------------------------------\n";
    cout << "\tUpdate User Info Screen";
    cout << "\n-----------------------------------\n";

    vector <stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    string Username = ReadUserName();
    UpdateUserByUsername(Username, vUsers);

}




void ShowAddNewClientsScreen()
{
  
    if(!checkPremissions(eAddNewClientPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }

    cout << "\n-----------------------------------\n";
    cout << "\tAdd New Clients Screen";
    cout << "\n-----------------------------------\n";

    AddNewClients();

}

void ShowFindClientScreen()
{
  
    if(!checkPremissions(eFindClientPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }

    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen";
    cout << "\n-----------------------------------\n";

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    sClient Client;
    string AccountNumber = ReadClientAccountNumber();
    if (FindClientByAccountNumber(AccountNumber, vClients, Client))
        PrintClientCard(Client);
    else
        cout << "\nClient with Account Number[" << AccountNumber << "] is not found!";

}

void ShowFindUserScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tFind User Screen";
    cout << "\n-----------------------------------\n";

    vector <stUser> vUsers = LoadUsersDataFromFile(UsersFileName);
    stUser User;
    string Username = ReadUserName();
    
    if (FindUserByUserName(Username, vUsers, User)) {
      
      PrintUserCard(User);
      
    } else {
      
      cout << "\nUser with Username [" << Username << "] is not found!" << endl;

    }
        
}

void ShowEndScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tProgram Ends :-)";
    cout << "\n-----------------------------------\n";

}

void ShowDepositScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDeposit Screen";
    cout << "\n-----------------------------------\n";


    sClient Client;

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();


    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadClientAccountNumber();
    }


    PrintClientCard(Client);

    double Amount = 0;
    cout << "\nPlease enter deposit amount? ";
    cin >> Amount;

    DepositBalanceToClientByAccountNumber(AccountNumber, Amount, vClients);

}

void ShowWithDrawScreen()
{
    cout << "\n-----------------------------------\n";
    cout << "\tWithdraw Screen";
    cout << "\n-----------------------------------\n";

    sClient Client;

    vector <sClient> vClients = LoadCleintsDataFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();


    while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadClientAccountNumber();
    }

    PrintClientCard(Client);

    double Amount = 0;
    cout << "\nPlease enter withdraw amount? ";
    cin >> Amount;

    //Validate that the amount does not exceeds the balance
    while (Amount > Client.AccountBalance)
    {
        cout << "\nAmount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
        cout << "Please enter another amount? ";
        cin >> Amount;
    }

    DepositBalanceToClientByAccountNumber(AccountNumber, Amount * -1, vClients);

}

void ShowTotalBalancesScreen()
{

    ShowTotalBalances();

}



void GoBackToTransactionsMenue()
{
    cout << "\n\nPress any key to go back to Transactions Menue...";
    system("read");
    ShowTransactionsMenue();

}
short ReadTransactionsMenueOption()
{
    cout << "Choose what do you want to do? [1 to 4]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

void PerfromTranactionsMenueOption(enTransactionsMenueOptions TransactionMenueOption)
{
    switch (TransactionMenueOption)
    {
    case enTransactionsMenueOptions::eDeposit:
    {
        system("clear");
        ShowDepositScreen();
        GoBackToTransactionsMenue();
        break;
    }

    case enTransactionsMenueOptions::eWithdraw:
    {
        system("clear");
        ShowWithDrawScreen();
        GoBackToTransactionsMenue();
        break;
    }


    case enTransactionsMenueOptions::eShowTotalBalance:
    {
        system("clear");
        ShowTotalBalancesScreen();
        GoBackToTransactionsMenue();
        break;
    }


    case enTransactionsMenueOptions::eShowMainMenue:
    {

        ShowMainMenue();

    }
    
    }

}

void ShowListUser()
{
  
    vector <stUser> vUsers = LoadUsersDataFromFile(UsersFileName);

    cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ") Client(s).";
    cout << "\n_______________________________________________________";
    cout << "_________________________________________\n" << endl;

    cout << "| " << left << setw(15) << "User Name";
    cout << "| " << left << setw(40) << "Password";
    cout << "| " << left << setw(12) << "Permissions";
    cout << "\n_______________________________________________________";
    cout << "__________________________________________________________\n" << endl;


    if (vUsers.size() == 0)
        cout << "\t\t\t\tNo Clients Available In the System!";
    else

        for (stUser User : vUsers)
        {

            PrintUserRecordLine(User);

            cout << endl;
        }

    cout << "\n_______________________________________________________";
    
  
  
}



void ShowTransactionsMenue()
{
    if(!checkPremissions(eTransactionsMenuePermissions)) {
       
      ShowAccessDeniedMessage();
      return;
    }
    
    system("clear");
    cout << "===========================================\n";
    cout << "\t\tTransactions Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Deposit.\n";
    cout << "\t[2] Withdraw.\n";
    cout << "\t[3] Total Balances.\n";
    cout << "\t[4] Main Menue.\n";
    cout << "===========================================\n";
    PerfromTranactionsMenueOption((enTransactionsMenueOptions)ReadTransactionsMenueOption());
}

short ReadMainMenueOption()
{
    cout << "Choose what do you want to do? [1 to 8]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
}

short ReadMangeUsersMenumScreen()
{
  
    cout << "Choose what do you want to do? [1 to 6]? ";
    short Choice = 0;
    cin >> Choice;

    return Choice;
  
}

bool UserExisistsByUserName(string FileName, string UserName)
{
  
  vector <stUser> vUser;

    fstream MyFile;
    MyFile.open(FileName, ios::in);//read Mode

    if (MyFile.is_open())
    {

        string Line;
        stUser User;

        while (getline(MyFile, Line))
        {

            User = ConvertLinetoRecordForUsers(Line);
            if (User.UserName == UserName)
            {
                MyFile.close();
                return true;
            }


            vUser.push_back(User);
        }

        MyFile.close();

    }

    return false;

  
}


stUser ReadNewUser();

void AddNewUser()
{
  
    stUser User;
    User = ReadNewUser();
    AddDataLineToFile(UsersFileName, ConvertRecordToLineForUser(User));
  
}

void AddNewUsers()
{
  
    char AddMore = 'Y';
    do
    {
        //system("cls");
        cout << "Adding New User:\n\n";

        AddNewUser();
        cout << "\nUser Added Successfully, do you want to add more Users? Y/N? ";


        cin >> AddMore;

    } while (toupper(AddMore) == 'Y');
  
  
}

void AddNewUserScreen()
{
  cout << "\n-----------------------------------\n";
  cout << "\tAdd New User Screen";
  cout << "\n-----------------------------------\n";
  
  AddNewUsers();
  
}


stUser ReadNewUser()
{
  
  stUser User;
  
  
  cout << "Enter UserName ";
  getline(cin >> ws , User.UserName);
  
  while(UserExisistsByUserName(UsersFileName,User.UserName)) {
    
    cout << "User with ["<< User.UserName <<"] already exists, Enter another Username?";
    getline(cin >> ws , User.UserName);
    
  }
  
  cout << "Enter Password :";
  getline(cin >> ws , User.Password);
  
  User.Permissions = setPermissions();
  
  
  return User;
  
  
}
void MangeUsersMenumScreen();

void GoBackToManageUsersMenue()
{
    cout << "\n\nPress any key to go back to Transactions Menue...";
    system("read");
    MangeUsersMenumScreen();

}

void PerfromManageUsersMenue(enManageUsers ManageUsersScreen)
{
  
  
    switch (ManageUsersScreen)
    {
    case enManageUsers ::eListUser:
    {
        system("clear");
        ShowListUser();
        GoBackToManageUsersMenue();
        break;
    }
    case  enManageUsers ::eAddNewUser:
    {
        system("clear");
        AddNewUserScreen();
        GoBackToManageUsersMenue();
        break;
    }
    case enManageUsers::eDeleteUesrs:
    {
        system("clear");
        ShowDeleteUsertScreen();
        GoBackToManageUsersMenue();
        break;
    }
    case enManageUsers::eUpdateUsers:
    {
        system("clear");
        ShowUpdateUserScreen();
        GoBackToManageUsersMenue();
        break;

    }
    case enManageUsers::eFindUser:
    {
      system("clear");
      ShowFindUserScreen();
      GoBackToManageUsersMenue();
      break;

    }
    case enManageUsers::eMainMenue:
    {

        ShowMainMenue();
        break;

    }
    
    }
  
}

void MangeUsersMenumScreen()
{
    if(!checkPremissions(eMangeUsersPermissions)) {
      
      ShowAccessDeniedMessage();
      return;
    }
    
    system("clear");
    cout << "===========================================\n";
    cout << "\t\tManage Users Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] List Users .\n";
    cout << "\t[2] Add New user .\n";
    cout << "\t[3] Delete user .\n";
    cout << "\t[4] Update user .\n";
    cout << "\t[5] Find user .\n";
    cout << "\t[6] Main Menue.\n";
    cout << "===========================================\n";
    PerfromManageUsersMenue((enManageUsers)ReadMangeUsersMenumScreen());
  
  
}



bool checkPremissions(enPermissions Permissions )
{
  if (currentUser.Permissions == eAll) {
    
    return true;
  }
  
  if ((currentUser.Permissions & Permissions) ==  Permissions) {
    
    return true;
  }
  return false;
}


void Login();

void PerfromMainMenueOption(enMainMenueOptions MainMenueOption)
{
    switch (MainMenueOption)
    {
    case enMainMenueOptions::eListClients:
    {
        system("clear");
        ShowAllClientsScreen();
        GoBackToMainMenue();
        break;
    }
    case enMainMenueOptions::eAddNewClient:
        system("clear");
        ShowAddNewClientsScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eDeleteClient:
        system("clear");
        ShowDeleteClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eUpdateClient:
        system("clear");
        ShowUpdateClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eFindClient:
        system("clear");
        ShowFindClientScreen();
        GoBackToMainMenue();
        break;

    case enMainMenueOptions::eShowTransactionsMenue:
        system("clear");
        ShowTransactionsMenue();
        break;

    case enMainMenueOptions::eMangeUsers:
        system("clear");
        MangeUsersMenumScreen();
        
        break;
    case enMainMenueOptions::eLogout:
        system("clear");
        Login();
    }

}

void LoginScreen()
{
  
    cout << "===========================================\n";
    cout << "\t\tLogin Screen\n";
    cout << "===========================================\n";
}

void ShowMainMenue()
{
    system("clear");
    cout << "===========================================\n";
    cout << "\t\tMain Menue Screen\n";
    cout << "===========================================\n";
    cout << "\t[1] Show Client List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n";
    cout << "\t[4] Update Client Info.\n";
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n";
    cout << "\t[7] Mange Users.\n";
    cout << "\t[8] Logout.\n";
    cout << "===========================================\n";
    PerfromMainMenueOption((enMainMenueOptions)ReadMainMenueOption());
}

stUser ReadUserNameAndPassword()
{
  stUser User;
  cout << "Enter UserName ? ";
  cin >> User.UserName;
  
  cout << "Enter Password ? "; 
  cin >> User.Password;
  
  return User;
  
}

bool FindUserByUsernameAndPassword(string UserName ,string Password ,stUser & CurrentPermissions)
{
  
  vector <stUser> vUser = LoadUsersDataFromFile(UsersFileName);
  
  for (const auto & v : vUser ) {
    
    if(v.UserName == UserName && v.Password == Password){
      
      CurrentPermissions= v;
      
      return true;
       
    }
    
  }
  
  return false;
    
}


void Login()
{
  
  
  system("clear");
  LoginScreen();
  
   
  currentUser = ReadUserNameAndPassword();
  
  while(!FindUserByUsernameAndPassword(currentUser.UserName,currentUser.Password,currentUser)) {
    system("clear");
    LoginScreen();
    
    cout << "Invlaid Username/Password!" << endl;
    
    currentUser = ReadUserNameAndPassword();
    system("clear");
    LoginScreen();
  }
  
  ShowMainMenue();
  
  
}

int main()
{
  
    Login();
    system("read");
    return 0;
    
}
