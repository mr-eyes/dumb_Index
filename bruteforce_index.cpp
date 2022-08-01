#include <parallel_hashmap/phmap.h>
#include "parallel_hashmap/phmap_dump.h"
#include <lib.hpp>
#include <iostream>

using std::cout;

#define PHMAP_SUBMAPS 6
#define cores 8

using PHMAP_HASH_SET = phmap::parallel_flat_hash_set<
    uint32_t,
    phmap::priv::hash_default_hash<uint32_t>,
    phmap::priv::hash_default_eq<uint32_t>,
    std::allocator<uint32_t>,
    1,
    std::mutex>;

using PHMAP_HASH_TO_COLORS = phmap::parallel_flat_hash_map<uint64_t, PHMAP_HASH_SET,
    std::hash<uint64_t>, std::equal_to<uint64_t>,
    std::allocator<std::pair<const uint64_t, PHMAP_HASH_SET>>,
    PHMAP_SUBMAPS,
    std::mutex>;

/*
struct file_info {
    string path;
    string prefix;
    string extension;
    string basename;
};
*/

int main(int argc, char** argv) {

    string bins_dir = argv[1];
    auto all_files = glob(bins_dir + "/*");
    cout << "scanned " << all_files.size() << " files." << endl;
    phmap::flat_hash_map<string, uint32_t> bin_to_id;
    phmap::flat_hash_map<uint32_t, string> id_to_bin;

    int id_counter = 1;
    for (const auto& file : all_files) {
        bin_to_id[file.basename] = id_counter;
        id_to_bin[id_counter] = file.basename;
        id_counter++;
    }

    PHMAP_HASH_TO_COLORS hash_to_colors;

    for (int i = 0; i < all_files.size(); i++) {
        auto current_file = all_files[i];
        auto bin_id = bin_to_id[current_file.basename];
        std::cerr << bin_id << ": " << current_file.basename << endl;

        // Loading bin
        phmap::flat_hash_set<uint64_t> loaded_bin;
        phmap::BinaryInputArchive ar_in(current_file.path.c_str());
        loaded_bin.phmap_load(ar_in);


        for (const auto& hashed_kmer : loaded_bin) {

            hash_to_colors.lazy_emplace_l(hashed_kmer,
                [bin_id](PHMAP_HASH_TO_COLORS::value_type& v) { v.second.insert(bin_id); },           // called only when key was already present
                [hashed_kmer, bin_id](const PHMAP_HASH_TO_COLORS::constructor& ctor) {
                    PHMAP_HASH_SET tmp_set;
                    tmp_set.insert(bin_id);
                    ctor(hashed_kmer, tmp_set);
                }
            );

        }

    }

    // DUMP
    for (const auto& [hash, colors] : hash_to_colors) {
        cout << hash << ":";
        for (auto const& color : colors) cout << color << ",";
        cout << endl;
    }


}