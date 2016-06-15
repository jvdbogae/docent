/*
 *  OvixModel.cpp
 *
 *  Copyright 2012 by Sara Stymne. All rights reserved.
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

#include "Docent.h"
#include "DocumentState.h"
#include "FeatureFunction.h"
#include "SearchStep.h"
#include "OvixModel.h"

#include <boost/foreach.hpp>

#include <iostream>
#include <map>

using namespace std;

struct OvixModelState
:	public FeatureFunction::State,
	public FeatureFunction::StateModifications
{
	OvixModelState(uint nsents) : tokens(0) {} // :  sentencePairs(nsents) {}

	typedef std::map<Word,uint> TypeMap_;

	TypeMap_ types;
	uint tokens;

	Float score() {
		uint typeSize = types.size();
		return -log(tokens)/log(2-(log(typeSize)/log(tokens+1)));
	}

	void addPhrasePair(const AnchoredPhrasePair& app) {
		BOOST_FOREACH(const Word &w, app.second.get().getTargetPhrase().get()) {
			tokens++;
			types[w]++;
		}
	}

	void removePhrasePair(const AnchoredPhrasePair& app) {
		BOOST_FOREACH(const Word &w, app.second.get().getTargetPhrase().get()) {
			tokens--;
			if (types.find(w)->second == 1) {
				types.erase(w);
			}
			else {
				types[w]--;
			}
		}
	}

	virtual OvixModelState *clone() const {
		return new OvixModelState(*this);
	}
};


struct WordCounter : public std::unary_function<const AnchoredPhrasePair &,Float> {
 	Float operator()(const AnchoredPhrasePair &ppair) const {
 		return Float(-ppair.second.get().getTargetPhrase().get().size());
 	};
};


FeatureFunction::State
*OvixModel::initDocument(
	const DocumentState &doc,
	Scores::iterator sbegin
) const {
	const std::vector<PhraseSegmentation> &segs = doc.getPhraseSegmentations();

	OvixModelState *s = new OvixModelState(segs.size());
	for(uint i = 0; i < segs.size(); i++) {
		BOOST_FOREACH(const AnchoredPhrasePair &app, segs[i]) {
			s->addPhrasePair(app);
		}
	}


	*sbegin = s->score();
	return s;
}

void OvixModel::computeSentenceScores(
	const DocumentState &doc,
	uint sentno,
	Scores::iterator sbegin
) const {
	*sbegin = Float(0);
}

FeatureFunction::StateModifications
*OvixModel::estimateScoreUpdate(
	const DocumentState &doc,
	const SearchStep &step,
	const State *state,
	Scores::const_iterator psbegin,
	Scores::iterator sbegin
) const {
	const OvixModelState *prevstate = dynamic_cast<const OvixModelState *>(state);
	OvixModelState *s = prevstate->clone();

	const std::vector<SearchStep::Modification> &mods = step.getModifications();
	for(std::vector<SearchStep::Modification>::const_iterator it = mods.begin(); it != mods.end(); ++it) {
		// Do Nothing if it is a swap,s ince that don't affect this model
		if (step.getDescription().substr(0,4) != "Swap") {
			PhraseSegmentation::const_iterator from_it = it->from_it;
			PhraseSegmentation::const_iterator to_it = it->to_it;

			for (PhraseSegmentation::const_iterator pit=from_it; pit != to_it; pit++) {
				s->removePhrasePair(*pit);
			}

			BOOST_FOREACH(const AnchoredPhrasePair &app, it->proposal) {
				s->addPhrasePair(app);
			}
		}

	}

	*sbegin = s->score();
	return s;
}

FeatureFunction::StateModifications
*OvixModel::updateScore(
	const DocumentState &doc,
	const SearchStep &step,
	const State *state,
	FeatureFunction::StateModifications *estmods,
	Scores::const_iterator psbegin,
	Scores::iterator estbegin
) const {
	return estmods;
}

FeatureFunction::State
*OvixModel::applyStateModifications(
	FeatureFunction::State *oldState,
	FeatureFunction::StateModifications *modif
) const {
	OvixModelState *os = dynamic_cast<OvixModelState *>(oldState);
	OvixModelState *ms = dynamic_cast<OvixModelState *>(modif);

	os->types.swap(ms->types);
	os->tokens = ms->tokens;

	return oldState;
}
