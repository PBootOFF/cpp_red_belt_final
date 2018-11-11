#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

//#include "durations.cpp"

using namespace std;

class InvertedIndex {
public:

	void Add(const string& document);
	vector<pair<size_t, size_t>> Lookup(const string& word) const;

	const string& GetDocument(size_t id) const {
		return docs[id];
	}

	size_t GetDocsNum() {
		return docs.size();
	}

private:
	map<string, vector<pair<size_t, size_t>>> index;
	map<string, map<size_t, size_t>> word_docid_index_map;
	vector<string> docs;

};

class SearchServer {
public:
	SearchServer();
	explicit SearchServer(istream& document_input);
	void UpdateDocumentBase(istream& document_input);
	void AddQueriesStream(istream& query_input, ostream& search_results_output);
	void DocsRatingCount(vector<pair<size_t, size_t>>& docid_count, const vector<string>& words);
	void DocsSortByRating(vector<pair<size_t, size_t>>& docid_count);
	void SendSearchResults(string& current_query, vector<pair<size_t, size_t>>& sorted_docs, ostream& search_results_output);

private:
	InvertedIndex index;
	//TotalDuration docs_rating_count_duration;
	//TotalDuration docs_sort_by_rating_duration;
	//TotalDuration send_search_results_duration;
};
