//* -*-	Mode:C++; c-basic-offset:4; tab-width:4; indent-tabs-mode:t -*- */
/* 
 * Copyright (C) 2011 Kazuya Sakai, Allright Received.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty, including any implied warranty for 
 * merchantability or fitness for a particular purpose. Under no
 * circumstances shall Kazuya Sakai be liable for any use of, 
 * misuse of, or inability to use this software, including incidental 
 * and consequential damages.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * Author: Kazuya Sakai, The Ohio State University
 * 
 * Greedy forwarding for ns2 version 2.34
 */
 
#include "greedy_nbr.h"

GreedyNbr::GreedyNbr(nsaddr_t id, float x, float y) :addr_(id), x_(x), y_(y) {
}

