#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Issue.h"

// Functional capability / project feature
class Feature : public Issue
{
    friend Project;
    std::vector<std::shared_ptr<Issue>> issues;

public:
    short getType() { return FEATURE; }
    bool create();
    bool change();
    void print();
    void saveFeatureSubtasks();
    void loadFeatureSubtasks();
    void addIssue(std::shared_ptr<Issue>);
    void showInfo(std::string);
    std::vector<std::shared_ptr<Issue>>& getIssues() { return issues; }
    friend std::ostream& operator<<(std::ostream& stream, Feature& feat);

    Feature(std::string name = "Feature name",
            std::string description = "High-level description of the feature\nMaximum 2000 characters",
            int priority = Medium) : Issue(name, description, priority)
    {
    }

    Feature(const Feature& feat) : Issue(feat) { this->issues = feat.issues; }
};
