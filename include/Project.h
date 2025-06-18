#pragma once

#include "Issue.h"
#include <vector>

#include "DateFunctions.h"

// Project class, the main entity for managing project-related data and operations
class Project {
	friend Feature;
	unsigned ID;
	char name[81];
	char description[2001];
	char dateCreated[11];

	std::vector<std::shared_ptr<Issue>> toDoList;
	std::vector<std::shared_ptr<Issue>> inProgress;
	std::vector<std::shared_ptr<Issue>> inReview;
	std::vector<std::shared_ptr<Issue>> done;
	std::vector<std::shared_ptr<ProjectMember>> projectMembers;
	
	void saveList();
	void saveList(std::vector<std::shared_ptr<Issue>>&, const std::string&);
	void loadList();
	void loadList(std::vector<std::shared_ptr<Issue>>&, const std::string&);
	void makeFilteredList(std::vector<std::shared_ptr<Issue>>&, std::vector<std::shared_ptr<Issue>>&, bool*, short&);
	void makeRoleFilteredList(std::vector<std::shared_ptr<Issue>>&, std::vector<std::shared_ptr<Issue>>&, short&);
	void makeExpandedList(std::vector<std::shared_ptr<Issue>>&, std::vector<std::shared_ptr<Issue>>&, short&);
public:
	unsigned getID() { return ID; }
	void setID(unsigned ID) { this->ID = ID; }
	std::string getName() { return name; }
	std::string getDescription() { return description; }
	std::string getDateCreated() { return dateCreated; }
	void setName(std::string);
	void setDescription(std::string);
	// Set the current date as the creation date
	void setCurrentDateAsDateCreated() { dateCurrent().copy(this->dateCreated, 11); }
	void setDateCreated(std::string dateCreated);
	void loadProject();
	void saveAllLists();
	void rewriteToFile();
	void create();
	void actions();
	void help();
	void showProjectInfo();
	void changeProjectDescription();
	void changeProjectName();
	void showAllProjectMembers();
	void removeProjectMember();
	void addProjectMember();
	void changeProjectMemberRole();
	bool removeProject();
	void addFeature();
	void addTask();
	void addBug();
	void showFilteredList(bool*);
	void showList();
	void changeIssue();
	void removeIssue(std::vector<std::shared_ptr<Issue>>&, unsigned, std::string);
	void removeIssue();
	void showIssueInfo();
	void assignIssueToSomePerson(bool bSelfAssign = false);
	void selfAssignIssue(){
		assignIssueToSomePerson(true);
	}
	void sendOnReview();
	void confirmSolution();
	void makeReport();
	void sortIssues();
	void sortExactList(std::vector<std::shared_ptr<Issue>>&, short);
	Project(std::string name = "Project name", std::string description = "Project description\nMaximum 2000 characters");
	Project(const Project& proj);
	static std::shared_ptr<Project> currentProject;
};

