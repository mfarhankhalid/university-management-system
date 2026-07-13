#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

static int idCounter = 1000;

class User {
protected:
    string name, email, password;
    int ID;

public:
    User() {
	 ID = idCounter++; 
	 }

    void setName(string n) {
	 name = n; 
	 }
    void setEmail(string e) {
	 email = e; 
	 }
    void setPassword(string p) { 
	password = p; 
	}

    string getEmail() const { 
	return email; 
	}
    int getID() const {
	 return ID; 
	 }
    string getName() const { 
	return name; 
	}

    virtual bool login(const string& e, const string& p) {
        return (e == email && p == password);
    }

    virtual void accessPortal() = 0;
    virtual void displayInfo() = 0;
    virtual ~User() {}
};


class Course {
private:
    string code, title, schedule;
    int credits;
    map<int, char> grades; // student ID -> grade

public:
    Course(string c, string t, int cr, string s) : code(c), title(t), credits(cr), schedule(s) {}

    string getCode() const {
	 return code; 
	 }
    void display() const {
        cout << code << " - " << title << " | Credits: " << credits << " | Schedule: " << schedule << endl;
    }

    void assignGrade(int studentID, char grade) {
        grades[studentID] = grade;
    }

    void viewGrades() {
        for (auto& g : grades)
            cout << "Student ID: " << g.first << " -> Grade: " << g.second << endl;
    }
};


class Student : public User {
private:
    vector<Course*> registered;
    vector<Course*> dropped;

public:
    void registerCourse(Course* course) {
        if (find(registered.begin(), registered.end(), course) == registered.end()) {
            registered.push_back(course);
            cout << course->getCode() << " registered successfully.\n";
        } else cout << "Already registered.\n";
    }

    void dropCourse(Course* course) {
        auto it = find(registered.begin(), registered.end(), course);
        if (it != registered.end()) {
            dropped.push_back(course);
            registered.erase(it);
            cout << course->getCode() << " dropped.\n";
        } else cout << "Not found.\n";
    }

    void reRegisterCourse(Course* course) {
        auto it = find(dropped.begin(), dropped.end(), course);
        if (it != dropped.end()) {
            registered.push_back(course);
            dropped.erase(it);
            cout << course->getCode() << " re-registered.\n";
        } else cout << "Not found in dropped list.\n";
    }

    void viewRegistered() {
        cout << "\nRegistered Courses:\n";
        for (auto c : registered) c->display();
    }

    void viewDropped() {
        cout << "\nDropped Courses:\n";
        for (auto c : dropped) c->display();
    }

    void accessPortal() override {
        cout << "Welcome to Student Portal, " << name << "!\n";
    }

    void displayInfo() override {
        cout << "ID: " << ID << " | Name: " << name << " | Email: " << email << endl;
    }
};


class Faculty : public User {
private:
    vector<Course*> teaching;

public:
    void addCourse(Course* c) { 
	teaching.push_back(c); 
	}

    void assignGrade(Course* c, int studentID, char grade) {
        c->assignGrade(studentID, grade);
        cout << "Grade assigned.\n";
    }

    void viewRoster(Course* c) {
        cout << "Viewing roster for " << c->getCode() << ":\n";
        c->viewGrades();
    }

    void accessPortal() override {
        cout << "Welcome to Faculty Portal, Prof. " << name << "!\n";
    }

    void displayInfo() override {
        cout << "ID: " << ID << " | Name: " << name << " | Email: " << email << endl;
    }
};


class SystemManager {
private:
    vector<Student*> students;
    vector<Faculty*> faculties;
    vector<Course*> courses;

public:
    void addSampleCourses() {
        courses.push_back(new Course("CS101", "Intro to CS", 3, "Mon-Wed"));
        courses.push_back(new Course("ENG102", "Expository Writing", 4, "Tue-Thu"));
        courses.push_back(new Course("PHY103", "Physics", 3, "Fri"));
    }

    void showCatalog() {
        cout << "\nCourse Catalog:\n";
        for (auto c : courses) c->display();
    }

    Course* findCourse(string code) {
        for (auto c : courses) if (c->getCode() == code) return c;
        return nullptr;
    }

    void signupAndLogin() {
        int role;
        cout << "Select Role: 1-Student 2-Faculty\nChoice: ";
        cin >> role; cin.ignore();

        User* user = nullptr;
        if (role == 1) user = new Student();
        else user = new Faculty();

        string input;
        cout << "\nSign Up:\nName: "; getline(cin, input); user->setName(input);
        cout << "Email: "; getline(cin, input); user->setEmail(input);
        cout << "Password: "; getline(cin, input); user->setPassword(input);
        cout << "Assigned ID: " << user->getID() << endl;

        cout << "\nLogin:\nEmail: "; getline(cin, input);
        string email = input;
        cout << "Password: "; getline(cin, input);
        string pass = input;

        if (!user->login(email, pass)) {
            cout << "Login Failed.\n"; return;
        }

        user->accessPortal();
        user->displayInfo();

        if (role == 1) {
            Student* s = dynamic_cast<Student*>(user);
            students.push_back(s);
            int ch;
            do {
                cout<<" 1.View Catalog"<<endl;
				cout<<" 2.Register"<<endl;
				cout<<" 3.Drop"<<endl;
				cout<<" 4.View Registered"<<endl;
				cout<<" 5.View Dropped"<<endl; 
				cout<<" 6.Re-register"<<endl;
				cout<<" 0.Exit\nChoice: "<<endl;
                cin >> ch; cin.ignore();
                string code;
                switch(ch) {
                    case 1: showCatalog(); break;
                    case 2:
                        cout << "Course Code: "; getline(cin, code);
                        if (Course* c = findCourse(code)) s->registerCourse(c);
                        else cout << "Invalid code.\n"; break;
                    case 3:
                        cout << "Course Code: "; getline(cin, code);
                        if (Course* c = findCourse(code)) s->dropCourse(c);
                        else cout << "Invalid code.\n"; break;
                    case 4: s->viewRegistered(); break;
                    case 5: s->viewDropped(); break;
                    case 6:
                        cout << "Course Code: "; getline(cin, code);
                        if (Course* c = findCourse(code)) s->reRegisterCourse(c);
                        else cout << "Invalid code.\n"; break;
                }
            } while (ch != 0);
        }
        else {
            Faculty* f = dynamic_cast<Faculty*>(user);
            faculties.push_back(f);
            for (auto c : courses) f->addCourse(c);
            int ch;
            do {
                cout << "\n1. View Catalog\n"<<endl;
				cout<<"2. Assign Grade\n"<<endl;
				cout<<"3. View Roster\n"<<endl;
				cout<<"0. Exit\nChoice: "<<endl;

                cin >> ch; cin.ignore();
                string code; int sid; char grade;
                switch(ch) {
                    case 1: showCatalog(); break;
                    case 2:
                        cout << "Course Code: "; getline(cin, code);
                        cout << "Student ID: "; cin >> sid;
                        cout << "Grade: "; cin >> grade; cin.ignore();
                        if (Course* c = findCourse(code)) f->assignGrade(c, sid, grade);
                        else cout << "Invalid code.\n"; break;
                    case 3:
                        cout << "Course Code: "; getline(cin, code);
                        if (Course* c = findCourse(code)) f->viewRoster(c);
                        else cout << "Invalid code.\n"; break;
                }
            } while (ch != 0);
        }
    }
};


int main() {
    SystemManager sys;
    sys.addSampleCourses();
    sys.signupAndLogin();
    return 0;
}