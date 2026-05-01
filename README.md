RX Search — GitHub Advanced Search CLI

Overview

GitHub search is powerful, but advanced filtering is often hidden, slow, and inconvenient for developers who need fast and precise repository discovery.

Searching for things like:

- small compiler repositories under 50KB
- active parser implementations in C
- toy interpreters for learning
- maintained backend projects with specific topics
- niche open-source tooling projects

usually requires memorizing GitHub search syntax like:

size:1..30 language:C topic:compiler stars:>10

and navigating through GitHub’s web UI.

That creates unnecessary friction.

RX Search solves this problem by bringing GitHub Advanced Search directly to the terminal with a cleaner workflow, better filtering, and faster developer productivity.

This project is designed for:

- Compiler Engineers
- Backend Engineers
- Infrastructure Engineers
- OSS Contributors
- Security Researchers
- Reverse Engineers
- Technical Recruiters
- Developer Tools Engineers

---

Features

Core Features

- Search repositories by keyword
- Language filtering
- Minimum stars filter
- Minimum forks filter
- Min/Max repository size filtering
- Multi-topic filtering
- Updated-after date filtering
- Sort by stars / forks / updated
- Order by desc / asc

---

Stability Features

- Input validation
- Better formatted terminal output
- GitHub API validation error handling
- Timeout handling
- Rate limit detection
- Clean terminal UX
- Optional filters (only repository name is required)

---

Example Use Cases

Find tiny C compiler repositories

Keyword: compiler
Language: C
Min Size: 1
Max Size: 30
Topic: compiler

---

Find active parser projects

Keyword: parser
Language: C++
Stars: 100
Updated After: 2025-01-01

---

Find toy interpreters for learning

Keyword: interpreter
Language: C
Topic: compiler interpreter

---

Installation

Ubuntu / Debian

Install required libraries:

sudo apt update
sudo apt install libcurl4-openssl-dev
sudo apt install nlohmann-json3-dev

---

Compile

g++ github_search.cpp -o ghsearch -lcurl

---

Run

./ghsearch

---

Usage

Input Fields

[1] Repository Name / Keyword (Required)

Main search keyword.

Examples:

compiler
parser
interpreter
database
virtual machine

---

[2] Language (Optional)

Programming language filter.

Examples:

C
C++
Rust
Go
Python
Java
JavaScript

---

[3] Minimum Stars (Optional)

Only show repositories with at least this number of stars.

Example:

10
100
500

---

[4] Minimum Forks (Optional)

Only show repositories with at least this number of forks.

Example:

5
20
100

---

[5] Min Repo Size KB (Optional)

Minimum repository size in KB.

Example:

1
10
100

---

[6] Max Repo Size KB (Optional)

Maximum repository size in KB.

Example:

30
100
1000

---

[7] Topics (Optional)

GitHub topics separated by spaces.

Examples:

compiler interpreter
parser lexer
virtual-machine runtime

Do not use "#".

Wrong:

#compiler

Correct:

compiler

---

[8] Updated After (Optional)

Filter repositories updated after a specific date.

Format:

YYYY-MM-DD

Example:

2025-01-01

---

[9] Sort (Optional)

Available options:

stars
forks
updated

---

[10] Order (Optional)

Available options:

desc
asc

---

Example Full Search

[1] Repository Name / Keyword: compiler
[2] Language: C
[3] Minimum Stars: 10
[4] Minimum Forks:
[5] Min Repo Size KB: 1
[6] Max Repo Size KB: 30
[7] Topics: compiler interpreter
[8] Updated After:
[9] Sort:
[10] Order:

---

Example Output

============================================================
Total Repository Found : 193
============================================================

[1] minilang

Language       : C
Stars          : 47
Forks          : 7
Size           : 29 KB
Updated        : 2025-10-22T09:31:08Z
URL            : https://github.com/2bt/minilang

Description:
Bootstrapped compiler of a C-like but Python-looking language

---

Why This Project Exists

This project was built from a real developer problem:

GitHub advanced search is powerful, but using it repeatedly through the browser is frustrating.

The goal of RX Search is simple:

Reduce Developer Friction

Less clicking.
Less hidden filters.
Less syntax memorization.
More productive engineering.

This is not just a portfolio project.

It is a real developer productivity tool.

---

Future Roadmap (v5)

Planned improvements:

- GitHub Personal Access Token support
- Pagination support
- CSV export
- JSON export
- TXT export
- Retry logic
- Better terminal UI
- Interactive page navigation
- Saved search presets
- Search history

---

License

GPL License

---

Author

Built for engineers who are tired of fighting GitHub search.
