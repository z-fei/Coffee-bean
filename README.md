# Workflow
  1. At beginning, recursive traversal all the disk to generate HASH map for files.
  2. After init, you can input a name to search(Case insensitive and no suffix required)，then find the file from HASH map generate from step 1.
  3. List the finding result.
  4. If the result contained folder, you can double click the item in the list, it will recursive traversal all the files in folder.
  5. Then, it will popup a modal to list the folder detail.

# Design idea
  1. Use HASH map at init, it will improve efficiency for future tasks.
  2. Use a thread pool to recursive traversal the folder.
  3. Use MVC to realize high polymerization and low coupling.
  4. Use virtual lists to improve display speed.

# Note
  If there is a warning about V142 tool, please replace v142 to v141(your build tool version) the Find.vcxproj file, then retarget solution. 

![image](https://github.com/z-fei/Coffee-bean/blob/master/Sequence%20diagram.png?raw=true)
