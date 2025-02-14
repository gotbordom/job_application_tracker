#include <iostream>
#include <string>
#include <limits>
#include <sqlite_modern_cpp.h>

using namespace std;
using namespace sqlite;

// Function to create the database table
void createTable(database &db)
{
    db << "CREATE TABLE IF NOT EXISTS job_applications ("
          "   id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "   description TEXT NOT NULL, "
          "   date TEXT NOT NULL, "
          "   status TEXT NOT NULL "
          ");";
}

// Function to add a new job application
void addJobApplication(database &db, const string &description, const string &date, const string &status)
{
    db << "INSERT INTO job_applications (description, date, status) VALUES (?, ?, ?);"
       << description
       << date
       << status;
    cout << "Job application added successfully!\n";
}

// Function to update the status of a job application
void updateJobApplication(database &db, int id, const string &newStatus)
{
    db << "UPDATE job_applications SET status = ? WHERE id = ?;"
       << newStatus
       << id;
    cout << "Job application updated successfully!\n";
}

// Function to display all job applications
void displayJobApplications(database &db)
{
    db << "SELECT id, description, date, status FROM job_applications;" >> [](int id, string description, string date, string status)
    {
        cout << "ID: " << id << "\n"
             << "Description: " << description << "\n"
             << "Date: " << date << "\n"
             << "Status: " << status << "\n\n";
    };
}

// Function to remove a job application by ID
void removeJobApplication(database &db, int id)
{
    db << "DELETE FROM job_applications WHERE id = ?;"
       << id;
    cout << "Job application removed successfully!\n";
}

int main()
{
    try
    {
        // Open the database (or create it if it doesn't exist)
        database db("job_applications.db");

        // Create the table if it doesn't exist
        createTable(db);

        int choice;
        while (true)
        {
            cout << "1. Add New Job Application\n"
                 << "2. Update Job Application Status\n"
                 << "3. View All Job Applications\n"
                 << "4. Remove Job Application\n"
                 << "5. Exit\n"
                 << "Enter your choice: ";

            // Check if the input is a valid integer
            if (!(cin >> choice))
            {
                cin.clear();                                         // Clear the error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << "Error: Invalid Input. Please enter a number between 1 and 5.\n\n";
                continue;
            }

            cin.ignore(); // Ignore the newline character left by cin

            if (choice == 1)
            {
                string description, date, status;
                cout << "Enter Job Description: ";
                getline(cin, description);
                cout << "Enter Date (YYYY-MM-DD): ";
                getline(cin, date);
                cout << "Enter Status (e.g., Applied, Interviewing, Rejected): ";
                getline(cin, status);

                addJobApplication(db, description, date, status);
            }
            else if (choice == 2)
            {
                int id;
                string newStatus;
                cout << "Enter Job Application ID to update: ";
                cin >> id;
                cin.ignore(); // Ignore the newline character left by cin
                cout << "Enter New Status: ";
                getline(cin, newStatus);

                updateJobApplication(db, id, newStatus);
            }
            else if (choice == 3)
            {
                displayJobApplications(db);
            }
            else if (choice == 4)
            {
                int id;
                cout << "Enter Job Application ID to remove: ";
                cin >> id;
                cin.ignore(); // Ignore the newline character left by cin

                removeJobApplication(db, id);
            }
            else if (choice == 5)
            {
                break;
            }
            else
            {
                cout << "Error: Invalid Input. Please enter a number between 1 and 5.\n\n";
            }
        }
    }
    catch (exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}