// ABCU CS Advising Assistant
// - Loads course data from a CSV file (CourseID, Title, Prereq1, Prereq2, ...)
// - Stores course objects in an in-memory structure
// - Provides a menu to list all courses (alphanumeric) and to show a course with prerequisites
// - Includes input validation and helpful error messages
// - Industry-style comments and clear naming for readability

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Course {
    std::string id;
    std::string title;
    std::vector<std::string> prereqs; // prerequisite course IDs
};

// Trim leading and trailing whitespace
static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }
    return s.substr(start, end - start);
}

// Uppercase a string (ASCII)
static std::string toUpper(const std::string& s) {
    std::string t = s;
    for (char& c : t) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return t;
}

// Basic CSV line parser supporting quotes around fields with commas
static std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (inQuotes) {
            if (ch == '"') {
                // If this is a doubled quote, append one and continue inside quotes
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field.push_back(ch);
            }
        } else {
            if (ch == '"') {
                inQuotes = true;
            } else if (ch == ',') {
                fields.push_back(field);
                field.clear();
            } else {
                field.push_back(ch);
            }
        }
    }
    fields.push_back(field);

    // Trim spaces around each field
    for (std::string& f : fields) {
        f = trim(f);
    }

    return fields;
}

// Load courses from a CSV file into the provided map. Returns true on success.
bool loadCoursesFromFile(const std::string& filename, std::map<std::string, Course>& courses, std::vector<std::string>& warnings) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error: Could not open file: " << filename << std::endl;
        return false;
    }

    courses.clear();
    warnings.clear();

    std::string line;
    size_t lineNum = 0;
    while (std::getline(in, line)) {
        ++lineNum;
        // Skip empty lines
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            continue;
        }

        std::vector<std::string> fields = parseCSVLine(line);
        if (fields.size() < 2) {
            warnings.push_back("Line " + std::to_string(lineNum) + " skipped: fewer than 2 fields");
            continue;
        }

        std::string rawId = trim(fields[0]);
        std::string id = toUpper(rawId);
        std::string title = fields[1]; // Keep original case for title

        if (id.empty()) {
            warnings.push_back("Line " + std::to_string(lineNum) + " skipped: empty course ID");
            continue;
        }
        if (title.empty()) {
            warnings.push_back("Line " + std::to_string(lineNum) + " has empty title for course " + id);
        }

        // Ensure a Course object exists for this ID
        Course& course = courses[id];
        course.id = id;
        if (!title.empty()) {
            course.title = title;
        }

        // Handle prerequisites (fields[2..])
        for (size_t i = 2; i < fields.size(); ++i) {
            std::string prereqId = toUpper(trim(fields[i]));
            if (prereqId.empty()) {
                continue;
            }
            course.prereqs.push_back(prereqId);
            // Ensure placeholder exists for prereq so we can resolve title later if defined elsewhere
            if (courses.find(prereqId) == courses.end()) {
                Course placeholder;
                placeholder.id = prereqId;
                placeholder.title = ""; // unknown title until defined
                courses[prereqId] = placeholder;
            }
        }
    }

    return true;
}

// Print the full, alphanumeric-sorted list of courses with titles.
void printSortedCourseList(const std::map<std::string, Course>& courses) {
    std::vector<std::string> ids;
    ids.reserve(courses.size());
    for (const auto& kv : courses) {
        const Course& c = kv.second;
        if (!c.title.empty()) { // Skip placeholder-only entries
            ids.push_back(c.id);
        }
    }
    std::sort(ids.begin(), ids.end());

    std::cout << std::endl;
    std::cout << "Computer Science Course List" << std::endl;
    std::cout << "----------------------------" << std::endl;
    for (const std::string& id : ids) {
        const Course& c = courses.at(id);
        std::cout << id << ", " << c.title << std::endl;
    }
    std::cout << std::endl;
}

// Print details for a specific course by ID (case-insensitive)
void printCourseInfo(const std::map<std::string, Course>& courses, const std::string& queryRaw) {
    std::string query = toUpper(trim(queryRaw));
    if (query.empty()) {
        std::cout << "Error: empty course ID." << std::endl;
        return;
    }

    auto it = courses.find(query);
    if (it == courses.end() || it->second.title.empty()) {
        std::cout << "Course not found: " << query << std::endl;
        return;
    }

    const Course& c = it->second;
    std::cout << std::endl;
    std::cout << c.id << ": " << c.title << std::endl;

    if (c.prereqs.empty()) {
        std::cout << "Prerequisites: None" << std::endl;
    } else {
        std::cout << "Prerequisites:" << std::endl;
        for (const std::string& pid : c.prereqs) {
            auto pit = courses.find(pid);
            if (pit != courses.end() && !pit->second.title.empty()) {
                std::cout << "  - " << pit->second.id << ": " << pit->second.title << std::endl;
            } else {
                std::cout << "  - " << pid << ": Title unknown" << std::endl;
            }
        }
    }
    std::cout << std::endl;
}

// Read a line safely from std::cin into s; return false on EOF
bool safeGetline(std::string& s) {
    if (!std::getline(std::cin, s)) {
        return false;
    }
    // Trim carriage return if present (Windows CRLF handling)
    if (!s.empty() && s.back() == '\r') {
        s.pop_back();
    }
    return true;
}

// Display the main menu
void showMenu() {
    std::cout << std::endl;
    std::cout << "Welcome to the ABCU Computer Science Advising Assistant" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "  1. Load Data Structure" << std::endl;
    std::cout << "  2. Print Course List" << std::endl;
    std::cout << "  3. Print Course" << std::endl;
    std::cout << "  9. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "What would you like to do? " << std::endl;
}

int main() {
    std::map<std::string, Course> courses;
    bool dataLoaded = false;

    while (true) {
        showMenu();
        std::string choiceLine;
        if (!safeGetline(choiceLine)) {
            std::cout << std::endl << "Input closed. Exiting." << std::endl;
            break;
        }
        choiceLine = trim(choiceLine);
        if (choiceLine.empty()) {
            std::cout << "Please enter a menu option (1, 2, 3, or 9)." << std::endl;
            continue;
        }

        // Validate numeric input for choice
        bool numeric = true;
        for (char ch : choiceLine) {
            if (!std::isdigit(static_cast<unsigned char>(ch))) {
                numeric = false;
                break;
            }
        }
        if (!numeric) {
            std::cout << "Invalid option. Please enter 1, 2, 3, or 9." << std::endl;
            continue;
        }

        int choice = std::stoi(choiceLine);
        if (choice == 1) {
            std::cout << "Enter the file name to load: " << std::endl;
            std::string filename;
            if (!safeGetline(filename)) {
                std::cout << std::endl << "Input closed. Exiting." << std::endl;
                break;
            }
            filename = trim(filename);
            if (filename.empty()) {
                std::cout << "Error: file name cannot be empty." << std::endl;
                continue;
            }

            std::vector<std::string> warnings;
            bool ok = loadCoursesFromFile(filename, courses, warnings);
            if (ok) {
                dataLoaded = true;
                std::cout << "Data loaded successfully from " << filename << std::endl;
                if (!warnings.empty()) {
                    std::cout << "Note: Some lines were skipped or had issues:" << std::endl;
                    for (const std::string& w : warnings) {
                        std::cout << "  - " << w << std::endl;
                    }
                }
            } else {
                dataLoaded = false;
            }
        } else if (choice == 2) {
            if (!dataLoaded) {
                std::cout << "Please load data first using option 1." << std::endl;
                continue;
            }
            printSortedCourseList(courses);
        } else if (choice == 3) {
            if (!dataLoaded) {
                std::cout << "Please load data first using option 1." << std::endl;
                continue;
            }
            std::cout << "Enter a course ID (e.g., CSCI300): " << std::endl;
            std::string query;
            if (!safeGetline(query)) {
                std::cout << std::endl << "Input closed. Exiting." << std::endl;
                break;
            }
            printCourseInfo(courses, query);
        } else if (choice == 9) {
            std::cout << "Thank you for using the ABCU CS Advising Assistant. Goodbye!" << std::endl;
            break;
        } else {
            std::cout << "Invalid option. Please enter 1, 2, 3, or 9." << std::endl;
        }
    }

    return 0;
}
