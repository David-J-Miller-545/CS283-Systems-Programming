## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**:  _Externalizing `get_student(...)` as it's own function is a good strategy because it allows a level of abstraction in the sense of getting a student without worrying how,as well as refactoring common areas that would require to retrieve a student from the database._

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** _The student that is produced here is within the stack and the second the function returns to it's caller, the space that this student was declared is returned to the stack. This meaning that whenever this function's caller tries to access the student through the address they were provided previously, it may no longer be good memory._

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** _This implementation is sound and wouldn't cause any bugs because the space is requested from the heap and the address to the spot on the heap is returned instead of the address on the stack as we saw in teh previous solution. The only issue this solution has is that it infers that the return value will be used and properly freed by this functions caller so that the memory address doesn't get lost and cause a memory leak._  


4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** _This is because each student is 64 bytes, so when adding the student at id=1 it is important to remember the that first 64 bytes are empty however, to get from the beginning to end it would take 64 * (1 + 1) = 64 * 2 = 128. This behavior is also similar to whenever we insert the student at id=3: 64 * (3 + 1) = 64 * 4 = 256. More or less the file size reported by ls will be the number of bytes (empty or not) between the beginning and the end of the file._

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** _The reason why the total storage used on the disk remained unchanged when we added students with ids of 1, 3, and 63 is because they were all contained within the same block of data. A block of data here is seem to be 4K or more specifically 4096, which also happens to be 64^2 and so conveniently the size of one of our students is 64 bytes. This meaning that a single block of memory can store exactly 64 students. Hence, once we added a student with id=64 which would be our 65th section of 64 bytes of data, a new block of 4K memory had to be allocated on the disk to contain this data. It is also important to note that the blocks are only allocated for bytes within a file that are not empty (comes into play in the following question)._

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:**  _As we see above adding a student with a very large student id of 99999 will greatly increase the number of bytes from the start of the file to the end of the file. Specifically the number of bytes would be 64 * (99999 + 1) = 64 * 100000 = 6400000. However, if we assume before we added this new student that we only had students with ids of 1, 3, 63, and 64; we would only have the first two 4K blocks of memory allocated for this file. When we add the new student with id=99999, the physical disk does not care that it is at the end of the file, it only cares that it is not within either of the existing blocks of data so what it does is it allocates one more 4K block of data for this distant student. In summary; ls looks at the logical file size which is how many bytes (empty or not) are between the beginning of the file and the end of the file while du looks at the physical disk usage which allocates blocks of memory for sections of a file that have non-empty data within it._