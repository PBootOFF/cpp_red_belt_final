#pragma once

#include "iterator_range.h"
#include "synchronized.h"

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <future>
#include <thread>
#include <functional>
#include <chrono>


//#include "durations.cpp"

using namespace std;

struct DocRating
{
	size_t docid;
	size_t rating;
};

class InvertedIndex {
public:

	void Add(const string& document);
	vector<DocRating> Lookup(const string& word) const;

	const string& GetDocument(size_t id) const {
		return docs[id];
	}

	size_t GetDocsNum() {
		return docs.size();
	}

private:
	map<string, vector<DocRating>> index;
	map<string, map<size_t, size_t>> word_docid_index_map;
	vector<string> docs;

};



class SearchServer {
public:
	SearchServer();
	//~SearchServer();
	explicit SearchServer(istream& document_input);
	void UpdateDocumentBase(istream& document_input);
	void UpdateDocumentBaseAsync(istream& document_input);
	void AddQueriesStream(istream& query_input, ostream& search_results_output);
	void AddQueriesStreamAsync(istream& query_input, ostream& search_results_output);
	void DocsRatingCount(vector<DocRating>& docid_count, const vector<string>& words);
	void DocsSortByRating(vector<DocRating>& docid_count);
	void SendSearchResults(string& current_query, vector<DocRating>& sorted_docs, ostream& search_results_output);

private:
	Synchronized<InvertedIndex> index;
	vector<future<void>> base_update;
	bool base_empty = true;
	vector<future<void>> queries;
	//TotalDuration docs_rating_count_duration;
	//TotalDuration docs_sort_by_rating_duration;
	//TotalDuration send_search_results_duration;
};
