
# FlexiBase Shell

#### Welcome to FlexiBase Shell, a simple and efficient database management tool.

FlexiBase Shell is a lightweight, efficient database management tool designed for simplicity and performance. Built from the ground up, all database structures, including indexing, retrieval, and key-value storage, are custom-designed to meet the needs of modern data management.

### Key Features
- Custom-Built Structures: All data structures, including B-trees for indexing and hashmaps for key-value search, are implemented from scratch to ensure optimal performance and flexibility.
- Intuitive Shell Commands: FlexiBase Shell offers an easy-to-use command-line interface, allowing users to perform various database operations efficiently.
- Efficient Data Management: With B-trees for indexing and hashmaps for key-value searches, FlexiBase Shell ensures fast and reliable data retrieval.
- Compact and Detailed Views: Users can display data in both compact and detailed formats, making it easier to manage and analyze stored information.



## Tech Stack

**Language:** C++ **_(all data-structures build from scratch)_**

## Installation
Clone the repository

```bash
   git clone https://github.com/BazilSuhail/FlexiBase.git
   cd my-project
```
Then compile the **Source.cpp** with any Compiler and the shell will appear

## Shell Commands

At first you can type **help** in the shell to know about the offered query commands

### Create a database
```bash
  create <database_name>
```
#### Example:
```bash
  create students_db
```
 
 ### Use a Database
```bash
  Use a Database
```
#### Example:
```bash
  use <database_name>
```
### Exit the Database
```bash
  exit db

``` 
### Insert a New Object in Databse
```bash
  db.insert([key1: value1], [key2: value2], ...)

```
#### Example:
```bash
  db.insert([Name: John], [Semester: 2], [CGPA: 3.45], [University: MIT], [Status: Current])
```
### Insert or Update a Key-Value Pair in a Document
```bash
  db.insertkey(<id>, <key>, <value>)

```
#### Example:
```bash
  db.insertkey(1055, Address, 123 Main St)

```
### Delete a Key-Value Pair in a Document
```bash
  db.deletekey(<id>, <key>)
```
#### Example:
```bash
  db.deletekey(1055, Address)
``` 
### Find an Object by ID
```bash
  db.find(<id>)

```
#### Example:
```bash
  db.find(1055)
```

### Show All Objects (Compact Format)
```bash
  db.show()
``` 
### Show All Objects (Detailed Format)
```bash
  db.show.pretty()
```
### Find Objects by Key-Value Pair
```bash
  db.findbyvalue(<key>, <value>)

```
#### Example:
```bash
  db.findbyvalue(Name, John)

``` 
