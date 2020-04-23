/*
 * Copyright (C) 2020  christian <irqmask@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <algorithm>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

constexpr int BYTES_PER_LINE = 16;

std::string get_filename(std::string path_and_filename)
{
    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    const size_t last_slash_idx = path_and_filename.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        path_and_filename.erase(0, last_slash_idx + 1);
    }

    // Remove extension if present.
    const size_t period_idx = path_and_filename.rfind('.');
    if (std::string::npos != period_idx)
    {
        path_and_filename.erase(period_idx);
    }
    return path_and_filename;
}

std::vector<uint8_t> read_file(const std::string &path_and_filename)
{
    struct stat stat_result;
    if (stat(path_and_filename.c_str(), &stat_result) != 0) {
        throw std::runtime_error("Unable to read file!");
    }

    std::vector<uint8_t> bytes(stat_result.st_size);
    std::fstream file(path_and_filename, std::ios::in | std::ios::binary);
    file.read(reinterpret_cast<char*>(bytes.data()), stat_result.st_size);
    file.close();

    return bytes;
}

void write_c_file(const std::string &path_and_filename, const std::vector<uint8_t> &bytes)
{
    if (bytes.size() == 0) {
        throw std::runtime_error("Input file is empty. No output file will be created!");
    }
    std::ofstream cfile(path_and_filename);
    if (!cfile.is_open()) {
        throw std::runtime_error("Unable to open file to write!");
    }

    std::string filename = get_filename(path_and_filename);
    std::string varname = "data_" + filename;
    cfile << "/**" << std::endl;
    cfile << " * @file " << filename << ".c" << std::endl;
    cfile << " */" << std::endl;
    cfile << "#include <stdint.h>" << std::endl;
    cfile << "" << std::endl;
    cfile << "/** Size in bytes of " << varname << "*/" << std::endl;
    std::string d = varname;
    for (auto & c: d) c = toupper(c);
    cfile << "#define " << d << "_SIZE " << bytes.size() << std::endl;
    cfile << "" << std::endl;
    cfile << "uint8_t " << varname << "[] = {" << std::endl;
    cfile << "    ";

    int value_count = 0;
    for (uint8_t b : bytes) {
        cfile << "0x" << std::hex << std::setw(2) << std::setfill('0') << int(b);
        value_count < (BYTES_PER_LINE - 1) ? cfile << ", " : cfile << ",";
        value_count++;
        if (value_count >= BYTES_PER_LINE) {
            cfile << std::endl << "    ";
            value_count = 0;
        }
    }
    if (value_count > 0) {
        cfile << std::endl;
    }
    cfile << "};" << std::endl;

    cfile.close();
}

void print_usage(std::string name)
{
    std::cout << "Usage: " << name << " <path and filename>" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        print_usage(argv[0]);
        return -EINVAL;
    }

    try {
        std::string infile = std::string(argv[1]);

        std::vector<uint8_t> bytes = read_file(infile);
        std::cerr << bytes.size() << " read from file " << infile << std::endl;

        std::string outfile = get_filename(infile) + ".c";
        write_c_file(outfile, bytes);
    }
    catch (std::exception &e) {
        std::cerr << "ERROR " << e.what() << std::endl;
        return -1000;
    }

    return 0;
}
