/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/fec/alist.h>

alist::alist(const char * fname)
    : data_ok(false) {
    read(fname);
}

std::vector<std::vector<char> > alist::get_matrix() {
    std::vector<std::vector<char> > mat;
    mat.resize(M);
    for ( int i = 0; i < M; i++ ) {
        mat[i].resize(N);
        for ( int j = 0; j < N; j++ ) {
            mat[i][j] = char(0);
        }
        for ( int k = 0; k < num_mlist[i]; k++ ) {
            mat[i][mlist[i][k] - 1] = char(1);
        }
    }
    return mat;
}

std::vector< std::vector<int> > alist::get_mlist() {
    return mlist;
}

std::vector< std::vector<int> > alist::get_nlist() {
    return nlist;
}

std::vector<int> alist::get_num_mlist() {
    return num_mlist;
}

std::vector<int> alist::get_num_nlist() {
    return num_nlist;
}

void alist::read(const char * fname) {
    std::ifstream file;
    std::string line;
    std::stringstream ss;
    
    file.open(fname);
    if(!(file.is_open())) {
        std::cout << "Could not open the file" << std::endl;
    }

    // Parse N and M
    std::getline(file, line);
    ss << line;
    ss >> N >> M;
    num_nlist.resize(N);
    num_mlist.resize(M);
    nlist.resize(N);
    mlist.resize(M);
    ss.seekg(0, std::ios::end);
    ss.clear();

    // parse max_num_n and max_num_m
    std::getline(file, line);
    ss << line;
    ss >> max_num_nlist >> max_num_mlist;
    ss.seekg(0, std::ios::end);
    ss.clear();

    // Parse weight of each column n
    std::getline(file, line);
    ss << line;
    for (int i = 0; i < N; i++ ) {
        ss >> num_nlist[i];
        nlist[i].resize(num_nlist[i]);
    }
    ss.seekg(0, std::ios::end);
    ss.clear();

    // Parse weight of each row m
    std::getline(file, line);
    ss << line;
    for (int i = 0; i < M; i++ ) { 
        ss >> num_mlist[i];
        mlist[i].resize(num_mlist[i]);
    }
    ss.seekg(0, std::ios::end);
    ss.clear();

    // Parse indices with non zero entries in ith column
    for (int column = 0; column < N; column++) {
        std::getline(file, line);
        ss << line;
        for (int entry = 0; entry < num_nlist[column]; entry++) {
            ss >> nlist[column][entry];
        }
        ss.seekg(0, std::ios::end);
        ss.clear();
    }

    // Parse indices with non zero entries in ith row
    for (int row = 0; row < M; row++) {
        std::getline(file, line);
        ss << line;
        for (int entry = 0; entry < num_mlist[row]; entry++) {
            ss >> mlist[row][entry];
        }
        ss.seekg(0, std::ios::end);
        ss.clear();
    }

    file.close();
    data_ok = true;
}

void alist::write(const char * fname) const
{
    if (!data_ok) {
        std::cout << "Data not ok, exiting" << std::endl;
        exit(1);
    }
    // Else
    std::ofstream file(fname, std::ofstream::out);
    // Write N and M
    file << N << " " << M << std::endl;
    file << max_num_nlist << " " << max_num_mlist << std::endl;
    // Write column weights
    for (size_t i = 0; i < num_nlist.size() - 1; i++) {
        file << num_nlist[i] << " ";
    }
    file << num_nlist[num_nlist.size() - 1] << std::endl;
    // Write row weights
    for (size_t i = 0; i < num_mlist.size() - 1; i++) {
        file << num_mlist[i] << " ";
    }
    file << num_mlist[num_mlist.size() - 1] << std::endl;
    // Write non zero indices in the ith column
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < num_nlist[i] - 1; j++) {
            file << nlist[i][j] << "\t";
        }
        file << nlist[i][num_nlist[i] - 1];
        file << std::endl;
    }
    // Write non zero indices in the ith row
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < num_mlist[i] - 1; j++) {
            file << mlist[i][j] << "\t";
        }
        file << mlist[i][num_mlist[i] - 1];
        file << std::endl;
    }
    file.close();
}

int alist::get_N() {
    return N;
}

int alist::get_M() {
    return M;
}

int alist::get_max_num_nlist() {
    return max_num_nlist;
}

int alist::get_max_num_mlist() {
    return max_num_mlist;
}

void alist::print_nlist_i(int column) {
    std::cout << "Indices in column " << column << std::endl;
    for (int i = 0; i < num_nlist[column] - 1; i++) {
        std::cout << nlist[column][i] << ", ";
    }
    std::cout << nlist[column][num_nlist[column] - 1] << std::endl;
}

void alist::print_mlist_i(int row) {
    std::cout << "Indices in row " << row << std::endl;
    for (int i = 0; i < num_mlist[row] - 1; i++) {
        std::cout << mlist[row][i] << ", ";
    }
    std::cout << mlist[row][num_mlist[row] - 1] << std::endl;
}
