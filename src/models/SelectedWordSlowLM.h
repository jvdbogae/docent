/*
 *  SelectedWordSlowLM.cpp
 *
 *  Copyright 2012 by Joerg Tiedemann. All rights reserved.
 *
 *  This file is part of Docent, a document-level decoder for phrase-based
 *  statistical machine translation.
 *
 *  Docent is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  Docent is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along with
 *  Docent. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef docent_SelectedWordSlowLM_h
#define docent_SelectedWordSlowLM_h

#include "FeatureFunction.h"

struct SelectedWordSlowLMFactory {
	static FeatureFunction *createNgramModel(const Parameters &);
};

#endif
