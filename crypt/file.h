// -*- c++ -*-
//
// $Id$
//
// Copyright (C) 2008, 2009  Rafael Ostertag
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

#ifndef _FILE_H
#define _FILE_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STRING
# include <string>
#endif

#ifdef HAVE_LIST
# include <list>
#endif

#include "yapetexception.h"

#include "bdbuffer.h"
#include "structs.h"
#include "key.h"
#include "partdec.h"

namespace YAPET {
    /**
     * @brief Class for storing and retrieving encrypted data to and
     * from disk
     *
     * This class takes care of storing and retrieving encrypted
     * password records to and from disk.
     *
     * Each file created by this class starts with a unencrypted
     * recognition string which currently consists of the 8 bytes
     * "YAPET1.0" as depicted below.
     *
@verbatim
+--------+--------+--------+--------+--------+--------+--------+--------+
|   Y    |   A    |   P    |   E    |   T    |   1    |   .    |   0    |
| 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte |
+--------+--------+--------+--------+--------+--------+--------+--------+
@endverbatim
     *
     * After the recognition string a 4 byte unsigned integer which is
     * stored in big-endian order follows. This indicator is read to
     * determine how many bytes to read in order to get the encrypted
     * header.
     *
@verbatim
+--------+--------+--------+--------+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted header exactly as many bytes    |
|        indicated by the prefix             |
+--------+--------+--------+--------+--...---+
@endverbatim
     *
     * The decrypted header is 25 bytes in size. The first byte
     * indicates the version of the file. The next 20 bytes are used
     * as control string. After decryption, the control string is
     * compared to the predefined clear text control string, in order
     * to find out whether or not the key used to decrypt was the same
     * used to encrypt.
     *
@verbatim
+--------+
|Version |
| 1 byte |
+--------+--------+--------+--...---+
|          Control String           |
|             20 bytes              |
+--------+--------+--------+--...---+
|  Time when the Password  |
|    was set (4 bytes)     |
+--------+--------+--------+
@endverbatim
    *
    * Each encrypted password record is prefixed by a 4 byte unsigned
    * integer which is stored in big-endian order. The methods take
    * care returning them in the appropriate order of the host
    * system. That integer is used to indicate the length of the
    * following encrypted data chunk.
    *
@verbatim
+--------+--------+--------+--------+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted password record of exactly as   |
|   many bytes as indicated by the prefix    |
+--------+--------+--------+--------+--...---+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted password record of exactly as   |
|   many bytes as indicated by the prefix    |
+--------+--------+--------+--------+--...---+
	      [ . . . ]
@endverbatim
    *
    * Putting this together, an encrypted file created by this class
    * looks like this
    *
@verbatim
+--------+--------+--------+--------+--------+--------+--------+--------+
|   Y    |   A    |   P    |   E    |   T    |   1    |   .    |   0    |
| 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte | 1 byte |
+--------+--------+--------+--------+--------+--------+--------+--------+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted header exactly as many bytes    |
|        indicated by the prefix             |
+--------+--------+--------+--------+--...---+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted password record of exactly as   |
|   many bytes as indicated by the prefix    |
+--------+--------+--------+--------+--...---+
|   Length indicator in big-endian  |
|         order (4 bytes)           |
+--------+--------+--------+--------+--...---+
|  Encrypted password record of exactly as   |
|   many bytes as indicated by the prefix    |
+--------+--------+--------+--------+--...---+
	      [ . . . ]
@endverbatim
    *
    * Instances of this class keeps the file open for the lifetime
    * of the instance.
    *
    * When saving a password record list, the file is reopened with
    * the \c O_TRUNC specified. The recognition string and header are
    * copied over from the former version of the file.
    *
    * @sa Record, FileHeader, PasswordRecord
    */
    class File {
	private:
	    /**
	     * @brief The file descriptor of the password file
	     *
	     * The file descriptor of the password file.
	     */
	    int fd;
	    /**
	     * @brief The file name of the file
	     *
	     * The file name of the file.
	     */
	    std::string filename;
	    /**
	     * @brief The modification time of the file.
	     *
	     * Holds the modification time of the file. It has to be
	     * updated each time a write occurs.
	     *
	     * This is used to detect file modification made outside
	     * this class.
	     */
	    time_t mtime;


	    //! Creates and opens a new file.
	    void openCreate() throw(YAPETException);
	    //! Opens an existing file
	    void openNoCreate() throw(YAPETException);
	    //! Returns the last modification time of the open file
	    time_t lastModified() const throw(YAPETException);
	    //! Seek to a position relative to the current offset
	    void seekCurr(off_t offset) const throw(YAPETException);
	    //! Seek to an absolute offset
	    void seekAbs(off_t offset) const throw(YAPETException);
	    //! Prepare the file for saving the password records.
	    void preparePWSave() throw(YAPETException);

	protected:
	    /**
	     * @brief A 16 bits word
	     *
	     * Represents a word of 16 bits. Used for little-big
	     * endian conversion.
	     */
	    struct WORD {
		    uint8_t a;
		    uint8_t b;
	    };

	    /**
	     * @brief A 32 bits double word.
	     *
	     * Represents a double word of 32 bits. Used for
	     * little-big endian conversion.
	     */
	    struct DWORD {
		    WORD a;
		    WORD b;
	    };

	    /**
	     * @brief Used for little-big endian conversion.
	     *
	     * Union for converting between little and big endian
	     * order.
	     */
	    union ENDIAN {
		    /**
		     * @brief 32 bits unsigned integer in host order.
		     *
		     * 32 bits unsigned integer in host order.
		     */
		    uint32_t abcd;
		    DWORD dword;
	    };

#ifndef WORDS_BIGENDIAN
	    //! The given integer will be converted to big endian
	    //! format
	    uint32_t uint32_to_disk(uint32_t i) const;
	    //! The given integer will be converted to the endianess
	    //! of the host
	    uint32_t uint32_from_disk(uint32_t i) const;
#else
	    /**
	     * @brief The given integer will be converted to big
	     * endian format
	     *
	     * Converts the length indicator provided to the big endian byte
	     * order, suitable for writing to disk.
	     *
	     * @param i the length indicator in host byte order
	     *
	     * @return an unsigned 32 bits integer in big-endian format.
	     */
	    inline uint32_t uint32_to_disk(uint32_t i) const { return i; }
	    /**
	     * @brief The given integer will be converted to the
	     * endianess of the host
	     *
	     * Converts the length indicator read from the file to the
	     * host byte order. The length indicator is always stored
	     * in big endian order.
	     *
	     * @param i the length indicator as read from the file
	     *
	     * @return an unsigned 32 bits integer in host byte order.
	     */
	    inline uint32_t uint32_from_disk(uint32_t i) const { return i; }
#endif // WORDS_BIGENDIAN

	    //! Seeks to the first password record length indicator in
	    //! the file
	    void seekDataSection() const throw(YAPETException);

	    //! Reads from the current offset in the file
	    BDBuffer* read() const throw(YAPETException);
	    //! Writes at the current offset in the file
	    void write(const BDBuffer& buff,
		       bool forceappend=false,
		       bool forcewrite=false)
		throw(YAPETException, YAPETRetryException);
	    //! Indicates whether or not the file is empty
	    bool isempty() const throw(YAPETException);
	    //! Initializes an empty file
	    void initFile(const Key& key) throw(YAPETException);
	    //! Writes the given header encrypted to the file
	    void writeHeader(const Record<FileHeader>& header,
			     const Key& key)
		throw(YAPETException);
	    //! Writes the given encrypted header to the file
	    void writeHeader(const BDBuffer& enc_header) throw(YAPETException);
	    //! Reads the encrypted header
	    BDBuffer* readHeader() const throw(YAPETException);
	    //! Validates the given key
	    void validateKey(const Key& key)
		throw(YAPETException,YAPETInvalidPasswordException);

	public:
	    //! Constructor
	    File(const std::string& fn,
		 const Key& key,
		 bool create=false)
		throw(YAPETException);
	    File(const File& f) throw(YAPETException);
	    ~File();

	    //! Saves a password record list.
	    void save(std::list<PartDec>& records) throw(YAPETException);
	    //! Reads the stored password records from the file.
	    std::list<PartDec> read(const Key& key) const throw(YAPETException);
	    //! Returns the file name of the current file.
	    std::string getFilename() const { return filename; }
	    //! Sets a new encryption key for the current file.
	    void setNewKey(const Key& oldkey, const Key& newkey)
		throw(YAPETException);
	    time_t getMasterPWSet(const Key& key) const
		throw(YAPETException,YAPETInvalidPasswordException);
	    //! Returns the time the master password was set
	    const File& operator=(const File& f) throw(YAPETException);
    };
}
#endif // _FILE_H
