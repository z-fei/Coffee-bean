# Workflow
  1. At begin, recursive traversal all the disk to generate HASH map for files.
  2. After init, you can input a name to search(Case insensitive and no suffix required)，then find the file from HASH map generate from step 1.
  3. List the finding result.
  4. If the result contained folder, you can double click the item in the list, it will recursive traversal all the files in folder.
  5. Then, it will popup a modal to list the folder detail.

# Design idea
  1. Establish HASH map at init, it will improve efficiency for future tasks.
  2. Using a thread pool to recursive traversal the folder.
  3. Using MVC to realize high polymerization and low coupling.
  4. Use virtual lists to improve display speed.