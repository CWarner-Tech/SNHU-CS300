//============================================================================
// Name        : Project Two
// Author      : Courtney Warner
// Description : Hash Table recommendation
// Course: CS-300-R4812 DSA: Analysis and Design 24EW4
//============================================================================

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//Define a structure to hold course information
struct Course {
	string courseNumber;//Course Number
	string title;//Course title
	vector<string> prerequisites;;//Vector to store prerequisites

	//Constructor with default values for parameters
	Course(string courseNumber = "", string title = "", vector<string> 
		prerequisites = {}) : courseNumber(courseNumber), title(title),
		prerequisites(prerequisites){}
};

//Define a class for the hash table
class HashTable {
private://Deifne a structure for a node in the hash table
	struct Node {
		Course course;//Course information
		unsigned int key;//Key for hashing
		Node* next;//Point to next node

		//Default constructor
		Node() : key(UINT_MAX), next(nullptr) {}

		//Constructure with parameters
		Node(Course aCourse, unsigned int aKey) : course(aCourse), key(aKey),
			next(nullptr) {}
	};

	vector < Node*>nodes;//Holds nodes
	unsigned int tableSize = 179;//Possible size of the hash table

	//Calculate the hash value of a key
	unsigned int hash(const string& key);
public:
	HashTable() { nodes.resize(tableSize, nullptr); }//Constructor 
	~HashTable();//Destructor

	void insert(const Course& course); //Method to insert a course
	void printCourseList();//Method to print all courses
	Course search(const string& courseNumber);//Method to search for a course
};

//Destructor to free allocated memory
HashTable::~HashTable() {
	for (Node* node : nodes) {
		while (node != nullptr) {
			Node* next = node->next;
			delete node;
			node = next;
		}
	}
}

//Calculate the hash value of a given key
unsigned int HashTable::hash(const string& key) {
	unsigned int hash = 0;
	for (char ch : key) {
		hash = hash * 31 + tolower(ch);//Convert character to lowercase and calculate hash
	}
	return hash % tableSize; //Returns hash value within the table size
}
//Insert a course into the hash table
void HashTable::insert(const Course& course) {
	unsigned int key = hash(course.courseNumber);//Calculate the hash key for course

	Node* newNode = new Node(course, key);// Create new node with course information
	//If the slot is empty, insert the new node
	if (nodes[key] == nullptr) {
		nodes[key] = newNode;
	}
	//Otherwise, find the last node and append the new node
	else {
		Node* oldNode = nodes[key];
		while (oldNode->next != nullptr) {
			oldNode = oldNode->next;
		}
		oldNode->next = newNode;
	}
}

//Print all courses in the hash table in alphnumeric order
void HashTable::printCourseList() {
	vector<Course> allCourses;

	//Collect all courses into the vector
	for (Node* node : nodes) {
		while (node != nullptr) {
			allCourses.push_back(node->course);
			node = node->next;
		}
	}

	//Sort courses in alphanumeric order
	sort(allCourses.begin(), allCourses.end(), [](const Course& a, const Course& b) {
		if (a.courseNumber != b.courseNumber) {
			return a.courseNumber < b.courseNumber;
		}
		return a.title < b.title;
		});
	//Print sorted course list
	for (const auto& course : allCourses) {
		cout << course.courseNumber << ", " << course.title << endl;
	}
}

//Search for a course by course number
Course HashTable::search(const string& courseNumber) {
	unsigned int key = hash(courseNumber);
	Node* node = nodes[key];

	while (node != nullptr) {
		string nodeCourseNumber = node->course.courseNumber;
		bool match = true;
		for (size_t i = 0; i < courseNumber.size(); ++i) {
			if (tolower(nodeCourseNumber[i]) != tolower(courseNumber[i])) {
				match = false;
				break;
			}
		}
		if (match) {
			return node->course;
		}
		node = node->next;
	}

	return Course("", "", {});
}

//Load course data from a CSV file into the hash table
void openAndReadFile(const string& filename, HashTable& hashtable) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error: Unable to open file " << filename << endl;
		return;
	}

	string line;
	getline(file, line); //Skip header line

	while (getline(file, line)) {
		stringstream ss(line);
		string courseNumber, title, prerequisite;
		
		getline(ss, courseNumber, ',');
		getline(ss, title, ',');
		vector<string> prerequisites;
		while (getline(ss, prerequisite, ',')) {
			if (!prerequisite.empty()) {
				prerequisites.push_back(prerequisite);
			}
		}
		Course course(courseNumber, title, prerequisites);
		hashtable.insert(course);
	}
	file.close();
}

//Main function
int main() {
	HashTable courseTable;//Create a has table to hold courses
	string filename = "Course_List.csv";//CSV file containing course data

	int choice = 0;
	string courseNumber;

	cout << "Welcome to the course planner." << endl;

	//Main menu loop
	while (choice != 9) {
		//Display menu options
		cout << "1. Load Data Structure." << endl;
		cout << "2. Print Course List." << endl;
		cout << "3. Print Course." << endl;
		cout << "9. Exit" << endl;
		cout << "What would you like to do? ";

		//Check if input is not an integer
		if (!(cin >> choice)) {
			//Clear input buffer and ignore invalid input
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "That is not a valid option. Please select a valid number." << endl;
			continue;
		}

		//Perform action based on user choice
		switch (choice) {
		case 1:
			openAndReadFile(filename, courseTable); // Load course data from CSV file
			cout << "Data structure loaded." << endl;
			break;

		case 2:
			cout << "Here is a simple schedule:" << endl;
			courseTable.printCourseList();// Print all courses in the hash table
			break;

		case 3:
			cout << "What course do you want to know about? ";
			cin >> courseNumber;
			{
				Course course = courseTable.search(courseNumber);// Search for course
				if (course.courseNumber.empty()) {
					cout << "Course not found." << endl; //Not a course option
				}
				else {
					cout << course.courseNumber << ", " << course.title << endl;
					cout << "Prerequisites:";
					for (const auto& pre : course.prerequisites) {
						cout << " " << pre;
					}
					cout << endl;
				}
			}
			break;

		case 9://Exit
			cout << "Thank you for using the course planner!" << endl;
			break;

		default: // If choice an integer but not a choice option
			cout << choice << " is not a valid option." << endl;
			break;
		}
	}
	return 0;
}