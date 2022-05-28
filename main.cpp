#include <iostream>
#include <fstream>
#include <vector>
#include <list>
using namespace std;

// Class of Users/ Nodes of Waiting Queue and Access List
class User
{
public:
	int key;
	int user_Id;
	char operation;

	User()
	{
		key = '\0';
		user_Id = '\0';
		operation = '\0';
	}

	User(int k)
	{
		key = k;
		user_Id = '\0';
		operation = '\0';
	}

	User(int k, int id, char op)
	{
		key = k;
		user_Id = id;
		operation = op;
	}

	bool operator < (const User& d)
	{
		if (key < d.key)
			return true;
		else if (key >= d.key)
			return false;
	}

	bool operator > (const User& d)
	{
		if (key > d.key)
			return true;
		else if (key <= d.key)
			return false;
	}
};

// Class of Priority Queue Waiting List
class PriorityQ
{
private:
	int min = 101;
	int PARENT(int i) {
		return (i - 1) / 2;
	}
	int LEFT(int i) {
		return (2 * i + 1);
	}
	int RIGHT(int i) {
		return (2 * i + 2);
	}

	// Recursive heapify down algo
	void heapify_down(int i)
	{
		// get left and right child of node at index `i`
		int left = LEFT(i);
		int right = RIGHT(i);

		int largest = i;

		// compare `A[i]` with its left and right child
		// and find the largest value
		if (left < size() && myQ[left] > myQ[i]) {
			largest = left;
		}

		if (right < size() && myQ[right] > myQ[largest]) {
			largest = right;
		}

		// swap with a child having greater value and
		// call heapify-down on the child
		if (largest != i)
		{
			swap(myQ[i], myQ[largest]);
			heapify_down(largest);
		}
	}

	// Recursive heapify-up algorithm
	void heapify_up(int i)
	{
		// check if the node at index `i` and its parent violate the heap property
		if (i && myQ[PARENT(i)] < myQ[i])
		{
			// swap the two if heap property is violated
			swap(myQ[i], myQ[PARENT(i)]);

			// call heapify-up on the parent
			heapify_up(PARENT(i));
		}
	}

public:
	vector<User> myQ;
	int size() {
		return myQ.size();
	}
	// Function to check if the heap is empty or not
	bool empty() {
		return myQ.empty();
	}

	// insert key into the heap
	void Insert(User key)
	{
		// first check the min element and change accordingly
		if (key < min)
			min = key.key;

		// insert a new element at the end of the Queue
		myQ.push_back(key);

		// get element index and sort by heapify up
		int index = size() - 1;
		heapify_up(index);
	}

	// Function to remove an element with the highest priority (present at the root)
	User ExtractMax()
	{
		// if the heap has no elements, throw an exception
		if (empty())
		{
			cout << "Error: Queue is Empty" << endl;
			return NULL;
		}
		else
		{
			// save the max element before popping it out
			User temp = myQ[0];
			// replace the root of the heap with the last element
			// of the vector
			myQ[0] = myQ.back();
			myQ.pop_back();

			// call heapify-down on the root node
			heapify_down(0);
			return temp;
		}
	}

	// Function to return an element with the highest priority (present at the root)
	int FindMax()
	{
		// if the heap has no elements, throw an exception
		if (empty())
		{
			return -1;
		}
		// otherwise, return the top (first) element
		return myQ[0].key;
	}

	int FindMin()
	{
		// if the heap has no elements, throw an exception
		if (empty())
		{
			return -1;
		}
		// otherwise, return the min variable
		return min;
	}

	void Print()
	{
		cout << "Printing --> ";
		for (int i = 0; i < size(); i++)
			cout << myQ[i].key << " ";

		cout << endl;
	}

	// helper function to use in Print, throws Max User ID
	int frontUserID()
	{
		return myQ.front().user_Id;
	}

	// throws Max User Op
	char frontUserOp()
	{
		return myQ.front().operation;
	}


};

// Class of File 
class File
{
public:

	PriorityQ* myQ;
	list<User> current_access;
	int id;
	char status;
	File()
	{
		id = '\0';
		status = 'e';
		myQ = new PriorityQ;
	}
};

// Class which represents main database
class Repository
{
private:
	list<File> *Database;
	int num_of_files;
	int curr_no_files;

	// Function to Calculate Hash Key
	int calcHashKey(int key)
	{
		return key % num_of_files;
	}

	// Function to search file and return true if found in desired Chain
	bool searchFile(int id)
	{
		int hash = calcHashKey(id);
		for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
		{
			if (it->id == id)
			{
				return true;
			}
		}
		return false;
	}

	// Function to search User in desired chain
	bool searchUser(int Userid, int FileID)
	{
		int hash = calcHashKey(FileID);
		for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
		{
			if (it->id == FileID)
			{
				for (auto k = it->current_access.begin(); k != it->current_access.end(); ++k)
				{
					if (k->user_Id == Userid)
						return true;
				}
			}
		}
		return false;
	}

	// helper function & modified copy of RequestFileAccess Function (For Load From File)
	void InsertUserInside(int FileId, int UserId, int priority, char op)
	{
		// first check is File exists or not
		if (searchFile(FileId) == false && searchUser(UserId, FileId) == false)
		{
			cout << "Error: Such a File does not exist or User ID already used. " << endl;
		}
		else
		{
			// Insertion Part
			int hash = calcHashKey(FileId);
			for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
			{
				if (it->id == FileId)
				{
					User temp(priority, UserId, op); // Insertion with Cases 
					if (it->myQ->empty() && it->current_access.empty()) // case: if queue and access list empty
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else if (it->current_access.front().operation == 'W') // case: if access has a W User
					{
						it->myQ->Insert(temp); // throw in waiting queue
					}
					else if (it->myQ->empty() && it->current_access.front().operation == 'R' && op == 'R') // case: if queue empty and access has R user(s) and temp is also R
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else if (it->myQ->FindMax() <= priority && it->current_access.front().operation == 'R' && op == 'R') // case: if temp has high priority (and is R user) than Max User in Queue and access has only R user(s)
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else
						it->myQ->Insert(temp); // throw in waiting queue
				}
			}
		}
	}

public:
	// Constructor
	Repository() 
	{ 
		num_of_files = 10;
		Database = new list<File>[num_of_files];
		curr_no_files = 0;
	}

	// Parameterized Constructor
	Repository(int k) 
	{ 
		num_of_files = k; 
		Database = new list<File>[num_of_files];
		curr_no_files = 0;
	}

	// Function to Insert File
	void InsertFile(int id)
	{
		if (searchFile(id) == false) // first check whether file already exists
		{
			// calculating hash key then push File within Database on that key
			int index = calcHashKey(id);
			File temp;
			temp.id = id;
			temp.status = 'o';
			Database[index].push_back(temp);
			curr_no_files++;
		}
		else
			cout << "File Already Exists." << endl;
	}

	// Funcion to Delete File
	void DeleteFile(int id)
	{
		bool found = false;
		int hash = calcHashKey(id);
		auto it = Database[hash].begin();
		for (; it != Database[hash].end(); ++it) // Files
		{
			if (it->id == id)
			{
				found = true;
				if (it->current_access.empty())
				{
					cout << "Deleted." << endl;
					it->status = 'e';
				}
				else
					cout << "Can't Delete: FIle Currently being Accessed." << endl;

				break;
			}
		}
		if (found && it->current_access.empty())
			Database[hash].erase(it);
		else if (!found)
			cout << "File Doesn't Exist." << endl;
	}

	// Function to Request File Access
	void RequestFileAccess(int UserId, int FileId)
	{
		// first check is File exists or not
		if (searchFile(FileId) == false)
		{
			cout << "Error: Such a File does not exist. " << endl;
		}
		else
		{
			int priority = -1, decision = 0;
			char op;
			cout << "Operation ('R' or 'W') : ";
			cin >> op; cout << endl;
			cout << "Select Priority:" << endl;
			cout << "1 - Custom. " << "\t" << "2 - Highest. " << "\t" << "3 - Lowest. " << endl << "Decision: ";
			cin >> decision;

			// Priority Selection Cases

			if (decision == 1) // For Custom Priority
			{
				cout << "Enter Priority ( 0 - 100 ): ";
				cin >> priority; cout << endl;
			}
			else if (decision == 2) // For Highest Priority
			{
				int hash = calcHashKey(FileId);
				for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
				{
					if (it->id == FileId)
					{
						priority = it->myQ->FindMax() + 1;
						if (priority > 100) // if Priority hits over max cap then turn it to 100
							priority = 100;
					}
				}
			}
			else if (decision == 3) // For Lowest Priority
			{
				int hash = calcHashKey(FileId);
				for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
				{
					if (it->id == FileId)
					{
						priority = it->myQ->FindMin() - 1;
						if (priority < 0) // if Priority hits lower min cap then turn it to 0
							priority = 0;
					}
				}
			}

			// Insertion Part
			int hash = calcHashKey(FileId);
			for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it)
			{
				if (it->id == FileId)
				{
					User temp(priority, UserId, op); // Insertion with Cases 
					if (it->myQ->empty() && it->current_access.empty()) // case: if queue and access list empty
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else if (it->current_access.front().operation == 'W') // case: if access has a W User
					{
						it->myQ->Insert(temp); // throw in waiting queue
					}
					else if (it->myQ->empty() && it->current_access.front().operation == 'R' && op == 'R') // case: if queue empty and access has R user(s) and temp is also R
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else if (it->myQ->FindMax() <= priority && it->current_access.front().operation == 'R' && op == 'R') // case: if temp has high priority (and is R user) than Max User in Queue and access has only R user(s)
					{
						it->current_access.push_back(temp); // give immediate access
					}
					else
						it->myQ->Insert(temp); // throw in waiting queue
				}
			}
		}
	}

	// Function to Release File Access
	void ReleaseFile(int UserID, int FileID)
	{
		if (searchFile(FileID) == false && searchUser(UserID, FileID) == false) // first check to see if user exists in current access or file exists 
		{
			cout << "Error: File or User doesn't exist." << endl;
		}
		else
		{
			int hash = calcHashKey(FileID);
			for (auto it = Database[hash].begin(); it != Database[hash].end(); ++it) // Files
			{
				if (it->id == FileID)
				{
					for (auto k = it->current_access.begin(); k != it->current_access.end(); ++k) // Users in Access
					{
						if (k->user_Id == UserID)
						{
							it->current_access.erase(k);  // remove the wanted user from access
							if (it->current_access.empty() && it->myQ->empty() == false) // if access empty bring new users to access
							{
								if (it->myQ->frontUserOp() == 'W') // if front user is W then throw it in
									it->current_access.push_back(it->myQ->ExtractMax());
								else // else take in all R users in front
								{
									for (auto j = it->myQ->myQ.begin(); j != it->myQ->myQ.end(); ++j) // loop to add all Read Users in Front
									{
										if (it->myQ->frontUserOp() == 'R')
											it->current_access.push_back(it->myQ->ExtractMax());
									}
								}
							}
							break;
						}
					}
				}
			}

		}
	}

	// Function to Print Hash Table 
	void Print()
	{
		for (int i = 0; i < 10; i++)
		{
			for (auto it = Database[i].begin(); it != Database[i].end(); ++it) // File
			{
				if (it->status == 'o') // if hash space (File) occupied
				{
					cout << "H" << i << " -> File " << it->id << " ... Access Granted to";
					if (it->current_access.empty())
						cout << " None " << endl;
					else
					{
						for (auto k = it->current_access.begin(); k != it->current_access.end(); ++k) // current users
						{
							cout << " User " << k->user_Id << ", ";
						}

						if (it->current_access.begin()->operation == 'R')
							cout << "Read" << endl;
						else
							cout << "Write" << endl;
					}

					cout << "Next "; // Next User and Waiting Queue
					if (it->myQ->empty())
					{
						cout << "None" << endl << "Waiting None" << endl;
					}
					else
					{
						cout << "User " << it->myQ->frontUserID() << ", ";
						if (it->myQ->frontUserOp() == 'R')
							cout << "Read" << endl;
						else
							cout << "Write" << endl;

						cout << "Waiting " << it->myQ->size() << " users." << endl;

					}
				}
			}
		}
	}

	// Function to Load Data from File
	void LoadfromFile()
	{
		ifstream Read;
		Read.open("input.txt");
		if (!Read)
		{
			cout << "Can't Find File." << endl;
			cout << "Loading Failed" << endl;
		}
		else
		{
			int file_id, user_id, priority; 
			char operation;
			while (!Read.eof())
			{
				// take in values
				Read >> file_id;
				Read.ignore();
				Read >> user_id;
				Read.ignore();
				Read >> priority;
				Read.ignore();
				Read >> operation;
				Read.ignore();

				// Create File
				this->InsertFile(file_id);
				// Insert User inside that File
				this->InsertUserInside(file_id, user_id, priority, operation);
			}
			cout << "Loaded Successfully." << endl;
		}
		Read.close();
	}

	// Menu Function to Access Database
	void Menu()
	{
		cout << "--------------------------------------------------------" << endl;
		cout << "              Welcome to User DataBase Menu             " << endl;
		cout << "--------------------------------------------------------" << endl;

		int decision = -1;
		while (decision != 0)
		{
			cout << "--------------------------------------------------------" << endl;
			cout << "1 - Print Database." << endl;
			cout << "2 - Load Data from File." << endl;
			cout << "3 - Insert a File." << endl;
			cout << "4 - Delete a File." << endl;
			cout << "5 - Request Access for a File." << endl;
			cout << "6 - Release Access of User." << endl;
			cout << "0 - Exit." << endl;
			cout << "--------------------------------------------------------" << endl;
			cout << "Enter: ";
			cin >> decision;
			
			cout << "--------------------------------------------------------" << endl;
			if (decision == 1) // Print
			{
				if (curr_no_files == 0)
					cout << "There are 0 Files." << endl;
				else
					this->Print();
			}
			else if (decision == 2) // Load From File
			{
				this->LoadfromFile();
			}
			else if (decision == 3) // Insert File
			{
				int id;
				cout << "File ID: ";
				cin >> id;
				this->InsertFile(id);
			}
			else if (decision == 4) // Delete File
			{
				int id;
				cout << "File ID: ";
				cin >> id;
				this->DeleteFile(id);
			}
			else if (decision == 5)
			{
				int user_id, file_id;
				cout << "User ID: ";
				cin >> user_id;
				cout << "File ID: ";
				cin >> file_id;
				this->RequestFileAccess(user_id, file_id);
			}
			else if (decision == 6)
			{
				int user_id, file_id;
				cout << "User ID: ";
				cin >> user_id;
				cout << "File ID: ";
				cin >> file_id;
				this->ReleaseFile(user_id, file_id);
			}
			cout << "--------------------------------------------------------" << endl;
		}

	}


	// Destructor
	~Repository()
	{
		delete[] Database;
		Database = nullptr;
	}


};

int main()
{
	Repository Rep;
	Rep.Menu();

	system("pause");
	return 0;
}