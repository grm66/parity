/****************************************************************\
*                                                                *
* Copyright (C) 2007 by Markus Duft <markus.duft@salomon.at>     *
*                                                                *
* This file is part of parity.                                   *
*                                                                *
* parity is free software: you can redistribute it and/or modify *
* it under the terms of the GNU Lesser General Public License as *
* published by the Free Software Foundation, either version 3 of *
* the License, or (at your option) any later version.            *
*                                                                *
* parity is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  *
* GNU Lesser General Public License for more details.            *
*                                                                *
* You should have received a copy of the GNU Lesser General      *
* Public License along with parity. If not,                      *
* see <http://www.gnu.org/licenses/>.                            *
*                                                                *
\****************************************************************/

#include "CoffMsDosStub.h"

namespace parity
{
	namespace binary
	{
		MsDosStub::MsDosStub(void* data, size_t length)
			: data_(0)
			, size_(length)
		{
			data_ = ::malloc(size_);

			if(!data_)
				throw utils::Exception("cannot allocate memory for MsDos Stub");

			::memcpy(data_, data, size_);
		}

		MsDosStub::~MsDosStub()
		{
			if(data_)
				::free(data_);
		}
	}
}

