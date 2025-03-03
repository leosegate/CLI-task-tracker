## CLI-task-tracker is a project used to track and manage your tasks with JSON

### How to use:

(the number "1" used for the examples is the ID of the task.)

### Adding a new task:
```
add "description"
```
### Updating and deleting tasks:
```
update 1 "updated description"
delete 1
```
### Marking a task as in progress or done:
```
mark-in-progress 1
mark-done 1
```
### Listing all tasks:
```
list
```
### Listing tasks by status:
```
list done
list todo
list in-progress
```

### Techniques and technologies used:
- [cJSON](https://github.com/DaveGamble/cJSON)
- C
- JSON
- [roadmap.sh](https://roadmap.sh/projects/task-tracker)
