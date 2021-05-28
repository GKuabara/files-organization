# Project 1 - SQL Functions in C

Project from Files Organization Discipline of University of São Paulo

## Students

Gabriel Alves Kuabara - nUSP 11275043<br/>
Milena Corrêa da Silva - nUSP 11795401

# Introduction

Basically, our program consists of SQL language functionalities built in C. In this project, there are 8 functionalities that works with csv or/and binary files. Half of these files contains information about the bus fleet of the city of Curitiba and the another half contains information about the lines which each of these buses are related.

The csv and binary files have a header containing general information about the file, about registers, and names of each field besides registers with the content of vehicles and lines. In summary, they are structured in according with the following images:

- Vehicle Header<br/>
![Vehicle Header](https://i.imgur.com/c8rAhVH.png)

- Vehicle Register<br/>
![Vehicle Registers](https://i.imgur.com/OpMrJwS.png)

- Line Header<br/>
![Line Header](https://i.imgur.com/erDIblB.png)

- Line Register<br/>
![Line Registers](https://i.imgur.com/2x3xAJt.png)

# Project Planning

In this project, we may prioritize the use of disk memory, not of RAM memory. So we tried to use the minimun of main memory by always maintaining our files updated with every information that we read or write.

## Code Modularization

We distributed our functionalities over six C files. The file **operantions.c** contains the main functions of each functionality of our program, in other words, it contains the "macro" of our program. In the **vehicle.c**, we have the functions related only about the vehicle files structure. In the **line.c**, we have the functions related only about the line files structure. So, in the **global.c** we have functions that can be used by both vehicle and line files. These are mainly functions that treat common parts of both files structures like the identifiers of logic remotion and of register size in the beggining of every data register. Also, the **stream.c** deals mainly with spliting strings, openning files, and readline. At least, the **funcao-fornecida.c** have two funcions offered by the instructors of the discipline. The first one is used to check if the binary file was written correctly and the second is used to read an input with quotes from the terminal. 

## Functionalities

### 1. Vehicle Create Table

From the vehicle csv file, we need to write a binary file with the given specifications. To accomplish our task, we fisrt open the files, treating possible erros. Then, we start our header with standard values, and start to read (using a readline and string lexer) and write (using multiple functions to fwrite each field) each register from the csv into the binary. After it, we update our header with the information got from the insertion of all these register.

### 2. Line Create Table

From the line csv file, we need to write a binary file with the given specifications. To accomplish our task, we fisrt open the files, treating possible erros. Then, we start our header with standard values, and start to read (using a readline and string lexer) and write (using multiple functions to fwrite each field) each register from the csv into the binary. After it, we update our header with the information got from the insertion of all these register.

### 3. Vehicle Select

From the vehicle binary file, we read all the valid register (those which aren't removed) and print them as requested. Firstly, we check the consistency of our binary file, and then read each register (storing into a struct) until the file last byte, printing all **not** removed registers. If we didn't find any register, we print a message error.

### 4. Line Select

From the line binary file, we read all the valid register (those which aren't removed) and print them as requested. Firstly, we check the consistency of our binary file, and then read each register (storing into a struct) until the file last byte, printing all **not** removed registers. If we didn't find any register, we print a message error.

### 5. Vehicle Select Where

From the vehicle binary file, we need to search and print for all the registers containing a specific value in a specific field given by the user. We start by checking our file consistency, following by searching in each register if it contains the 'value' in the requested 'field'. So, we print all the registers containing the 'value' in the 'field'. If there is any register, the program prints a message error.

### 6. Line Select Where

From the line binary file, we need to search and print for all the registers containing a specific value in a specific field given by the user. We start by checking our file consistency, following by searching in each register if it contains the 'value' in the requested 'field'. So, we print all the registers containing the 'value' in the 'field'. If there is any register, the program prints a message error.

### 7. Vehicle Insert Into

The program receives 'n' registers typed by the user that we need to add in the end of the vehicle binary file. First of all, we check the consistency of our file. Following, we read each line/register from the terminal, adding them to the end of the file. At least, we update the header given the number of new inserted register.

### 8. Line Insert Into

The program receives 'n' registers typed by the user that we need to add in the end of the line binary file. First of all, we check the consistency of our file. Following, we read each line/register from the terminal, adding them to the end of the file. At least, we update the header given the number of new inserted register.

# Use Instructions

