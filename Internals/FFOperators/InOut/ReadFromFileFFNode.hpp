/*
    This file is part of PiCo.
    PiCo is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    PiCo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with PiCo.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * ReadFromFileFFNode.hpp
 *
 *  Created on: Dec 7, 2016
 *      Author: misale
 */

#ifndef INTERNALS_FFOPERATORS_INOUT_READFROMFILEFFNODE_HPP_
#define INTERNALS_FFOPERATORS_INOUT_READFROMFILEFFNODE_HPP_

#include <ff/node.hpp>
#include <Internals/utils.hpp>
#include <Internals/Types/Token.hpp>
#include <Internals/Types/Microbatch.hpp>

using namespace ff;

template <typename Out>
class ReadFromFileFFNode: public ff_node {
public:
    ReadFromFileFFNode(std::string filename_) :
            filename(filename_)
    {}


	void* svc(void* in){
	    std::ifstream infile(filename);
	    std::string line;
	    mb_t *microbatch = new mb_t(MICROBATCH_SIZE);
	    if (infile.is_open()) {

	        /* get a line */
            while (getline(infile, line)) {
                microbatch->push_back(Token<Out>(line));

                /* send out micro-batch if complete */
                if (microbatch->full()) {
                    ff_send_out(reinterpret_cast<void*>(microbatch));
                    microbatch = new mb_t(MICROBATCH_SIZE);
                }
            }
            infile.close();

            /* send out the remainder micro-batch */
            if (!microbatch->empty()) {
                ff_send_out(reinterpret_cast<void*>(microbatch));
            }
            else {
                delete microbatch;
            }
        }
        else
        {
            fprintf(stderr, "Unable to open file %s\n", filename.c_str());
        }
#ifdef DEBUG
		fprintf(stderr, "[READ FROM FILE MB-%p] In SVC: SEND OUT PICO_EOS\n", this);
#endif
		ff_send_out(PICO_EOS);
		return EOS;
	}

private:
	typedef Microbatch<Token<Out>> mb_t;
    std::string filename;
};



#endif /* INTERNALS_FFOPERATORS_INOUT_READFROMFILEFFNODE_HPP_ */
