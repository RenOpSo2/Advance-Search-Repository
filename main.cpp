/*
========================================================
GitHub Advanced Search CLI PRO v4
Language : C++
Library  : libcurl + nlohmann/json

UPGRADE v4:
- Better stable version
- Proper min/max repo size filter
- Multiple topics support
- Better formatted output
- Input validation
- Retry-safe curl setup
- Timeout handling
- Total repository count
- Clean terminal UX
- Optional sort/order
- Only repository name required

INSTALL:
sudo apt install libcurl4-openssl-dev
sudo apt install nlohmann-json3-dev

COMPILE:
g++ github_search.cpp -o ghsearch -lcurl

RUN:
./ghsearch
========================================================
*/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

/*
========================================================
CURL CALLBACK
========================================================
*/

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output)
{
    size_t total = size * nmemb;
    output->append((char*)contents, total);
    return total;
}

/*
========================================================
HELPERS
========================================================
*/

void line()
{
    cout << "============================================================\n";
}

bool isNumber(const string& s)
{
    if (s.empty()) return false;

    for (char c : s)
    {
        if (!isdigit(c))
            return false;
    }

    return true;
}

string encodeQuery(const string& str)
{
    string result;

    for (char c : str)
    {
        if (c == ' ')
            result += "%20";
        else
            result += c;
    }

    return result;
}

vector<string> splitTopics(const string& input)
{
    vector<string> topics;
    string word;
    stringstream ss(input);

    while (ss >> word)
    {
        topics.push_back(word);
    }

    return topics;
}

/*
========================================================
PRETTY PRINT
========================================================
*/

void printRepo(const json& repo, int index)
{
    cout << "[" << index << "] "
         << repo.value("name", "N/A")
         << "\n\n";

    cout << left << setw(15) << "Language"
         << ": "
         << repo.value("language", "N/A")
         << "\n";

    cout << left << setw(15) << "Stars"
         << ": "
         << repo.value("stargazers_count", 0)
         << "\n";

    cout << left << setw(15) << "Forks"
         << ": "
         << repo.value("forks_count", 0)
         << "\n";

    cout << left << setw(15) << "Size"
         << ": "
         << repo.value("size", 0)
         << " KB\n";

    cout << left << setw(15) << "Updated"
         << ": "
         << repo.value("updated_at", "N/A")
         << "\n";

    cout << left << setw(15) << "URL"
         << ": "
         << repo.value("html_url", "N/A")
         << "\n";

    cout << "\nDescription:\n";

    if (repo["description"].is_null())
        cout << "No description\n";
    else
        cout << repo.value("description", "No description") << "\n";

    cout << "\n------------------------------------------------------------\n\n";
}

void printResults(const string& response)
{
    try
    {
        json data = json::parse(response);

        if (data.contains("message") &&
            data["message"] == "Validation Failed")
        {
            line();
            cout << "GitHub API Validation Error\n";
            line();
            cout << data.dump(4) << "\n";
            return;
        }

        if (data.contains("message") &&
            data["message"] == "API rate limit exceeded")
        {
            line();
            cout << "GitHub API Rate Limit Exceeded\n";
            line();
            cout << "Gunakan GitHub Token di versi berikutnya.\n";
            return;
        }

        int total = data.value("total_count", 0);

        line();
        cout << "Total Repository Found : " << total << "\n";
        line();
        cout << "\n";

        if (!data.contains("items") || data["items"].empty())
        {
            cout << "No repositories found.\n";
            return;
        }

        int index = 1;

        for (const auto& repo : data["items"])
        {
            printRepo(repo, index++);
        }
    }
    catch (exception& e)
    {
        line();
        cout << "JSON Parse Error\n";
        line();
        cout << e.what() << "\n";
    }
}

/*
========================================================
MAIN
========================================================
*/

int main()
{
    string keyword;
    string language;
    string stars;
    string forks;
    string minSize;
    string maxSize;
    string topicInput;
    string updated;
    string sort;
    string order;

    line();
    cout << "        GitHub Advanced Search CLI PRO v4\n";
    line();
    cout << "\n";

    /*
    INPUT
    */

    cout << "[1] Repository Name / Keyword (required): ";
    getline(cin, keyword);

    if (keyword.empty())
    {
        cout << "\n[ERROR] Repository name wajib diisi.\n";
        return 1;
    }

    cout << "[2] Language (optional | C/C++/Python/etc): ";
    getline(cin, language);

    cout << "[3] Minimum Stars (optional | number only): ";
    getline(cin, stars);

    cout << "[4] Minimum Forks (optional | number only): ";
    getline(cin, forks);

    cout << "[5] Min Repo Size KB (optional | ex: 1): ";
    getline(cin, minSize);

    cout << "[6] Max Repo Size KB (optional | ex: 100): ";
    getline(cin, maxSize);

    cout << "[7] Topics (optional | ex: compiler interpreter): ";
    getline(cin, topicInput);

    cout << "[8] Updated After (optional | YYYY-MM-DD): ";
    getline(cin, updated);

    cout << "[9] Sort (optional | stars/forks/updated): ";
    getline(cin, sort);

    cout << "[10] Order (optional | desc/asc): ";
    getline(cin, order);

    /*
    BASIC VALIDATION
    */

    if (!stars.empty() && !isNumber(stars))
    {
        cout << "\n[ERROR] Stars harus angka.\n";
        return 1;
    }

    if (!forks.empty() && !isNumber(forks))
    {
        cout << "\n[ERROR] Forks harus angka.\n";
        return 1;
    }

    if (!minSize.empty() && !isNumber(minSize))
    {
        cout << "\n[ERROR] Min size harus angka.\n";
        return 1;
    }

    if (!maxSize.empty() && !isNumber(maxSize))
    {
        cout << "\n[ERROR] Max size harus angka.\n";
        return 1;
    }

    /*
    BUILD QUERY
    */

    string query = keyword;

    if (!language.empty())
        query += "+language:" + language;

    if (!stars.empty())
        query += "+stars:>=" + stars;

    if (!forks.empty())
        query += "+forks:>=" + forks;

    /*
    Proper size range
    */

    if (!minSize.empty() && !maxSize.empty())
    {
        query += "+size:" + minSize + ".." + maxSize;
    }
    else if (!minSize.empty())
    {
        query += "+size:>=" + minSize;
    }
    else if (!maxSize.empty())
    {
        query += "+size:<=" + maxSize;
    }

    /*
    Multi topic support
    */

    vector<string> topics = splitTopics(topicInput);

    for (const auto& topic : topics)
    {
        query += "+topic:" + topic;
    }

    if (!updated.empty())
        query += "+pushed:>=" + updated;

    query = encodeQuery(query);

    /*
    BUILD URL
    */

    string url =
        "https://api.github.com/search/repositories?q=" + query;

    if (!sort.empty())
        url += "&sort=" + sort;

    if (!order.empty())
        url += "&order=" + order;

    url += "&per_page=10";

    /*
    CURL SETUP
    */

    CURL* curl;
    CURLcode res;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl)
    {
        cout << "\n[ERROR] Failed init CURL.\n";
        return 1;
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(
        headers,
        "User-Agent: GitHubSearchCLI"
    );

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    /*
    stability improvements
    */

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    cout << "\n";
    line();
    cout << "Fetching repositories from GitHub...\n";
    line();
    cout << "\n";

    res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        cerr << "[CURL ERROR] "
             << curl_easy_strerror(res)
             << "\n";
    }
    else
    {
        printResults(response);
    }

    /*
    CLEANUP
    */

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}
