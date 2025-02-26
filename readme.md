# people

**what is?**  ー This is a CLI tool for tracking the number of days since you have made contact with people.  
**why a CLI?**  ー 99% of people don't know what a terminal is -- lightweight & discreet  
**OS?** ー This is compiled to a Unix Executable File and should run on Linux, macOS, Ubuntu, Solaris etc. I've avoided using POSIX libs and adapting the source code for Windows will be trivial.  
    
[usage](#usage)  
[installation](#installation)  
[walkthrough](#walkthrough)  
[making accessible system-wide with an alias](#making-this-program-accessible-system-wide-with-an-alias)  

## usage
```
./people  add     forename surname
./people  check   forename surname
./people  check   all
./people  forget  forename surname
./people  forget  all
./people  days    number [see Walkthrough below]
```

## installation
Download this repo using the green `<>Code` button above, or via the terminal:  
```
git clone https://github.com/CallumBeaney/people
```
If you have any problems running the executable file due to e.g. a `bad CPU` error, cd into the `src` directory, and make the program with e.g. GCC make utility like so: 
```
gcc -o people main.c helpers.c
```
Unless you wish to make this program accessible system-wide using an alias, or have the above execution problems, you only need the executable named `people`.  

## walkthrough
1. First, add some people:  
```
./people add Fred Durst 
    Added Fred Durst to your People List
./people add David Hume
    Added David Hume to your People List
```
2. Check one of them:
```
./people check fred durst
    fred durst - last checked 0 days ago
```
Case isn't important; spelling is.  
You will be prompted with an offer to reset the date associated with this name.   
  
3. Imagine: 2 months earlier, 'Joanna Newsom' was added to this list, and you want to see the full list.  

```       
./people check all

David Hume      - last checked  0       days ago
Fred Durst      - last checked  0       days ago
Joanna Newsom   - last checked  59      days ago  ! IMPORTANT
```
Again, you will be prompted with an offer to reset the dates associated with these names to 0 days.  
That `! IMPORTANT` is triggered when your checking interval stored in the file `timespan` is smaller than the elapsed days since you last checked a person.  
You won't have a `timespan` file initially; one will be made automatically.  
  
4. To change that interval, run the following command with your preferred number of days before you're given a warning:  
```
./people days 100                 
    Interval to compare dates set to: 100 days 

./people check all        
    David Hume      - last checked  0       days ago
    Fred Durst      - last checked  0       days ago
    Joanna Newsom   - last checked  59      days ago
```
The program will auto-sort your name list alphabetically when you do a checkall.  
  
5. To stop checking in on a person:  
```
./people forget fred durst
```
6. Your `yellowpages` file would then look like below. You can add names manually; the syntax must follow the below example & there must be an empty final line in the file.  If it gets corrupted, you can delete it, and when you next run an ADD command, a fresh file will be generated.
```
1   David Hume,29/3/2023
2   Joanna Newsom,29/1/2023
3      
```
7. To forget all people in your list, just use:
```
./people forget all
```

## making this program accessible system-wide with an alias
For the sake of being able to quickly check without moving to the containing folder and executing from there like a standard C executable, here is how you can create an alias and add it to your PATH. You can find instructions on how to do this online for your specific operating system. If using a macOS, the process will look like this:  

1. Open a terminal and type:
```
nano ~/.zshrc
```  
2. In the file, add the following lines with the path to the executable 'people' replacing the paths below. Do not remove `:$PATH` from within the quotes.  
```
export PATH="/Users/username/Applications/program_folder/people:$PATH"
alias people='/Users/username/Applications/program_folder/people'
```  
3. Then exit and save by `^X` and submitting `Y` to confirm save on exit, and then reload your profile file by running:
```
source ~/.zshrc
```  
Now you can call your program from anywhere with just e.g. 'people check all', **but** it will need to be configured to only read and write your People List data from a specific folder.  
  
4. Move to the `/src/` directory containing the source files, open `constants.h`, and at lines 4 and 5, change the inside of the quotes to the path of that folder. It should look initially look something like this:
``` 
#define TIMEFILE "timespan"
#define NAMEFILE "yellowpages"
```  
5. ...and with the given path, it might look something like this:
``` 
#define TIMEFILE "/Users/userName/Applications/people/src/timespan"
#define NAMEFILE "/Users/userName/Applications/people/src/yellowpages"  
```  
If you want to make the files invisible in the main folder system, you can prepend with a dot on macOS e.g. `~/people/.timespan`.  
  
6. Make the program with e.g. GCC make utility from the `/src/` directory: 
```
gcc -o people main.c helpers.c
```  
7. You can now run People like this:

```
% people add John Titor        
	Added John Titor to your People List
% people check John titor
	John titor - last checked 0 days ago
    Reset number of days passed to 0?
    Y/N: n
```
