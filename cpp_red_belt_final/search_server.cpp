#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>

vector<string> SplitIntoWords(const string& line) {
	istringstream words_input(line);
	return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer():
	docs_rating_count_duration("Count docs rating"),
	docs_sort_by_rating_duration("Sort docs by rating"),
	send_search_results_duration("Send search results")
{}

SearchServer::SearchServer(istream& document_input): 
	docs_rating_count_duration("Count docs rating"),
	docs_sort_by_rating_duration("Sort docs by rating"),
	send_search_results_duration("Send search results")
{
	UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
	InvertedIndex new_index;

	for (string current_document; getline(document_input, current_document); ) {
		new_index.Add(move(current_document));
	}

	index = move(new_index);
}

void SearchServer::DocsRatingCount(vector<pair<size_t, size_t>>& docid_count, const vector<string>& words) {
	
	ADD_DURATION(docs_rating_count_duration);

	for (const auto& word : words) {
		for (const size_t docid : index.Lookup(word)) {
			docid_count[docid].first = docid;
			docid_count[docid].second++;
		}
	}
	
}

void SearchServer::DocsSortByRating(vector<pair<size_t, size_t>>& docid_count) {

	ADD_DURATION(docs_sort_by_rating_duration);

	auto middle = docid_count.size() > 5 ? docid_count.begin() + 5 : docid_count.end();

	partial_sort(
		begin(docid_count),
		middle,
		end(docid_count),
		[](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
			int64_t lhs_docid = lhs.first;
			auto lhs_hit_count = lhs.second;
			int64_t rhs_docid = rhs.first;
			auto rhs_hit_count = rhs.second;
			return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
		}
	);

}

void SearchServer::SendSearchResults(string& current_query,
									 vector<pair<size_t, size_t>>& sorted_docs,
									 ostream& search_results_output) {

	ADD_DURATION(send_search_results_duration);

	search_results_output << current_query << ':';
	for (auto[docid, hitcount] : Head(sorted_docs, 5)) {
		if (hitcount > 0) {
			search_results_output << " {"
				<< "docid: " << docid << ", "
				<< "hitcount: " << hitcount << '}';
		}
	}
	search_results_output << endl;

}

void SearchServer::AddQueriesStream(
	istream& query_input, ostream& search_results_output
) {

	vector<pair<size_t, size_t>> docid_count(index.GetDocsNum());

	for (string current_query; getline(query_input, current_query); ) {

		fill(docid_count.begin(), docid_count.end(), make_pair(0, 0));

		const auto words = SplitIntoWords(current_query);
		DocsRatingCount(docid_count, words);
		DocsSortByRating(docid_count);
		SendSearchResults(current_query, docid_count, search_results_output);

	}

}

void InvertedIndex::Add(const string& document) {
	docs.push_back(document);

	const size_t docid = docs.size() - 1;
	for (const auto& word : SplitIntoWords(document)) {
		index[word].push_back(docid);
	}
}

list<size_t> InvertedIndex::Lookup(const string& word) const {
	if (auto it = index.find(word); it != index.end()) {
		return it->second;
	} else {
		return {};
	}
}
