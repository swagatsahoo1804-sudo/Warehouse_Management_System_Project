#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using namespace std;

#define DEVICE "/dev/warehouse"

void sendCommand(const string& command)
{
    int fd = open(DEVICE, O_RDWR);

    if (fd < 0)
    {
        perror("Failed to open warehouse device");
        return;
    }

    string cmd = command + "\n";

    if (write(fd, cmd.c_str(), cmd.length()) < 0)
    {
        perror("Failed to write to driver");
        close(fd);
        return;
    }

    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    lseek(fd, 0, SEEK_SET);

    int bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    if (bytesRead < 0)
    {
        perror("Failed to read from driver");
    }
    else
    {
        buffer[bytesRead] = '\0';
        cout << "\nDriver Response:\n";
        cout << buffer << endl;
    }

    close(fd);
}

void stockIn()
{
    string id;
    int quantity;

    cout << "\nEnter Product ID: ";
    cin >> id;

    cout << "Enter Quantity: ";
    cin >> quantity;

    string command = "STOCK_IN " + id + " " + to_string(quantity);

    sendCommand(command);
}

void stockOut()
{
    string id;
    int quantity;

    cout << "\nEnter Product ID: ";
    cin >> id;

    cout << "Enter Quantity: ";
    cin >> quantity;

    string command = "STOCK_OUT " + id + " " + to_string(quantity);

    sendCommand(command);
}

void checkStock()
{
    string id;

    cout << "\nEnter Product ID: ";
    cin >> id;

    string command = "CHECK_STOCK " + id;

    sendCommand(command);
}

void viewInventory()
{
    sendCommand("VIEW_INVENTORY");
}

void searchProduct()
{
    string id;

    cout << "\nEnter Product ID: ";
    cin >> id;

    string command = "SEARCH_PRODUCT " + id;

    sendCommand(command);
}

int main()
{
    int choice;

    while (true)
    {
        cout << "\n=====================================\n";
        cout << "       WAREHOUSE MANAGEMENT SYSTEM\n";
        cout << "=====================================\n";
        cout << "1. Stock In\n";
        cout << "2. Stock Out\n";
        cout << "3. Check Stock\n";
        cout << "4. View All Inventory\n";
        cout << "5. Search Product\n";
        cout << "6. Exit\n";
        cout << "=====================================\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                stockIn();
                break;

            case 2:
                stockOut();
                break;

            case 3:
                checkStock();
                break;

            case 4:
                viewInventory();
                break;

            case 5:
                searchProduct();
                break;

            case 6:
                cout << "\nExiting Warehouse Management System.\n";
                return 0;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }
    }

    return 0;
}
