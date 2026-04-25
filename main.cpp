/*
========================================================
GitHub Advanced Search CLI PRO v3
Language : C++
Library  : libcurl + nlohmann/json

FEATURES:
- Only repository name is required
- All other filters optional
- Pretty formatted output
- Total repository count
- Cleaner terminal UX
- GitHub Search API
- Better for portfolio project

INSTALL:

Ubuntu:
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
HELPER
========================================================
*/

void line()
{
    cout << "====================================================\n";
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

/*
========================================================
PRETTY PRINT RESULT
========================================================
*/

void printResults(const string& response)
{
    try
    {
        json data = json::parse(response);

        /*
        GitHub validation error
        */
        if (data.contains("message") &&
            data["message"] == "Validation Failed")
        {
            line();
            cout << "GitHub API Validation Error\n";
            line();
            cout << data.dump(4) << "\n";
            return;
        }

        int total = data["total_count"];

        line();
        cout << "Total Repository Found : " << total << "\n";
        line();
        cout << "\n";

        if (data["items"].empty())
        {
            cout << "No repositories found.\n";
            return;
        }

        int index = 1;

        for (auto& repo : data["items"])
        {
            cout << "[" << index++ << "] "
                 << repo.value("name", "N/A") << "\n\n";

            cout << "Language    : "
                 << repo.value("language", "N/A") << "\n";

            cout << "Stars       : "
                 << repo.value("stargazers_count", 0) << "\n";

            cout << "Forks       : "
                 << repo.value("forks_count", 0) << "\n";

            cout << "Size        : "
                 << repo.value("size", 0) << " KB\n";

            cout << "Updated     : "
                 << repo.value("updated_at", "N/A") << "\n";

            cout << "URL         : "
                 << repo.value("html_url", "N/A") << "\n";

            cout << "\nDescription :\n";

            if (repo["description"].is_null())
                cout << "No description\n";
            else
                cout << repo["description"] << "\n";

            cout << "\n----------------------------------------------------\n\n";
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
    string size;
    string topic;
    string updated;
    string sort;
    string order;

    line();
    cout << "        GitHub Advanced Search CLI PRO v3\n";
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

    cout << "[5] Repository Size KB (optional | ex: 30): ";
    getline(cin, size);

    cout << "[6] Topic (optional | without #): ";
    getline(cin, topic);

    cout << "[7] Updated After (optional | YYYY-MM-DD): ";
    getline(cin, updated);

    cout << "[8] Sort (optional | stars/forks/updated): ";
    getline(cin, sort);

    cout << "[9] Order (optional | desc/asc): ";
    getline(cin, order);

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

    if (!size.empty())
        query += "+size:>=" + size;

    if (!topic.empty())
        query += "+topic:" + topic;

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
    FETCH
    */

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
             << endl;
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
