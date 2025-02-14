#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <sqlite_modern_cpp.h>

using namespace std;
using namespace sqlite;
namespace fs = filesystem; // For filesystem operations

// Function to check if a date string is in YYYY-MM-DD format
bool isValidDate(const string &date)
{
    if (date.empty())
    {
        return true; // Empty date is allowed (will default to today)
    }

    if (date.length() != 10 || date[4] != '-' || date[7] != '-')
    {
        return false; // Incorrect format
    }

    // Check if the year, month, and day are valid
    int year, month, day;
    try
    {
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(5, 2));
        day = stoi(date.substr(8, 2));
    }
    catch (...)
    {
        return false; // Invalid conversion
    }

    // Basic validation for year, month, and day
    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31)
    {
        return false;
    }

    return true;
}

// Function to get today's date in YYYY-MM-DD format
string getTodayDate()
{
    time_t now = time(nullptr);
    tm tm = *localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
    return string(buffer);
}

// Function to create the database table
void createTable(database &db)
{
    db << "CREATE TABLE IF NOT EXISTS job_applications ("
          "   id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "   description TEXT NOT NULL, "
          "   date TEXT NOT NULL, "
          "   status TEXT NOT NULL, "
          "   url TEXT, "  // New column for job description URL
          "   notes TEXT " // New column for notes
          ");";
}

// Function to add a new job application
void addJobApplication(database &db, const string &description, const string &date, const string &status, const string &url = "", const string &notes = "")
{
    string finalDate = date;

    // If the date is empty, default to today's date
    if (finalDate.empty())
    {
        finalDate = getTodayDate();
    }

    // Validate the date format
    if (!isValidDate(finalDate))
    {
        cout << "Error: Invalid date format. Please use YYYY-MM-DD or leave empty for today's date.\n";
        return;
    }

    // Debugging: Print the values being inserted
    cout << "Debug: Inserting job application with the following details:\n"
         << "Description: " << description << "\n"
         << "Date: " << finalDate << "\n"
         << "Status: " << status << "\n"
         << "URL: " << url << "\n"
         << "Notes: " << notes << "\n";

    try
    {
        db << "INSERT INTO job_applications (description, date, status, url, notes) VALUES (?, ?, ?, ?, ?);"
           << description
           << finalDate
           << status
           << url
           << notes;
        cout << "Job application added successfully!\n";
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << "\n";
        cerr << "Failed to add job application. Please check the input data.\n";
    }
}

// Function to check if an entry exists and print its details
bool checkAndPrintEntryDetails(database &db, int id)
{
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications WHERE id = ?;"
       << id >>
        count;

    if (count == 0)
    {
        cout << "Error: Job application with ID " << id << " does not exist.\n";
        return false;
    }

    // Display the job application details
    db << "SELECT description, date, status, url, notes FROM job_applications WHERE id = ?;"
       << id >>
        [](string description, string date, string status, string url, string notes)
    {
        cout << "Job Application Details:\n"
             << "Description: " << description << "\n"
             << "Date: " << date << "\n"
             << "Status: " << status << "\n"
             << "URL: " << (url.empty() ? "N/A" : url) << "\n"
             << "Notes: " << (notes.empty() ? "N/A" : notes) << "\n";
    };

    return true;
}

// Function to update the status of a job application
void updateJobApplication(database &db)
{
    // Check if the database is empty
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications;" >> count;

    if (count == 0)
    {
        cout << "Error: DB is empty. Cannot update entries.\n";
        return;
    }

    // Display all job applications with their descriptions
    cout << "List of Job Applications:\n";
    db << "SELECT id, description FROM job_applications;" >> [](int id, string description)
    {
        cout << "ID: " << id << " | Description: " << description << "\n";
    };

    // Ask the user to select an ID to update
    int id;
    cout << "Enter the ID of the job application you want to update: ";
    cin >> id;
    cin.ignore(); // Ignore the newline character left by cin

    // Check if the ID exists and print details
    if (!checkAndPrintEntryDetails(db, id))
    {
        return;
    }

    // Ask for the new status
    string newStatus;
    cout << "Enter New Status (e.g., Applied, Interviewing, Rejected): ";
    getline(cin, newStatus);

    // Update the status
    db << "UPDATE job_applications SET status = ? WHERE id = ?;"
       << newStatus
       << id;
    cout << "Job application updated successfully!\n";
}

// Function to display all job applications
void displayJobApplications(database &db)
{
    // Check if the database is empty
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications;" >> count;

    if (count == 0)
    {
        cout << "Error: DB is empty. No entries to display.\n";
        return;
    }

    // Display all job applications
    db << "SELECT id, description, date, status, url, notes FROM job_applications;" >> [](int id, string description, string date, string status, string url, string notes)
    {
        cout << "ID: " << id << "\n"
             << "Description: " << description << "\n"
             << "Date: " << date << "\n"
             << "Status: " << status << "\n"
             << "URL: " << (url.empty() ? "N/A" : url) << "\n"
             << "Notes: " << (notes.empty() ? "N/A" : notes) << "\n\n";
    };
}

// Function to remove a job application by description
void removeJobApplication(database &db)
{
    // Check if the database is empty
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications;" >> count;

    if (count == 0)
    {
        cout << "Error: DB is empty. Cannot remove entries.\n";
        return;
    }

    // Display all job applications with their descriptions
    cout << "List of Job Applications:\n";
    db << "SELECT id, description FROM job_applications;" >> [](int id, string description)
    {
        cout << "ID: " << id << " | Description: " << description << "\n";
    };

    // Ask the user to select an ID to remove
    int id;
    cout << "Enter the ID of the job application you want to remove: ";
    cin >> id;
    cin.ignore(); // Ignore the newline character left by cin

    // Check if the ID exists and print details
    if (!checkAndPrintEntryDetails(db, id))
    {
        return;
    }

    // Ask for confirmation
    string confirmation;
    cout << "Are you sure you want to delete this job application? (yes/no): ";
    getline(cin, confirmation);

    if (confirmation == "yes" || confirmation == "y")
    {
        db << "DELETE FROM job_applications WHERE id = ?;"
           << id;
        cout << "Job application removed successfully!\n";
    }
    else
    {
        cout << "Deletion canceled.\n";
    }
}

// Function to remove all entries from the database
void removeAllEntries(database &db)
{
    // Check if the database is empty
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications;" >> count;

    if (count == 0)
    {
        cout << "Error: DB is already empty.\n";
        return;
    }

    // Ask for confirmation
    string confirmation;
    cout << "Are you sure you want to delete ALL job applications? (yes/no): ";
    getline(cin, confirmation);

    if (confirmation == "yes" || confirmation == "y")
    {
        db << "DELETE FROM job_applications;";
        cout << "All job applications removed successfully!\n";
    }
    else
    {
        cout << "Deletion canceled.\n";
    }
}

// Function to export all entries to a CSV file
void exportToCSV(database &db)
{
    // Check if the database is empty
    int count = 0;
    db << "SELECT COUNT(*) FROM job_applications;" >> count;

    if (count == 0)
    {
        cout << "Error: DB is empty. No entries to export.\n";
        return;
    }

    // Ask for the CSV file name
    string filename;
    cout << "Enter the name of the CSV file to export to (e.g., job_applications.csv): ";
    getline(cin, filename);

    // Get the absolute path of the file
    fs::path filePath = fs::absolute(filename);

    // Open the CSV file for writing
    ofstream csvFile(filePath);
    if (!csvFile.is_open())
    {
        cout << "Error: Could not open file " << filePath << " for writing.\n";
        return;
    }

    // Write the CSV header
    csvFile << "ID,Description,Date,Status,URL,Notes\n";

    // Write all entries to the CSV file
    db << "SELECT id, description, date, status, url, notes FROM job_applications;" >> [&](int id, string description, string date, string status, string url, string notes)
    {
        csvFile << id << "," << description << "," << date << "," << status << "," << url << "," << notes << "\n";
    };

    csvFile.close();
    cout << "Job applications exported to " << filePath << " successfully!\n";
}

// Function to import entries from a CSV file
void importFromCSV(database &db)
{
    // Ask for the CSV file name
    string filename;
    cout << "Enter the name of the CSV file to import from (e.g., job_applications.csv): ";
    getline(cin, filename);

    // Get the absolute path of the file
    fs::path filePath = fs::absolute(filename);

    // Open the CSV file for reading
    ifstream csvFile(filePath);
    if (!csvFile.is_open())
    {
        cout << "Error: Could not open file " << filePath << " for reading.\n";
        return;
    }

    // Read the CSV file line by line
    string line;
    getline(csvFile, line); // Skip the header line
    while (getline(csvFile, line))
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos || pos4 == string::npos || pos5 == string::npos)
        {
            cout << "Error: Invalid CSV format in line: " << line << "\n";
            continue;
        }

        string description = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string date = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string status = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string url = line.substr(pos4 + 1, pos5 - pos4 - 1);
        string notes = line.substr(pos5 + 1);

        // Add the job application to the database
        addJobApplication(db, description, date, status, url, notes);
    }

    csvFile.close();
    cout << "Job applications imported from " << filePath << " successfully!\n";
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
                 << "5. Remove All Entries\n"
                 << "6. Export to CSV\n"
                 << "7. Import from CSV\n"
                 << "8. Exit\n"
                 << "Enter your choice: ";

            // Check if the input is a valid integer
            if (!(cin >> choice))
            {
                cin.clear();                                         // Clear the error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << "Error: Invalid Input. Please enter a number between 1 and 8.\n\n";
                continue;
            }

            cin.ignore(); // Ignore the newline character left by cin

            if (choice == 1)
            {
                string description, date, status, url, notes;
                cout << "Enter Job Description: ";
                getline(cin, description);
                cout << "Enter Date (YYYY-MM-DD or leave empty for today): ";
                getline(cin, date);
                cout << "Enter Status (e.g., Applied, Interviewing, Rejected): ";
                getline(cin, status);
                cout << "Enter Job Description URL (optional): ";
                getline(cin, url);
                cout << "Enter Notes (optional): ";
                getline(cin, notes);

                addJobApplication(db, description, date, status, url, notes);
            }
            else if (choice == 2)
            {
                updateJobApplication(db);
            }
            else if (choice == 3)
            {
                displayJobApplications(db);
            }
            else if (choice == 4)
            {
                removeJobApplication(db);
            }
            else if (choice == 5)
            {
                removeAllEntries(db);
            }
            else if (choice == 6)
            {
                exportToCSV(db);
            }
            else if (choice == 7)
            {
                importFromCSV(db);
            }
            else if (choice == 8)
            {
                break;
            }
            else
            {
                cout << "Error: Invalid Input. Please enter a number between 1 and 8.\n\n";
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