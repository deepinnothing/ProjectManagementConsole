This is a project management console application for Windows platform written solely in C++ in a matter of 2 weeks. It has all the basic functionality of project management apps, for instance:
- Creation of user accounts using unique login, password (with simple encryption), name and surname with error-checking;
- Creation, editing and removing projects (if signed in with a project's administrator account);
- Adding features, tasks and reporting bugs with priorities (features can have subtasks);
- Adding/removing project members and giving them specific roles;
- Assigning issues to specific project members;
- Displaying project's issues with filters enabled;
- Generating a summarizing project report and saving it to a *.txt file;
- Tracking issues with specific lists (To do -> In progress -> In review -> Resolved);
- And more.

Things to note:
- All the data is stored locally in the program's folder in binary files;
- The program is better run in a separate console window in fullscreen mode, preferably with lowered scale as well, to avoid displaying issues;
- Use up/down arrow keys to navigate through menus and Ctrl+J for multiline input.

Comparison of project roles and actions they can perform:
| Action | Tester | Developer | Client / Designer | Administrator |
|--------|--------|-----------|-------------------|---------------|
| Display project information | ✅ | ✅ | ✅ | ✅ |
| Display issues | ✅ | ✅ | ✅ | ✅ |
| Sort issues | ✅ | ✅ | ✅ | ✅ |
| Display detailed issue information | ✅ | ✅ | ✅ | ✅ |
| Edit issue | ✅ | ✅ | ✅ | ✅ |
| Delete issue | ✅ | ✅ | ✅ | ✅ |
| Display help info | ✅ | ✅ | ✅ | ✅ |
| Generate report and save to file | ❌ | ❌ | ✅ | ✅ |
| Add new feature (to `To-do` list) | ❌ | ❌ | ✅ | ✅ |
| Generate report and save to file | ❌ | ❌ | ✅ | ✅ |
| Add new task (to `To-do` list) | ❌ | ✅ | ❌ | ✅ |
| Submit an issue solution for review (move to `In review` list) | ❌ | ✅ | ❌ | ✅ |
| Assign an issue to self (move to `In progress` list) | ❌ | ✅ | ❌ | ✅ |
| Report a bug | ✅ | ❌ | ❌ | ✅ |
| Confirm an issue solution (move to `Resolved` list) | ✅ | ❌ | ❌ | ✅ |
| Change project name | ❌ | ❌ | ❌ | ✅ |
| Change project description | ❌ | ❌ | ❌ | ✅ |
| Delete project | ❌ | ❌ | ❌ | ✅ |
| Add new project member | ❌ | ❌ | ❌ | ✅ |
| Change project member role | ❌ | ❌ | ❌ | ✅ |
| Display all project members | ❌ | ❌ | ❌ | ✅ |
| Remove project member | ❌ | ❌ | ❌ | ✅ |
| Assign an issue to a project member | ❌ | ❌ | ❌ | ✅ |
