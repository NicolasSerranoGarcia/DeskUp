#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "desk_up_lookup_table/desk_up_lookup_table.h"

namespace fs = std::filesystem;
using DeskUp::Utils::LookUpTable;

static void print_usage(const char* prog){
    std::cerr << "Usage:\n"
                 "  " << prog << " <file> <command> [args]\n\n"
                 "Commands:\n"
                 "  count                       - Print total slots (including deleted)\n"
                 "  add <name>                  - Add an entry\n"
                 "  del <name>                  - Delete an entry (keeps empty line)\n"
                 "  dir <name>                  - Print directory path for entry\n"
                 "  list                        - List non-empty entries with line numbers\n"
                 "  raw                         - Dump raw file contents with line numbers\n";
}

static int cmd_list(const fs::path& p){
    std::ifstream in(p);
    if(!in.is_open()){
        std::cerr << "Failed to open file: " << p << "\n";
        return 1;
    }
    std::string line;
    if(!std::getline(in, line)){
        std::cerr << "Empty or unreadable file\n";
        return 1;
    }
    unsigned long long count = 0;
    try{ count = std::stoull(line); } catch(...){ std::cerr << "Invalid first line (count)\n"; return 1; }

    std::size_t idx = 1;
    while(std::getline(in, line)){
        if(!line.empty()){
            std::cout << idx << ": " << line << "\n";
        }
        idx++;
    }
    std::cerr << "(slots reported by count: " << count << ")\n";
    return 0;
}

static int cmd_raw(const fs::path& p){
    std::ifstream in(p, std::ios::binary);
    if(!in.is_open()){
        std::cerr << "Failed to open file: " << p << "\n";
        return 1;
    }
    std::string s;
    std::size_t lineNo = 0;
    while(std::getline(in, s)){
        std::cout << lineNo++ << ": '" << s << "'\n";
    }
    return 0;
}

int main(int argc, char** argv){
    if(argc < 3){
        print_usage(argv[0]);
        return 1;
    }

    fs::path filePath = argv[1];
    std::string cmd = argv[2];

    try{
        if(cmd == "list"){
            return cmd_list(filePath);
        }
        if(cmd == "raw"){
            return cmd_raw(filePath);
        }

        LookUpTable table(filePath);

        if(cmd == "count"){
            std::cout << table.getNumEntries() << "\n";
            return 0;
        } else if(cmd == "add"){
            if(argc < 4){
                std::cerr << "add requires <name>\n";
                return 1;
            }
            std::string name = argv[3];
            auto line = table.addEntry(name);
            std::cout << "added at line " << line << "\n";
            return 0;
        } else if(cmd == "del"){
            if(argc < 4){
                std::cerr << "del requires <name>\n";
                return 1;
            }
            std::string name = argv[3];
            auto line = table.deleteEntry(name);
            std::cout << "deleted line " << line << " (emptied)\n";
            return 0;
        } else if(cmd == "dir"){
            if(argc < 4){
                std::cerr << "dir requires <name>\n";
                return 1;
            }
            std::string name = argv[3];
            std::cout << table.getDirFromEntry(name).string() << "\n";
            return 0;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    } catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}