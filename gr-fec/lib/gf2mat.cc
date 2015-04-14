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

#include <gnuradio/fec/gf2mat.h>
#include <iostream>

GF2Mat::GF2Mat(int m, int n) {
    M = m;
    N = n;
    H.resize(M);
    for ( int i = 0; i < M; i++ ) {
        H[i].resize(N);
        for ( int j = 0; j < N; j++ ) {
            H[i][j] = char(0);
        }
    }
}

GF2Mat::GF2Mat(std::vector <std::vector<char> > X) {
    M = X.size();
    N = X[0].size();
    H.resize(M);
    for ( int i = 0; i < M; i++ ) {
        H[i].resize(N);
        for ( int j = 0; j < N; j++ ){
            H[i][j] = X[i][j];
        }
    }
}

GF2Mat::GF2Mat(alist _list) {
    M = _list.get_M();
    N = _list.get_N();
    H.resize(M);
    for ( int i = 0; i < M; i++ ) {
        H[i].resize(N);
        for ( int j = 0; j < N; j++ ) {
            H[i][j] = char(0);
        }
    }
    H = _list.get_matrix();
}

int GF2Mat::get_M() {
    return M;
}

int GF2Mat::get_N() {
    return N;
}

void GF2Mat::set_element(int i, int j, char val) {
    H[i][j] = val;
}

char GF2Mat::get_element(int i, int j) {
    return H[i][j];
}

void GF2Mat::print_matrix() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << int(H[i][j]) << " ";
        }
        std::cout << '\n';
    }
}

GF2Vec GF2Mat::operator[](int i) {
    GF2Vec row(N);
    row.set_vec(H[i]);
    return row;
}

GF2Vec GF2Mat::get_row(int i) {
    GF2Vec row(N);
    for ( int j = 0; j < N; j++ ){
        row[j] = H[i][j];
    }
    return row;
}

GF2Vec GF2Mat::get_col(int i) {
    GF2Vec col(M);
    for ( int j = 0; j < M; j++ ) {
        col[j] = H[j][i];
    }
    return col;
}

void GF2Mat::add_cols(int i, int j) {
    for ( int row = 0; row < M; row++ ) {
        H[row][i] = H[row][i] ^ H[row][j];
    }
}

void GF2Mat::add_rows(int i, int j) {
    for ( int col = 0; col < N; col++ ) {
        H[i][col] = H[i][col] ^ H[j][col];
    }
}

void GF2Mat::set_row(int row, GF2Vec vec) {
    for ( int j = 0; j < N; j++ ) {
        H[row][j] = vec[j];
    }
}

void GF2Mat::set_col(int col, GF2Vec vec) {
    for ( int j = 0; j < M; j++ ) {
        H[j][col] = vec[j];
    }
}

void GF2Mat::swap_cols(int i, int j) {
    GF2Vec tmp;
    tmp = get_col(i);
    set_col(i, get_col(j));
    set_col(j, tmp);
}

GF2Mat GF2Mat::get_G(std::vector<int> & permute, 
        int & rank) {
    permute.resize(N);
    rank = 0;
    for ( int col = 0; col < N; col++ ) {
        permute[col] = col;
    }
    GF2Mat G(H);
    int diag = 0;
    int limit = M;
    int temp;
    while (diag < limit) {
        bool non_zero = false;
        for ( int col = diag; col < N; col++ ) {
            if ( G.get_element(diag, col) == char(1) ) {
                non_zero = true;
                rank++;
                G.swap_cols(diag, col);
                temp = permute[diag];
                permute[diag] = permute[col];
                permute[col] = temp;
                break;
            }
        }
        if (non_zero) {
            for ( int row = 0; row < diag; row++ ) {
                if (G.get_element(row, diag) == char(1)){
                    G.add_rows(row, diag);
                }
            }
            for ( int row = diag + 1; row < M; row++ ) {
                if (G.get_element(row, diag) == char(1)) {
                    G.add_rows(row, diag);
                }
            }
            diag++;
        }
        else{
            GF2Vec current_row;
            current_row = G.get_row(diag);
            for ( int row = diag; row < limit - 1; row++ ) {
                G.set_row(row, G.get_row(row + 1));
            }
            G.set_row(limit - 1, current_row);
            limit--;
        }
    }
    return G;
}

std::vector<std::vector<char> > GF2Mat::get_H() {
    return H;
}
