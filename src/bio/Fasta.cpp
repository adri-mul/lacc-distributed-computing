#include "lacc/bio/Fasta.hpp"

#include <stdexcept>
#include <fstream>
#include <iostream>

namespace lacc::bio {
std::vector<Read> read_fasta_like(const std::string& path) {
    // STUDENT TODO:
    // Read a DNA text file and return a vector of Read{id, seq} records.
    //
    // Required starter format for this repository:
    //   - Input is plain text.
    //   - A record begins with a header line that starts with '>'.
    //   - The remainder of the header line is the read identifier.
    //   - One or more sequence lines follow and belong to that record.
    //   - Reading continues until the next header line or end of file.
    //   - Blank lines may be ignored.
    //
    // Example:
    //   >read_001
    //   ACTGACTG
    //   ACTG
    //   >read_002
    //   TTTTGGGA
    //
    // Required behavior:
    //   - Throw std::runtime_error if the file cannot be opened.
    //   - Throw std::runtime_error if no valid records are found.
    //   - Throw std::runtime_error on malformed structure (for example, sequence
    //     content before the first header).
    //   - Keep parsing deterministic.
    // function variables
    std::vector<Read> fasta_data;
    Read *data = new Read;

    std::ifstream file(path);   // opens file
    if (!file.is_open()) {
        throw std::runtime_error("file " + path + " cannot be opened");
    }
    std::string s;
    bool valid_record_found;
    int record_counter = 0;
    char c;
    file.get(c);
    if (c != '>') {
        throw std::runtime_error("malformed structure");
    } else {
        getline(file, data->id);
        valid_record_found = true;
        record_counter++;
    }
    
    while (std::getline(file, s)) {
        if (s.front() == '>') {     // compares front character with '>'
            fasta_data.push_back(*data);   // saves data to fasta_data
            data = new Read;
            data->id = s.substr(1,s.length()-1);
            record_counter++;
        } else {
            data->seq.append(s);
        }
    }
    fasta_data.push_back(*data);
    
    if (!valid_record_found) {
        throw std::runtime_error("No valid records found in file");
    }
    
    return fasta_data;
}

} // namespace lacc::bio
