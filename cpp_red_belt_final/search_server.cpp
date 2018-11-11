#include "search_server.h"

vector<string> SplitIntoWords(const string& line) {
	istringstream words_input(line);
	return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

<<<<<<< HEAD
SearchServer::SearchServer()//:
	//docs_rating_count_duration("Count docs rating"),
	//docs_sort_by_rating_duration("Sort docs by rating"),
	//send_search_results_duration("Send search results")
{
}
=======
SearchServer::SearchServer():
	docs_rating_count_duration("Count docs rating"),
	docs_sort_by_rating_duration("Sort docs by rating"),
	send_search_results_duration("Send search results")
{}
>>>>>>> parent of 7bd52cc...  single-threaded version that meets performance requirements

SearchServer::SearchServer(istream& document_input): 
	docs_rating_count_duration("Count docs rating"),
	docs_sort_by_rating_duration("Sort docs by rating"),
	send_search_results_duration("Send search results")
{
	UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {

	auto f = async(launch::async, &SearchServer::UpdateDocumentBaseAsync, this, ref(document_input));
	base_update.push_back(move(f));
	
}

void SearchServer::UpdateDocumentBaseAsync(istream& document_input) {

	InvertedIndex new_index;

	for (string current_document; getline(document_input, current_document); ) {
		new_index.Add(move(current_document));
	}

	index.GetAccess().ref_to_value = move(new_index);

	base_empty = false;

}

void SearchServer::DocsRatingCount(vector<DocRating>& docid_count, const vector<string>& words) {
	
	ADD_DURATION(docs_rating_count_duration);

	for (const auto& word : words) {
<<<<<<< HEAD
		for (const DocRating docid_num : index.GetAccess().ref_to_value.Lookup(word)) {
			if(docid_num.docid < docid_count.size()){
				docid_count[docid_num.docid].docid = docid_num.docid;
				docid_count[docid_num.docid].rating += docid_num.rating;
			}
=======
		for (const size_t docid : index.Lookup(word)) {
			docid_count[docid].first = docid;
			docid_count[docid].second++;
>>>>>>> parent of 7bd52cc...  single-threaded version that meets performance requirements
		}
	}
	
}

void SearchServer::DocsSortByRating(vector<DocRating>& docid_count) {

	ADD_DURATION(docs_sort_by_rating_duration);

	auto middle = docid_count.size() > 5 ? docid_count.begin() + 5 : docid_count.end();

	partial_sort(
		begin(docid_count),
		middle,
		end(docid_count),
		[](DocRating lhs, DocRating rhs) {
			int64_t lhs_docid = lhs.docid;
			auto lhs_hit_count = lhs.rating;
			int64_t rhs_docid = rhs.docid;
			auto rhs_hit_count = rhs.rating;
			return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
		}
	);

}

void SearchServer::SendSearchResults(string& current_query,
									 vector<DocRating>& sorted_docs,
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

<<<<<<< HEAD
	//TotalDuration total_query_duration("Total");
	//ADD_DURATION(total_query_duration);

	auto f = async(launch::async, &SearchServer::AddQueriesStreamAsync, this, ref(query_input), ref(search_results_output));
	queries.push_back(move(f));
	//f.get();
}

void SearchServer::AddQueriesStreamAsync(
	istream& query_input, ostream& search_results_output
) {

	while (base_empty) {
		std::this_thread::sleep_for(std::chrono::microseconds(1));
	}

	vector<DocRating> docid_count(index.GetAccess().ref_to_value.GetDocsNum());

	for (string current_query; getline(query_input, current_query); ) {
=======
	vector<pair<size_t, size_t>> docid_count(index.GetDocsNum());

	for (string current_query; getline(query_input, current_query); ) {

		fill(docid_count.begin(), docid_count.end(), make_pair(0, 0));
>>>>>>> parent of 7bd52cc...  single-threaded version that meets performance requirements

		const auto words = SplitIntoWords(current_query);

		docid_count.assign(docid_count.size(), { 0, 0 });
		DocsRatingCount(docid_count, words);
		DocsSortByRating(docid_count);
		SendSearchResults(current_query, docid_count, search_results_output);

	}

}

void InvertedIndex::Add(const string& document) {
	docs.push_back(document);

	const size_t docid = docs.size() - 1;
	for (const auto& word : SplitIntoWords(document)) {
<<<<<<< HEAD
		if (word_docid_index_map.count(word) == 0) {
			word_docid_index_map[word][docid] = 0;
			index[word].push_back({docid, 1});
		}		
		else {
			if (word_docid_index_map[word].count(docid) == 0) {
				index[word].push_back({ docid, 1 });
				word_docid_index_map[word][docid] = index[word].size() - 1;
			}
			else {
				size_t docid_index = word_docid_index_map[word][docid];
				index[word][docid_index].rating++;
			}
		}
=======
		index[word].push_back(docid);
>>>>>>> parent of 7bd52cc...  single-threaded version that meets performance requirements
	}
}

<<<<<<< HEAD
vector<DocRating> InvertedIndex::Lookup(const string& word) const {
=======
list<size_t> InvertedIndex::Lookup(const string& word) const {
>>>>>>> parent of 7bd52cc...  single-threaded version that meets performance requirements
	if (auto it = index.find(word); it != index.end()) {
		return it->second;
	} else {
		return {};
	}
}
