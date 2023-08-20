//============================================================================
// Name        : ABCU Courses Project Two
// Author      : D.D. Sessions
// Version     : 1.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : 
//============================================================================


#include <algorithm>
#include <iostream>
#include <vector>

#include "CSVparser.hpp"

using namespace std;

// forward declarations
const unsigned int DEFAULT_SIZE = 10;

// define a data structure to hold course information and pre-requisites
struct PreRequisite {
    string Id;
};

struct Course {
    string Id;
    string Name;
    vector<PreRequisite> PreRequisites;
};

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 *
*/
class HashTable {
private:
    // structure to hold Courses
    struct Node {
        Course course;
        unsigned int key;
        Node* next;

        // default constructor
        Node() {
			key = UINT_MAX;
			next = nullptr;
		}

        // initialize with a course
        Node(Course aCourse) : Node() {
            course = aCourse;
        }

        // initialize with a course and a key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
			key = aKey;
		}
    };

    vector<Node> table;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(string key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course Course);
    void PrintCoursesOrder();
    Course Search(string courseID);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    table.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory
 */
HashTable::HashTable(unsigned int size) {
    this->tableSize = size;
    table.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    table.erase(table.begin());
}

/**
 * Calculate the hash value of a given key
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(string key) {
    int sum = 0;

    for (int i = 0; i < key.length(); i++) {
		sum += (int)key[i];
	}
    return sum % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // convert the course id to an integer and pass to the hash function
    transform(course.Id.begin(), course.Id.end(), course.Id.begin(), ::toupper);
    unsigned key = hash(course.Id);

    // check if a course already exists at the key location
    Node* oldNode = &(table.at(key));

    // if not, create a new node and insert it
    if (oldNode == nullptr) {
        Node* newNode = new Node(course, key);

        // insert this node to the key position
        table.insert(table.begin() + key, *newNode);
    }

    // else if node is not used
    else {

        // assign old node key
        if (oldNode->key == UINT_MAX) {
            oldNode->key = key;
            oldNode->course = course;
            oldNode->next = nullptr;
        }

        // else find the next available node
        else {
            while (oldNode->next != nullptr) {
                oldNode = oldNode->next;
            }

            // insert the new node at the end
            oldNode->next = new Node(course, key);
        }
    }
}

/**
 * Print courses in alpha-numerical order
 *
 */
void HashTable::PrintCoursesOrder() {
    vector<Course> courses;

    // iterate through the table
    for (auto it = table.begin(); it != table.end(); ++it) {
        // if the key is not equal to UINT_MAX
        if (it->key != UINT_MAX) {
            // add the course to the vector
			courses.push_back(it->course);
            // create a node pointer to the next node
            Node* node = it->next;

            // while the node is not null
            while (node != nullptr) {
				// add the course to the vector
				courses.push_back(node->course);
                // move to the next node
				node = node->next;
			}
		}
    }

    // sort the vector
    int min;
    int size = courses.size();
    // iterate through the vector
    for (int i = 0; i < size; i++) {
        // set the min to the current index
		min = i;
        // iterate through the vector starting at the next element
        for (int j = i + 1; j < size; j++) {
            // if the course id is less than the current min
            if (courses[j].Id.compare(courses[min].Id) < 0) {
                // set the min to the current index
                min = j;
            }
		}

        // if the min is not equal to the current index
        if (min != i) {
            // swap the current index with the min
            std::swap(courses.at(i), courses.at(min));
        }
	}
    
    // print the courses
	for (int i = 0; i < size; i++) {
		cout << courses[i].Id << ", " << courses[i].Name << endl;
	}
}

/**
 * Search for the specified courseId
 *
 * @param courseId The course id to search for
 */
Course HashTable::Search(string courseId) {
    transform(courseId.begin(), courseId.end(), courseId.begin(), ::toupper);
    Course course;

    // get the key
    unsigned int key = hash(courseId);
    Node* node = &(table.at(key));

    // if the key is not equal to UINT_MAX and the course id is equal to the search id
    if (node != nullptr && node->key != UINT_MAX && node->course.Id.compare(courseId) == 0) {

        // return the course
        return node->course;
    }

    // if the key is equal to UINT_MAX or null
    if (node == nullptr || node->key == UINT_MAX) {

        // return the course
        return course;
    }

    // while the node is not null
    while (node != nullptr) {

        // if the course id is equal to the search id
        if (node->key != UINT_MAX && node->course.Id.compare(courseId) == 0) {
            // return the course
            return node->course;
        }

        // move to the next node
        node = node->next;
    }

    // return the course
    return course;
}

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourses(string csvPath, HashTable* hashtable) {
    cout << "Loading CSV file " << csvPath << endl;

    // create a CSV Parser instance with the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {
            // create a course and add it to the container
            Course course;
            vector<PreRequisite> prereqs;
            course.Id = file[i][0];
            course.Name = file[i][1];

            // get the number of columns in the current row
            int rowColumnCount = file.getRow(i).size();

            // loop to read columns of a CSV file for prerequisites
            for (unsigned int j = 2; j < rowColumnCount; j++) {
                PreRequisite prereq;
                prereq.Id = file[i][j];
                prereqs.emplace_back(prereq);
			}

            course.PreRequisites = prereqs;
            // add the course to the end
			hashtable->Insert(course);
        }
    } catch (csv::Error& e) {
		std::cerr << e.what() << std::endl;
	}
}

/**
 * Display the course information
 *
 * @param course The course to display
 */
void displayCourseInfo(Course course) {
    // print the course id and name
    cout << course.Id << ", " << course.Name << endl;
    // if the course has prerequisites
    if (course.PreRequisites.size() > 0) {
        // print the prerequisites
        cout << "Prerequisites: ";
        string delim = "";
        // iterate through the prerequisites
        for (auto& preReq : course.PreRequisites) {
            cout << delim << preReq.Id;
            delim = ", ";
        }
        cout << endl;
    }
}

/**
 * Display the welcome message and return the CSV file path
 *
 */
string theWelcome() {
    string fileName;

    cout << "Welcome to the course planner." << endl;

    cout << "Please enter the name of the file you would like to read from: ";

    cin >> fileName;

    return fileName;
}

/**
 * Display the menu
 *
 */
void theMenu() {
	cout << "  1. Load Data Structure" << endl;
	cout << "  2. Print Course List" << endl;
	cout << "  3. Print Course" << endl;
	cout << "  9. Exit\n" << endl;
    cout << "What would you like to do? ";
}

/**
 * Main entry point into the program
 *
 */
int main() {
    string csvPath = theWelcome();
    //string csvPath = "course-list.csv";
    system("cls");
    
    // create a hash table
    HashTable* courseTable = new HashTable();
    Course course;
    string courseId;
    cout << "Welcome to the course planner.\n" << endl;

    int choice = 0;
    // loop until the user chooses to exit
    while (choice != 9) {
        theMenu();
        cin >> choice;

        switch (choice) {
        case 1:
            // load the courses
            loadCourses(csvPath, courseTable);
            cout << "Data Loaded!\n" << endl;
            break;

        case 2:
            cout << "Here is a sample schedule:\n" << endl;
            // print the courses
            courseTable->PrintCoursesOrder();
            cout << endl;
            break;

        case 3:
            cout << "What course do you want to know about? ";
            cin >> courseId;
            // search for the course
            course = courseTable->Search(courseId);

            // if the course id is not empty
            if (!course.Id.empty()) {
                // display the course information
                displayCourseInfo(course);
            } else {
                cout << "Course Id " << courseId << " not found." << endl;
            }
            cout << endl;
            break;

        case 9:
			break;

        default:
            cout << choice << " is not a valid option\n" << endl;
            break;
        }
    }

    cout << "Thank you for using the course planner!" << endl;

    return 0;
}
