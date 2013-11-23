// $Id$
//
// Copyright (C) 2008-2010  Rafael Ostertag
//
// This file is part of YAPET.
//
// YAPET is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// YAPET is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// YAPET.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU GPL version 3 section 7
//
// If you modify this program, or any covered work, by linking or combining it
// with the OpenSSL project's OpenSSL library (or a modified version of that
// library), containing parts covered by the terms of the OpenSSL or SSLeay
// licenses, Rafael Ostertag grants you additional permission to convey the
// resulting work.  Corresponding Source for a non-source form of such a
// combination shall include the source code for the parts of OpenSSL used as
// well as that of the covered work.
//

#include "bdbuffer.h"

#include <cstdlib>
#include <cstring>

#include "intl.h"

using namespace YAPET;

/**
 * Allocates \c s bytes of memory on the heap.
 *
 * @param s size of the memory chunk to be allocated in bytes
 *
 * @return the pointer to the start of the allocated memory.
 *
 * @throw YAPETException if the memory could not be allocated.
 */
uint8_t*
BDBuffer::alloc_mem (uint32_t s) throw (YAPETException) {
    uint8_t* tmp = (uint8_t*) std::malloc (s);

    if (tmp == 0)
        throw YAPETException (_ ("Memory exhausted") );

    return tmp;
}

/**
 * Frees the memory associated with the pointer provided. Before the
 * memory is deallocated, \c s bytes of the memory region starting
 * from the pointer \c d will be zero'ed out.
 *
 * @param d pointer to the memory to be free'd
 *
 * @param s size of the memory chunk. Needed to clear out the memory.
 */
void
BDBuffer::free_mem (uint8_t* d, uint32_t s) {
    std::memset (d, 0, s);
    std::free (d);
}

/**
 * Initializes the object by allocating \c is bytes.
 *
 * @param is number of bytes to be allocated.
 */
BDBuffer::BDBuffer (uint32_t is) throw (YAPETException) : _size (is) {
    data = alloc_mem (_size);
}

/**
 * Initializes the object, but does not allocate memory.
 *
 * If the object is created using this constructor, functions
 * returning pointer to the buffer will return \c 0
 */
BDBuffer::BDBuffer() : _size (0), data (0) { }

BDBuffer::BDBuffer (const BDBuffer& ed) throw (YAPETException) {
    if (ed.data == 0) {
        data = 0;
        _size = 0;
        return;
    }

    data = alloc_mem (ed._size);
    std::memcpy (data, ed.data, ed._size);
    _size = ed._size;
}

/**
 * Deallocates the memory and zero'es it out.
 *
 * @sa BDBuffer::free_mem()
 */
BDBuffer::~BDBuffer() {
    if (data == 0) return;

    free_mem (data, _size);
}

/**
 * Resizes the buffer to the given size \c ns. It does not utilize \c
 * realloc(), so you can be sure the pointer to the resized buffer
 * will change.
 *
 * The content of the old buffer memory region will be copied over to
 * the new location. If the newly allocated memory is smaller than the
 * former buffer size, the content of the old buffer will be truncated
 * to fill the entire space of the new buffer. If the new size is
 * larger than the old size, the entire old buffer is preserved while
 * copying.
 *
 * @param ns the new size of the memory chunk serving as buffer
 */
void
BDBuffer::resize (uint32_t ns) throw (YAPETException) {
    if (data == 0) {
        data = alloc_mem (ns);
        _size = ns;
        return;
    }

    uint8_t* newbuf = alloc_mem (ns);

    if (ns > _size)
        std::memcpy (newbuf, data, _size);
    else
        std::memcpy (newbuf, data, ns);

    free_mem (data, _size);
    _size = ns;
    data = newbuf;
}

/**
 * Returns a pointer to the \c n-th byte of the buffer, where \c
 * n=pos. \c pos is zero-based, the first byte of the buffer is at
 * position \c 0.
 *
 * It checks \c pos for a valid value. In case \c pos overruns the
 * buffer, a \c std::out_of_range exception is thrown.
 *
 * @param pos the index of the byte to retrieve.
 *
 * @return the pointer to the byte at position specified.
 *
 * @throw std::out_of_range exception if \c pos is not a valid
 * index.
 */
uint8_t*
BDBuffer::at (uint32_t pos) throw (std::out_of_range) {
    if (pos > (_size - 1) )
        throw std::out_of_range (_ ("Position out of range") );

    return data + pos;
}

/**
 * Returns a pointer to the \c n-th byte of the buffer, where \c
 * n=pos. \c pos is zero-based, the first byte of the buffer is at
 * position \c 0.
 *
 * It checks \c pos for a valid value. In case \c pos overruns the
 * buffer, a \c std::out_of_range exception is thrown.
 *
 * @param pos the index of the byte to retrieve.
 *
 * @return the pointer to the byte at position specified.
 *
 * @throw std::out_of_range exception if \c pos is not a valid index.
 */
const uint8_t*
BDBuffer::at (uint32_t pos) const throw (std::out_of_range) {
    if (pos > (_size - 1) )
        throw std::out_of_range (_ ("Position out of range") );

    return data + pos;
}

const BDBuffer&
BDBuffer::operator= (const BDBuffer & ed) {
    if (this == &ed) return *this;

    if (data != 0)
        free_mem (data, _size);

    if (ed.data != 0) {
        data = alloc_mem (ed._size);
        std::memcpy (data, ed.data, ed._size);
    } else {
        data = 0;
    }

    _size = ed._size;
    return *this;
}
